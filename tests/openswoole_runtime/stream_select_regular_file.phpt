--TEST--
openswoole_runtime: stream_select() with a regular file does not close it
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

OpenSwoole\Runtime::enableCoroutine(OpenSwoole\Runtime::HOOK_ALL);
// epoll refuses regular files, that warning is expected
OpenSwoole\Coroutine::set(['log_level' => SWOOLE_LOG_ERROR]);
co::run(function () {
    $file = fopen(__FILE__, 'r');
    [$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
    go(function () use ($b) {
        co::usleep(100000);
        fwrite($b, 'x');
    });
    for ($i = 0; $i < 2; $i++) {
        $read = [$file, $a];
        $write = $except = null;
        var_dump(stream_select($read, $write, $except, 1) > 0);
    }
    fseek($file, 0);
    var_dump(fread($file, 5) === substr(file_get_contents(__FILE__), 0, 5));
});
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
