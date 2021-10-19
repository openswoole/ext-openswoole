--TEST--
swoole_feature/cross_close: stream
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID >= 80000) die("Skipped: php version >= 8."); ?>
--FILE--
<?php
require __DIR__ . '/../../include/bootstrap.php';
Swoole\Runtime::enableCoroutine();
go(function () {
    $fp = stream_socket_client('tcp://' . REDIS_SERVER_HOST . ':' . REDIS_SERVER_PORT, $errno, $errstr, 1);
    if (!$fp) {
        exit("$errstr ($errno)\n");
    } else {
        go(function () use ($fp) {
            co::sleep(0.001);
            echo "CLOSE\n";
            var_dump($fp);
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

Warning: fclose(): supplied resource is not a valid stream resource in %s/tests/swoole_feature/cross_close/stream_php7.php on line 18
DONE
