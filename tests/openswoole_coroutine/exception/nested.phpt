--TEST--
openswoole_coroutine/exception: exception before yield
--SKIPIF--
<?php require  __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Coroutine as co;
go(function () {
    try {
        echo "start\n";
        go(function(){
            try {
                echo "sub start\n";
                throw new Exception('sub coro Exception');
                co::usleep(1000);
                echo "after go2 sleep\n";
            } catch (Exception $e) {
                echo 'Caught exception: ',  $e->getMessage(), "\n";
            } finally {
                echo "finally 2\n";
            }
        });
        echo "after go1 sleep\n";
    } catch (Exception $e) {
        echo 'Caught exception: ',  $e->getMessage(), "\n";
    } finally {
        echo "finally 1\n";
    }
});
    echo "end\n";

?>
--EXPECT--
start
sub start
Caught exception: sub coro Exception
finally 2
after go1 sleep
finally 1
end
