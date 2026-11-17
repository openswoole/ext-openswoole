#pragma once

// Local servers for tests that used outside sites, each runs in its own thread on 127.0.0.1.

#include "test_core.h"
#include "openswoole_socket.h"
#include "openswoole_ssl.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <memory>
#include <string>
#include <thread>

namespace openswoole {
namespace test {

static inline int local_bind(int type, int *port) {
    int fd = ::socket(AF_INET, type, 0);
    if (fd < 0) {
        return -1;
    }
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (::bind(fd, (sockaddr *) &addr, sizeof(addr)) < 0 || (type == SOCK_STREAM && ::listen(fd, 128) < 0)) {
        ::close(fd);
        return -1;
    }
    socklen_t len = sizeof(addr);
    getsockname(fd, (sockaddr *) &addr, &len);
    *port = ntohs(addr.sin_port);
    // lets the loop check for stop() regularly
    timeval tv = {0, 100000};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    return fd;
}

// Answers every request with a small HTML page and closes the connection, over TLS when ssl is true.
class LocalHttpServer {
  public:
    static constexpr const char *BODY = "<html><head><title>OpenSwoole test page</title></head></html>";

    explicit LocalHttpServer(bool ssl = false) : ssl_(ssl) {
        signal(SIGPIPE, SIG_IGN);
        fd_ = local_bind(SOCK_STREAM, &port_);
        EXPECT_GE(fd_, 0);
        thread_ = std::thread([this]() { run(); });
    }

    ~LocalHttpServer() {
        stop_ = true;
        thread_.join();
        ::close(fd_);
    }

    int port() const {
        return port_;
    }

  private:
    bool ssl_;
    int fd_ = -1;
    int port_ = 0;
    std::atomic<bool> stop_{false};
    std::thread thread_;

    void run() {
#ifdef OSW_USE_OPENSSL
        std::unique_ptr<SSLContext> ctx;
        if (ssl_) {
            ctx.reset(new SSLContext());
            ctx->set_cert_file(get_root_path() + "/tests/include/ssl_certs/server.crt");
            ctx->set_key_file(get_root_path() + "/tests/include/ssl_certs/server.key");
            if (!ctx->create()) {
                return;
            }
        }
#endif
        while (!stop_) {
            int conn = ::accept(fd_, nullptr, nullptr);
            if (conn < 0) {
                continue;
            }
            timeval tv = {5, 0};
            setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
            network::Socket *sock = make_socket(conn, OSW_FD_STREAM);
            sock->set_block();
#ifdef OSW_USE_OPENSSL
            if (ssl_ && (sock->ssl_create(ctx.get(), OSW_SSL_SERVER) < 0 || sock->ssl_accept() != OSW_READY)) {
                sock->free();
                continue;
            }
#endif
            serve(sock);
            sock->free();
        }
    }

    ssize_t sock_recv(network::Socket *sock, char *buf, size_t n) {
#ifdef OSW_USE_OPENSSL
        if (ssl_) {
            return sock->ssl_recv(buf, n);
        }
#endif
        return ::recv(sock->fd, buf, n, 0);
    }

    void sock_send(network::Socket *sock, const std::string &data) {
#ifdef OSW_USE_OPENSSL
        if (ssl_) {
            sock->ssl_send(data.c_str(), data.length());
            sock->ssl_shutdown();
            return;
        }
#endif
        ::send(sock->fd, data.c_str(), data.length(), MSG_NOSIGNAL);
    }

    void serve(network::Socket *sock) {
        // read the request head, or until the client stops sending
        std::string request;
        char buf[4096];
        while (request.find("\r\n\r\n") == request.npos) {
            ssize_t n = sock_recv(sock, buf, sizeof(buf));
            if (n <= 0) {
                break;
            }
            request.append(buf, n);
        }
        std::string body(BODY);
        sock_send(sock,
                  "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(body.length()) +
                      "\r\nConnection: close\r\n\r\n" + body);
    }
};

// A DNS server: A queries get 127.0.0.1, AAAA queries ::1, names containing "not-found" NXDOMAIN.
// A silent server never answers. Use it with openswoole_set_dns_server(server.address()).
class LocalDnsServer {
  public:
    explicit LocalDnsServer(bool silent = false) : silent_(silent) {
        fd_ = local_bind(SOCK_DGRAM, &port_);
        EXPECT_GE(fd_, 0);
        thread_ = std::thread([this]() { run(); });
    }

    ~LocalDnsServer() {
        stop_ = true;
        thread_.join();
        ::close(fd_);
    }

    std::string address() const {
        return "127.0.0.1:" + std::to_string(port_);
    }

  private:
    bool silent_;
    int fd_ = -1;
    int port_ = 0;
    std::atomic<bool> stop_{false};
    std::thread thread_;

    void run() {
        unsigned char q[512];
        while (!stop_) {
            sockaddr_in peer = {};
            socklen_t plen = sizeof(peer);
            ssize_t n = ::recvfrom(fd_, q, sizeof(q), 0, (sockaddr *) &peer, &plen);
            if (n < 12 || silent_) {
                continue;
            }
            // question: the name labels, then type and class
            size_t pos = 12;
            std::string name;
            while (pos < (size_t) n && q[pos] != 0) {
                name.append((const char *) &q[pos + 1], q[pos]);
                name.push_back('.');
                pos += q[pos] + 1;
            }
            pos += 1;
            if (pos + 4 > (size_t) n) {
                continue;
            }
            uint16_t qtype = (q[pos] << 8) | q[pos + 1];
            size_t qend = pos + 4;
            bool found = name.find("not-found") == name.npos;

            std::string r((const char *) q, qend);
            r[2] = (char) 0x81;                        // QR, RD
            r[3] = (char) (found ? 0x80 : 0x83);       // RA, NXDOMAIN when not found
            r[6] = 0;
            r[7] = found ? 1 : 0;                      // ANCOUNT
            r[8] = r[9] = r[10] = r[11] = 0;
            if (found) {
                bool v6 = qtype == 28;
                const unsigned char rr[] = {0xc0, 0x0c, 0, (unsigned char) (v6 ? 28 : 1), 0, 1, 0, 0, 0, 60, 0,
                                            (unsigned char) (v6 ? 16 : 4)};
                r.append((const char *) rr, sizeof(rr));
                if (v6) {
                    unsigned char addr[16] = {};
                    addr[15] = 1;
                    r.append((const char *) addr, sizeof(addr));
                } else {
                    const unsigned char addr[4] = {127, 0, 0, 1};
                    r.append((const char *) addr, sizeof(addr));
                }
            }
            ::sendto(fd_, r.data(), r.length(), 0, (sockaddr *) &peer, plen);
        }
    }
};

// Sets the DNS server for the scope and restores the previous one.
class DnsServerScope {
  public:
    explicit DnsServerScope(const std::string &address) : saved_(openswoole_get_dns_server()) {
        openswoole_set_dns_server(address);
    }
    ~DnsServerScope() {
        openswoole_set_dns_server(saved_.first + ":" + std::to_string(saved_.second));
    }

  private:
    std::pair<std::string, int> saved_;
};

// A host name that can not be resolved: a label over 63 bytes can not be sent in a DNS query,
// so the lookup fails without the network.
static inline std::string unresolvable_host() {
    return std::string(64, 'x') + ".not-found.invalid";
}

}  // namespace test
}  // namespace openswoole
