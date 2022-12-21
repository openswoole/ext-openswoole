--TEST--
swoole_coroutine: iterator
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
co::run(function () {
    $i = Co::list();
    // 1
    var_dump($i->current());
    $i->next();
    var_dump($i->current());
    $i->rewind();
    go(function () use ($i) {
        co::usleep(100000);
        // 1
        var_dump($i->current());
        $i->next();
        var_dump($i->current());
        // 3
        $i3 = Co::list();
        var_dump($i3->current());
        $i3->next();
        var_dump($i3->current());
    });
    // 2
    $i2 = Co::list();
    var_dump($i2->current());
    $i2->next();
    var_dump($i2->current());
    $i2->rewind();
});
?>
--EXPECT--
int(1)
NULL
int(2)
int(1)
int(1)
NULL
int(2)
NULL
