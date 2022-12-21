--TEST--
swoole_coroutine: coro not inline function
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Coroutine as co;

function test()
    {
        echo "start func\n";
        Co::usleep(1000);
        echo "end func\n";
        return "return func params\n";
    }

co::run(function() {
    echo "start\n";
    co::create(function () {
        $ret = test();
        echo $ret;
    });
    echo "end\n";

});

?>
--EXPECT--
start
start func
end
end func
return func params
