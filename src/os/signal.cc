/*
  +----------------------------------------------------------------------+
  | OpenSwoole                                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | hello@swoole.co.uk so we can mail you a copy immediately.            |
  +----------------------------------------------------------------------+
  | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
  +----------------------------------------------------------------------+
*/

#include "openswoole.h"
#include "openswoole_api.h"
#include "openswoole_signal.h"
#include "openswoole_socket.h"
#include "openswoole_reactor.h"

#ifdef HAVE_SIGNALFD
#include <sys/signalfd.h>
#endif

#ifdef HAVE_KQUEUE
#ifdef USE_KQUEUE_IDE_HELPER
#include "helper/kqueue.h"
#else
#include <sys/event.h>
#endif
#endif

using openswoole::Event;
using openswoole::Reactor;
using openswoole::Signal;
using openswoole::SignalHandler;
using openswoole::network::Socket;

#ifdef HAVE_SIGNALFD
static SignalHandler openswoole_signalfd_set(int signo, SignalHandler handler);
static bool openswoole_signalfd_create();
static void openswoole_signalfd_clear();
static int openswoole_signalfd_event_callback(Reactor *reactor, Event *event);
#endif

#ifdef HAVE_KQUEUE
static SignalHandler openswoole_signal_kqueue_set(int signo, SignalHandler handler);
#endif

static void openswoole_signal_async_handler(int signo);

#ifdef HAVE_SIGNALFD
static sigset_t signalfd_mask;
static int signal_fd = 0;
static pid_t signalfd_create_pid;
static Socket *signal_socket = nullptr;
#endif
static Signal signals[OSW_SIGNO_MAX];
static int _lock = 0;

char *openswoole_signal_to_str(int sig) {
    static char buf[64];
    snprintf(buf, sizeof(buf), "%s", strsignal(sig));
    if (strchr(buf, ':') == 0) {
        size_t len = strlen(buf);
        snprintf(buf + len, sizeof(buf) - len, ": %d", sig);
    }
    return buf;
}

/**
 * block all singal
 */
void openswoole_signal_block_all(void) {
    sigset_t mask;
    sigfillset(&mask);
    int ret = pthread_sigmask(SIG_BLOCK, &mask, nullptr);
    if (ret < 0) {
        openswoole_sys_warning("pthread_sigmask() failed");
    }
}

/**
 * set new signal handler and return origin signal handler
 */
SignalHandler openswoole_signal_set(int signo, SignalHandler func, int restart, int mask) {
    // ignore
    if (func == nullptr) {
        func = SIG_IGN;
    }
    // clear
    else if ((long) func == -1) {
        func = SIG_DFL;
    }

    struct sigaction act {
    }, oact{};
    act.sa_handler = func;
    if (mask) {
        sigfillset(&act.sa_mask);
    } else {
        sigemptyset(&act.sa_mask);
    }
    act.sa_flags = 0;
    if (sigaction(signo, &act, &oact) < 0) {
        return nullptr;
    }
    return oact.sa_handler;
}

/**
 * set new signal handler and return origin signal handler
 */
SignalHandler openswoole_signal_set(int signo, SignalHandler handler) {
#ifdef HAVE_SIGNALFD
    if (OpenSwooleG.use_signalfd) {
        return openswoole_signalfd_set(signo, handler);
    } else
#endif
    {
#ifdef HAVE_KQUEUE
        // SIGCHLD can not be monitored by kqueue, if blocked by SIG_IGN
        // see https://www.freebsd.org/cgi/man.cgi?kqueue
        // if there's no main reactor, signals cannot be monitored either
        if (signo != SIGCHLD && osw_reactor()) {
            return openswoole_signal_kqueue_set(signo, handler);
        } else
#endif
        {
            signals[signo].handler = handler;
            signals[signo].activated = true;
            signals[signo].signo = signo;
            return openswoole_signal_set(signo, openswoole_signal_async_handler, 1, 0);
        }
    }
}

