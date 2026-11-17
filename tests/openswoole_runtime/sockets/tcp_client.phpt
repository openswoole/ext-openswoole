--TEST--
openswoole_runtime/sockets: tcp client
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
skip('TODOv22');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Runtime;

const N = 8;

Runtime::enableCoroutine(SWOOLE_HOOK_ALL);

$GLOBALS['time'] = [];
$s = microtime(true);
co::run(function () {
    // a local HTTP peer that answers each connection after 200ms, concurrently
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
    Assert::assert($server);
    $port = (int) explode(':', stream_socket_get_name($server, false))[1];
    go(function () use ($server) {
        for ($i = 0; $i < N; $i++) {
            $conn = stream_socket_accept($server, 5);
            Assert::assert($conn);
            go(function () use ($conn) {
                fread($conn, 8192);
                Co::sleep(0.2);
                $body = str_repeat('<p>OpenSwoole test page</p>', 500);
                fwrite($conn, "HTTP/1.0 200 OK\r\nContent-Length: " . strlen($body) . "\r\nConnection: close\r\n\r\n" . $body);
                fclose($conn);
            });
        }
        fclose($server);
    });

    $n = N;
    while($n--) {
        go(function() use ($port) {
            $s = microtime(true);
            $domain = '127.0.0.1';
            $sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
            Assert::true(socket_connect($sock, $domain, $port));
            socket_write($sock, "GET / HTTP/1.0\r\nHost: $domain\r\nConnection: close\r\nKeep-Alive: off\r\n\r\n");

            $html = '';
            while(true) {
                $data = socket_read($sock, 8192);
                if ($data == '') {
                    break;
                }
                $html .= $data;
            }

            Assert::greaterThanEq(strlen($html), 10000);
            Assert::contains($html, 'OpenSwoole test page');
            socket_close($sock);

            $GLOBALS['time'][] = microtime(true) - $s;
        });
    }
});
echo "Done\n";
Assert::lessThanEq(microtime(true) - $s, array_sum($GLOBALS['time']) / 3);
?>
--EXPECT--
Done
