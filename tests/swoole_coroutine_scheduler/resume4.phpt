--TEST--
swoole_coroutine_scheduler: user yield and resume4
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
Co::yield();
$id = go(function () {
    $id = Co::getCid();
    echo "start coro $id\n";
    Co::yield();
    echo "resume coro $id\n";
});
echo "start to resume $id\n";
Co::resume($id);
echo "main\n";
swoole_event::wait();
?>
--EXPECTF--
Fatal error: Uncaught OpenSwoole\Error: API must be called in the coroutine in %s:%d
Stack trace:
#0 %s(3): OpenSwoole\Coroutine::yield()
#1 {main}
  thrown in %s on line %d
