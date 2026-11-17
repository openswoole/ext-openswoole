--TEST--
openswoole_client_coro: eof with smtp qq
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
co::run(function () {
    // a local peer that answers like the smtp.qq.com server did
    $server = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, 0);
    Assert::true($server->bind('127.0.0.1', 0));
    Assert::true($server->listen());
    $port = $server->getsockname()['port'];
    go(function () use ($server) {
        $conn = $server->accept(5);
        Assert::isInstanceOf($conn, OpenSwoole\Coroutine\Socket::class);
        $conn->send("220 smtp.qq.com Esmtp QQ Mail Server\r\n");
        Assert::same($conn->recv(8192, 5), "ehlo smtp.qq.com\r\n");
        // several lines in one packet, split by the client's eof check
        $conn->send("250-smtp.qq.com\r\n250-PIPELINING\r\n250-SIZE 73400320\r\n250-STARTTLS\r\n" .
            "250-AUTH LOGIN PLAIN\r\n250-AUTH=LOGIN\r\n250-MAILCOMPRESS\r\n250 8BITMIME\r\n");
        $conn->recv(8192, 5);
        $conn->close();
        $server->close();
    });

    $client = new OpenSwoole\Coroutine\Client(SWOOLE_TCP);
    $client->set([
        'open_eof_check' => true,
        'package_eof' => "\r\n",
    ]);
    $client->connect('127.0.0.1', $port, 5);
    go(function () use ($client) {
        $n = 9;
        while ($n--) {
            $data = $client->recv();
            echo $data;
            if (empty($data)) {
                break;
            }
        }
    });
    $client->send("ehlo smtp.qq.com\r\n");
});
echo "DONE\n";
?>
--EXPECT--
220 smtp.qq.com Esmtp QQ Mail Server
250-smtp.qq.com
250-PIPELINING
250-SIZE 73400320
250-STARTTLS
250-AUTH LOGIN PLAIN
250-AUTH=LOGIN
250-MAILCOMPRESS
250 8BITMIME
DONE
