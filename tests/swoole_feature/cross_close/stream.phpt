--TEST--
swoole_feature/cross_close: stream
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID < 80000) die("Skipped: php version < 8."); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
Swoole\Runtime::enableCoroutine();
go(function () {
    $fp = stream_socket_client('tcp://' . REDIS_SERVER_HOST . ':' . REDIS_SERVER_PORT, $errno, $errstr, 1);
    if (!$fp) {
        exit("$errstr ($errno)\n");
    } else {
        go(function () use ($fp) {
            Co::usleep(1000);
            echo "CLOSE\n";
            fclose($fp);
            echo "DONE\n";
        });
        echo "READ\n";
        Assert::assert(!fread($fp, 1024));
        echo "CLOSED\n";
        fclose($fp);
    }
});
?>
--EXPECTF--
READ
CLOSE
CLOSED

Fatal error: Uncaught TypeError: fclose(): supplied resource is not a valid stream resource in %s/tests/swoole_feature/cross_close/stream.php:%d
Stack trace:
#0 %s/tests/swoole_feature/cross_close/stream.php(%d): fclose(Resource id #%d)
%A
  thrown in %s/tests/swoole_feature/cross_close/stream.php on line %d
