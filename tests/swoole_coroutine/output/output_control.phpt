--TEST--
swoole_coroutine/output: ob_* in coroutine
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
ob_start();
echo 'main';
// #co1
go(function () {
    ob_start();
    echo "foo\n";
    $ob_1 = (ob_get_status(true));
    // yield and it will switch to #co2
    co::usleep(100000);
    // resume to ob_1
    Assert::same($ob_1, (ob_get_status(true)));
    ob_start(); // ob_2
    echo "bar\n";
    Assert::same(ob_get_status()['level'], 1);
    ob_start(); // ob_3
    // yield and it will switch to #co3
    co::usleep(200000);
    // resume to ob_3
    Assert::same(ob_get_status()['level'], 2);
    echo "baz\n";
    Assert::same(ob_get_clean(), "baz\n"); // clean ob_3
    echo ob_get_clean(); // ob_1, ob_2, expect foo\n bar\n;
});

// #co2
go(function () {
    Assert::same(ob_get_status(true), []); //empty
    Assert::assert(!ob_get_contents());
    Co::usleep(1000);
    Assert::assert(!ob_get_clean());
});

// #co3
go(function () {
    co::usleep(200000);
    Assert::same(ob_get_status(true), []); //empty
});
Assert::same(ob_get_clean(), 'main');
OpenSwoole\Event::wait();
?>
--EXPECT--
foo
bar
