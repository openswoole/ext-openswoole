--TEST--
openswoole_http2_server: the SETTINGS frame has the configured http2 settings
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

const NAMES = [1 => 'header_table_size', 3 => 'max_concurrent_streams', 4 => 'initial_window_size', 5 => 'max_frame_size', 6 => 'max_header_list_size'];

foreach ([[
    'http2_header_table_size' => 8192,
    'http2_max_concurrent_streams' => 100,
    'http2_initial_window_size' => 32768,
    'http2_max_frame_size' => 32768,
    'http2_max_header_list_size' => 16384,
], []] as $settings) {
    $pm = new SwooleTest\ProcessManager;
    $pm->parentFunc = function () use ($pm) {
        co::run(function () use ($pm) {
            $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
            Assert::true($cli->connect('127.0.0.1', $pm->getFreePort(), 5));
            $cli->send("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n" . substr(pack('N', 0), 1) . "\x04\x00" . pack('N', 0));
            $buffer = '';
            while (strlen($buffer) < 9 || strlen($buffer) < 9 + unpack('N', "\0" . substr($buffer, 0, 3))[1]) {
                $data = $cli->recv(5);
                if ($data === '' || $data === false) {
                    break;
                }
                $buffer .= $data;
            }
            Assert::same(ord($buffer[3]), 4);
            $length = unpack('N', "\0" . substr($buffer, 0, 3))[1];
            $out = [];
            for ($i = 9; $i < 9 + $length; $i += 6) {
                ['id' => $id, 'value' => $value] = unpack('nid/Nvalue', substr($buffer, $i, 6));
                $out[] = NAMES[$id] . '=' . $value;
            }
            echo implode(', ', $out), "\n";
        });
        $pm->kill();
    };
    $pm->childFunc = function () use ($pm, $settings) {
        $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
        $http->set(['log_file' => '/dev/null', 'open_http2_protocol' => true] + $settings);
        $http->on('workerStart', function () use ($pm) {
            $pm->wakeup();
        });
        $http->on('request', function ($request, $response) {
            $response->end('ok');
        });
        $http->start();
    };
    $pm->childFirst();
    $pm->run();
}
?>
--EXPECT--
header_table_size=8192, max_concurrent_streams=100, initial_window_size=32768, max_frame_size=32768, max_header_list_size=16384
header_table_size=4096, max_concurrent_streams=1280, initial_window_size=65535, max_frame_size=16384
