#include "test_core.h"
#include "openswoole_signal.h"

#ifdef HAVE_SIGNALFD
static void sig_usr1(int signo) {}

TEST(os_signal, swSignalfd_set) {
    int ret;
    sigset_t curset;

    SwooleG.use_signalfd = 1;

    openswoole_event_init(OSW_EVENTLOOP_WAIT_EXIT);

    sigemptyset(&curset);
    sigprocmask(SIG_BLOCK, NULL, &curset);
    ret = sigismember(&curset, SIGUSR1);
    ASSERT_EQ(ret, 0);

    openswoole_signalfd_init();
    openswoole_signal_set(SIGUSR1, sig_usr1);
    openswoole_signalfd_setup(SwooleTG.reactor);

    sigemptyset(&curset);
    sigprocmask(SIG_BLOCK, NULL, &curset);
    ret = sigismember(&curset, SIGUSR1);
    ASSERT_EQ(ret, 1);

    openswoole_signal_set(SIGUSR1, NULL);
    openswoole_signal_clear();

    sigemptyset(&curset);
    sigprocmask(SIG_BLOCK, NULL, &curset);
    ret = sigismember(&curset, SIGUSR1);
    ASSERT_EQ(ret, 0);

    openswoole_event_wait();
}
#endif
