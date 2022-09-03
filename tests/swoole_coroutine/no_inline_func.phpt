--TEST--
swoole_coroutine: coro not inline function
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Coroutine as co;

co::run(function() {

    co::create(function () {
        $ret = test();
        echo $ret;
    });
    function test()
    {
        echo "start func\n";
        Co::usleep(1000);
        echo "end func\n";
        return "return func params\n";
    }

});
echo "start\n";

echo "end\n";
?>
--EXPECT--
start
start func
end
end func
return func params
