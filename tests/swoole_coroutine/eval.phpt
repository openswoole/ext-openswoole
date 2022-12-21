--TEST--
swoole_coroutine: eval
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Assert::same(Co::stats()['coroutine_num'], 0);

co::run(function() {
    go(function () {
        echo "start 1\n";
        eval('co::usleep(500000);');
        echo "end 1\n";
    });
    go(function () {
        eval(' echo "start 2\n" ;');
        co::usleep(500000);
        echo "end 2\n";
    });
});
echo "main end\n";
?>
--EXPECT--
start 1
start 2
end 1
end 2
main end
