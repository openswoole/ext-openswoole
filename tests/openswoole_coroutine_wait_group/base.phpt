--TEST--
openswoole_coroutine_wait_group: base
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; skip('TODOv22'); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$wg = new OpenSwoole\Coroutine\WaitGroup;
go(function () use ($wg) {
    go(function () use ($wg) {
        $wg->add();
        Assert::same(
            file_get_contents(__FILE__),
            Co::readFile(__FILE__)
        );
        echo "TASK[1] DONE\n";
        $wg->done();
    });
    $cid = go(function () use ($wg) {
        $wg->add();
        Assert::true(Co::yield());
        echo "TASK[2] DONE\n";
        $wg->done();
    });
    go(function () use ($wg, $cid) {
        $wg->add();
        Assert::notSame(Co::usleep(1000), false);
        Co::resume($cid);
        echo "TASK[3] DONE\n";
        $wg->done();
    });
    $wg->wait();
});
?>
--EXPECT--
TASK[1] DONE
TASK[2] DONE
TASK[3] DONE
