--TEST--
openswoole_coroutine_wait_group: logic
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; skip('TODOv22'); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
go(function () {
    $wg = new OpenSwoole\Coroutine\WaitGroup;
    Assert::throws(function () use ($wg) {
        $wg->add(-1);
    }, LogicException::class);
    $wg->add(1);
    go(function () use ($wg) {
        Co::usleep(1000);
        Assert::throws(function () use ($wg) {
            $wg->add(1);
        }, LogicException::class);
        $wg->done();
    });
    $wg->wait();
    Assert::throws(function () use ($wg) {
        $wg->done();
    }, LogicException::class);
    echo "DONE\n";
});
?>
--EXPECT--
DONE
