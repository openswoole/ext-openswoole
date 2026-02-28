--TEST--
openswoole_timer: Timer callback
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

class TestCo
{
    public function foo()
    {
        Co::usleep(1000);
        $cid = go(function () {
            co::yield();
        });
        co::resume($cid);
        echo @$this->test;
    }
}

for ($c = MAX_CONCURRENCY; $c--;) {
    OpenSwoole\Timer::after($c + 1, [new TestCo, 'foo']);
}

?>
--EXPECTF--
