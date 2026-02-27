--TEST--
openswoole_timer: enable_coroutine setting
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::run(function() {

    OpenSwoole\Timer::after(1, function () {
        $uid = Co::getCid();
        echo "#{$uid}\n";
        OpenSwoole\Timer::after(1, function () {
            $uid = Co::getCid();
            echo "#{$uid}\n";
        });
    });

});

?>
--EXPECT--
#2
#3
