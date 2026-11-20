--TEST--
openswoole_client_sync: socks5 handshake with a local proxy
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['log_level' => SWOOLE_LOG_WARNING]);

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    $connect = function (string $host, array $settings = []) use ($pm) {
        $client = new OpenSwoole\Client(SWOOLE_SOCK_TCP, OPENSWOOLE_SOCK_SYNC);
        $client->set($settings + ['socks5_host' => '127.0.0.1', 'socks5_port' => $pm->getFreePort()]);
        $ret = @$client->connect($host, 80, 5);
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

    // username longer than 255 bytes
    [$ret, $client] = $connect('auth.test', ['socks5_username' => str_repeat('u', 300), 'socks5_password' => 'pass']);
    Assert::false($ret);
    Assert::same($client->errCode, OpenSwoole\Constant::ERROR_SOCKS5_HANDSHAKE_FAILED);

    // proxy rejects the connect request
    [$ret, $client] = $connect('refused.test');
    Assert::false($ret);
    Assert::same($client->errCode, OpenSwoole\Constant::ERROR_SOCKS5_SERVER_ERROR);

    echo "DONE\n";
    $pm->kill();
};

// minimal SOCKS5 server: rejects "refused.test" and keeps the connection open, echoes "<host>:<data>" otherwise
$pm->childFunc = function () use ($pm) {
    Co::run(function () use ($pm) {
        $server = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, 0);
        Assert::true($server->bind('127.0.0.1', $pm->getFreePort()));
        Assert::true($server->listen());
        $pm->wakeup();
        while (($conn = $server->accept()) !== false) {
            go(function () use ($conn) {
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
                while (($data = $conn->recv()) !== '' && $data !== false) {
                    $conn->send("{$host}:{$data}");
                }
                $conn->close();
            });
        }
    });
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
