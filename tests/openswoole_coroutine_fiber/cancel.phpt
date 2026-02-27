--TEST--
openswoole_coroutine_fiber: cancel coroutine with fiber context
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['use_fiber_context' => true]);
co::run(function () {
    $cid = go(function () {
        $ret = Co::yield();
        Assert::false($ret);
        Assert::true(Co::isCanceled());
        echo "canceled\n";
    });

    go(function () use ($cid) {
        Assert::true(Co::cancel($cid));
        echo "cancel sent\n";
    });
});
?>
--EXPECT--
canceled
cancel sent
