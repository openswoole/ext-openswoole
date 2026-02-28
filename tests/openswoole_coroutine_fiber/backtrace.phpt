--TEST--
openswoole_coroutine_fiber: backtrace across coroutines
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['use_fiber_context' => true]);
co::run(function () {
    $main_cid = Co::getCid();

    go(function () use ($main_cid) {
        $cid = Co::getCid();
        Assert::assert($cid > 0);

        // verify own backtrace
        $bt = Co::getBackTrace($cid);
        Assert::isArray($bt);

        // verify parent coroutine backtrace
        $parent_bt = Co::getBackTrace($main_cid);
        Assert::isArray($parent_bt);
        Assert::assert(count($parent_bt) > 0);

        // verify pcid
        $pcid = Co::getPcid();
        Assert::same($pcid, $main_cid);

        echo "backtrace OK\n";
    });

    // verify coroutine list
    go(function () {
        $list = Co::list();
        $count = 0;
        foreach ($list as $cid) {
            $count++;
            Assert::assert($cid > 0);
            $bt = Co::getBackTrace($cid);
            Assert::isArray($bt);
        }
        Assert::assert($count >= 2);
        echo "list OK\n";
    });
});
?>
--EXPECT--
backtrace OK
list OK