static void openswoole_signal_async_handler(int signo) {
    if (osw_reactor()) {
        osw_reactor()->singal_no = signo;
    } else {
        // discard signal
        if (_lock) {
            return;
        }
        _lock = 1;
        openswoole_signal_callback(signo);
        _lock = 0;
    }
}

void openswoole_signal_callback(int signo) {
    if (signo >= OSW_SIGNO_MAX) {
        openswoole_warning("signal[%d] number is invalid", signo);
        return;
    }
    SignalHandler callback = signals[signo].handler;
    if (!callback) {
        openswoole_error_log(
            OSW_LOG_WARNING, OSW_ERROR_UNREGISTERED_SIGNAL, OSW_UNREGISTERED_SIGNAL_FMT, openswoole_signal_to_str(signo));
        return;
    }
    callback(signo);
}

SignalHandler openswoole_signal_get_handler(int signo) {
    if (signo >= OSW_SIGNO_MAX) {
        openswoole_warning("signal[%d] numberis invalid", signo);
        return nullptr;
    } else {
        return signals[signo].handler;
    }
}

void openswoole_signal_clear(void) {
#ifdef HAVE_SIGNALFD
    if (OpenSwooleG.use_signalfd) {
        openswoole_signalfd_clear();
    } else
#endif
    {
        int i;
        for (i = 0; i < OSW_SIGNO_MAX; i++) {
            if (signals[i].activated) {
#ifdef HAVE_KQUEUE
                if (signals[i].signo != SIGCHLD && osw_reactor()) {
                    openswoole_signal_kqueue_set(signals[i].signo, nullptr);
                } else
#endif
                {
                    openswoole_signal_set(signals[i].signo, (SignalHandler) -1, 1, 0);
                }
            }
        }
    }
    osw_memset_zero(&signals, sizeof(signals));
}

#ifdef HAVE_SIGNALFD
void openswoole_signalfd_init() {
    sigemptyset(&signalfd_mask);
    osw_memset_zero(&signals, sizeof(signals));
}

/**
 * set new signal handler and return origin signal handler
 */
static SignalHandler openswoole_signalfd_set(int signo, SignalHandler handler) {
    SignalHandler origin_handler = nullptr;

    if (handler == nullptr && signals[signo].activated) {
        sigdelset(&signalfd_mask, signo);
        osw_memset_zero(&signals[signo], sizeof(Signal));
    } else {
        sigaddset(&signalfd_mask, signo);
        origin_handler = signals[signo].handler;
        signals[signo].handler = handler;
        signals[signo].signo = signo;
        signals[signo].activated = true;
    }

    if (osw_reactor()) {
        if (signal_fd == 0) {
            openswoole_signalfd_create();
        } else {
            sigprocmask(SIG_SETMASK, &signalfd_mask, nullptr);
            signalfd(signal_fd, &signalfd_mask, SFD_NONBLOCK | SFD_CLOEXEC);
        }
        openswoole_signalfd_setup(osw_reactor());
    }

    return origin_handler;
}

static bool openswoole_signalfd_create() {
    if (signal_fd != 0) {
        return false;
    }

    signal_fd = signalfd(-1, &signalfd_mask, SFD_NONBLOCK | SFD_CLOEXEC);
    if (signal_fd < 0) {
        openswoole_sys_warning("signalfd() failed");
        signal_fd = 0;
        return false;
    }
    signal_socket = openswoole::make_socket(signal_fd, OSW_FD_SIGNAL);
    if (sigprocmask(SIG_BLOCK, &signalfd_mask, nullptr) == -1) {
        openswoole_sys_warning("sigprocmask() failed");
        signal_socket->fd = -1;
        signal_socket->free();
        close(signal_fd);
        signal_socket = nullptr;
        signal_fd = 0;
        return false;
    }
    signalfd_create_pid = getpid();
    OpenSwooleG.signal_fd = signal_fd;

    return true;
}

