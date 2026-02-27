--TEST--
openswoole_runtime/file_hook: flock
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
\OpenSwoole\Runtime::enableCoroutine();

function test_flock()
{
    $fp = fopen('/tmp/flock.log', 'w+');
    Assert::assert(flock($fp, LOCK_EX));
    Co::usleep(1000);
    $ret = fwrite($fp, ($date = date('Y-m-d H:i:s')));
    Assert::same($ret, strlen($date));
    flock($fp, LOCK_UN);
    fclose($fp);
}

$num = mt_rand(2, 16);
OpenSwoole\Util::setAio(['thread_num' => $num]);
go(function () use ($num) {
    for ($i = $num; $i--;) {
        go('test_flock');
    }
});
openswoole_event_wait();
echo "DONE\n";

?>
--EXPECT--
DONE
