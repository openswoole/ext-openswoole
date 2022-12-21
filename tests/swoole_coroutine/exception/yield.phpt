--TEST--
swoole_coroutine/exception: exception after yield
--SKIPIF--
<?php require  __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

co::run(function () {
    try {
        echo "start\n";
        co::usleep(1000);
        echo "after sleep\n";
        throw new Exception('coro Exception');
    } catch (Exception $e) {
        echo 'Caught exception: ',  $e->getMessage(), "\n";
    } finally {
        echo "finally.\n";
    }
});
    echo "end\n";

?>
--EXPECT--
start
after sleep
Caught exception: coro Exception
finally.
end
