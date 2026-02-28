--TEST--
openswoole_timer: #2697
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$server = new OpenSwoole\Server('127.0.0.1', get_one_free_port());
$server->set(['log_file' => '/dev/null']);
$server->on('workerStart', function (OpenSwoole\Server $server, int $worker_id) {
    OpenSwoole\Timer::after(1000, function () {
        var_dump('never here');
    });
    OpenSwoole\Timer::tick(1000, function () {
        var_dump('never here');
    });
    OpenSwoole\Timer::clearAll();
    if ($worker_id === 0) {
        OpenSwoole\Timer::after(10, function () use ($server) {
            $server->shutdown();
        });
    }
});
$server->on('receive', function () { });
$server->start();
echo "DONE\n";
?>
--EXPECT--
DONE