bool openswoole_signalfd_setup(Reactor *reactor) {
    if (signal_fd == 0 && !openswoole_signalfd_create()) {
        return false;
    }
    if (!openswoole_event_isset_handler(OSW_FD_SIGNAL)) {
        openswoole_event_set_handler(OSW_FD_SIGNAL, openswoole_signalfd_event_callback);
        reactor->set_exit_condition(Reactor::EXIT_CONDITION_SIGNALFD, [](Reactor *reactor, size_t &event_num) -> bool {
            event_num--;
            return true;
        });
        reactor->add_destroy_callback([](void *) {
            // child process removes signal socket, parent process will not be able to trigger signal
            if (signal_socket && signalfd_create_pid == getpid()) {
                openswoole_event_del(signal_socket);
            }
        });
    }
    if (!(signal_socket->events & OSW_EVENT_READ) && openswoole_event_add(signal_socket, OSW_EVENT_READ) < 0) {
        return false;
    }
    return true;
}

static void openswoole_signalfd_clear() {
    if (signal_fd) {
        if (sigprocmask(SIG_UNBLOCK, &signalfd_mask, nullptr) < 0) {
            openswoole_sys_warning("sigprocmask(SIG_UNBLOCK) failed");
        }
        if (signal_socket) {
            signal_socket->free();
            signal_socket = nullptr;
        }
        osw_memset_zero(&signalfd_mask, sizeof(signalfd_mask));
    }
    OpenSwooleG.signal_fd = signal_fd = 0;
}

static int openswoole_signalfd_event_callback(Reactor *reactor, Event *event) {
    struct signalfd_siginfo siginfo;
    ssize_t n = read(event->fd, &siginfo, sizeof(siginfo));
    if (n < 0) {
        openswoole_sys_warning("read from signalfd failed");
        return OSW_OK;
    }
    if (siginfo.ssi_signo >= OSW_SIGNO_MAX) {
        openswoole_warning("unknown signal[%d]", siginfo.ssi_signo);
        return OSW_OK;
    }
    if (signals[siginfo.ssi_signo].activated) {
        SignalHandler handler = signals[siginfo.ssi_signo].handler;
        if (handler == SIG_IGN) {
            return OSW_OK;
        } else if (handler) {
            handler(siginfo.ssi_signo);
        } else {
            openswoole_error_log(OSW_LOG_WARNING,
                             OSW_ERROR_UNREGISTERED_SIGNAL,
                             OSW_UNREGISTERED_SIGNAL_FMT,
                             openswoole_signal_to_str(siginfo.ssi_signo));
        }
    }

    return OSW_OK;
}
#endif

#ifdef HAVE_KQUEUE
/**
 * set new signal handler and return origin signal handler
 */
static SignalHandler openswoole_signal_kqueue_set(int signo, SignalHandler handler) {
    struct kevent ev;
    SignalHandler origin_handler = nullptr;
    Reactor *reactor = osw_reactor();

    // clear signal
    if (handler == nullptr) {
        signal(signo, SIG_DFL);
        osw_memset_zero(&signals[signo], sizeof(Signal));
        EV_SET(&ev, signo, EVFILT_SIGNAL, EV_DELETE, 0, 0, NULL);
    }
    // add/update signal
    else {
        signal(signo, SIG_IGN);
        origin_handler = signals[signo].handler;
        signals[signo].handler = handler;
        signals[signo].signo = signo;
        signals[signo].activated = true;
#ifndef __NetBSD__
        auto sigptr = &signals[signo];
#else
        auto sigptr = reinterpret_cast<intptr_t>(&signals[signo]);
#endif
        // save swSignal* as udata
        EV_SET(&ev, signo, EVFILT_SIGNAL, EV_ADD, 0, 0, sigptr);
    }
    int n = kevent(reactor->native_handle, &ev, 1, nullptr, 0, nullptr);
    if (n < 0 && osw_unlikely(handler)) {
        openswoole_sys_warning("kevent set signal[%d] error", signo);
    }

    return origin_handler;
}
#endif
