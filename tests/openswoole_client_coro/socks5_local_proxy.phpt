--TEST--
openswoole_client_coro: socks5 handshake with a local proxy
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['log_level' => SWOOLE_LOG_WARNING]);

// minimal SOCKS5 server: rejects "refused.test", echoes "<host>:<data>" otherwise
function socks5_serve(OpenSwoole\Coroutine\Socket $conn): void
{
    $greet = $conn->recv();
    if ($greet === '' || $greet === false) {
        $conn->close();
        return;
    }
    $method = ord($greet[2]);
    $conn->send("\x05" . chr($method));
    if ($method === 0x02) {
        $auth = $conn->recv();
        if ($auth === '' || $auth === false) {
            $conn->close();
            return;
        }
        $conn->send("\x01\x00");
    }
    $req = $conn->recv();
    if ($req === '' || $req === false) {
        $conn->close();
        return;
    }
    $host = substr($req, 5, ord($req[4]));
    $status = $host === 'refused.test' ? 0x05 : 0x00;
    $conn->send("\x05" . chr($status) . "\x00\x01\x00\x00\x00\x00\x00\x00");
    if ($status === 0x00) {
        while (($data = $conn->recv()) !== '' && $data !== false) {
            $conn->send("{$host}:{$data}");
        }
    }
    $conn->close();
}

Co::run(function () {
    $port = get_one_free_port();
    $server = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, 0);
    Assert::true($server->bind('127.0.0.1', $port));
    Assert::true($server->listen());
    go(function () use ($server) {
        while (($conn = $server->accept()) !== false) {
            go('socks5_serve', $conn);
        }
    });

    $connect = function (string $host, array $settings = []) use ($port) {
        $client = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        $client->set($settings + ['timeout' => 5, 'socks5_host' => '127.0.0.1', 'socks5_port' => $port]);
        $ret = $client->connect($host, 80);
        return [$ret, $client];
    };

    // valid host
    [$ret, $client] = $connect('example.test');
    Assert::true($ret);
    $client->send('hello');
    Assert::same($client->recv(), 'example.test:hello');
    $client->close();

    // valid host with credentials
    [$ret, $client] = $connect('auth.test', ['socks5_username' => 'user', 'socks5_password' => 'pass']);
    Assert::true($ret);
    $client->send('hello');
    Assert::same($client->recv(), 'auth.test:hello');
    $client->close();

    // host longer than 255 bytes
    [$ret, $client] = $connect(str_repeat('a', 300) . '.test');
    Assert::false($ret);
    Assert::same($client->errCode, OpenSwoole\Constant::ERROR_SOCKS5_HANDSHAKE_FAILED);

    // password longer than 255 bytes
    [$ret, $client] = $connect('auth.test', ['socks5_username' => 'user', 'socks5_password' => str_repeat('p', 300)]);
    Assert::false($ret);
    Assert::same($client->errCode, OpenSwoole\Constant::ERROR_SOCKS5_HANDSHAKE_FAILED);

    // proxy rejects the connect request
    [$ret, $client] = $connect('refused.test');
    Assert::false($ret);

    $server->close();
    echo "DONE\n";
});
?>
--EXPECT--
DONE
