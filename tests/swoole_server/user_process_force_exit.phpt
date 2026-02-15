--TEST--
swoole_server: process force exit
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
<?php skip_if_function_not_exist('pcntl_signal'); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Constant;

$atomic = new OpenSwoole\Atomic;
$pm = new SwooleTest\ProcessManager;

$pm->parentFunc = function () use ($pm) {
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {

    $server = new OpenSwoole\Server('127.0.0.1', get_one_free_port(), SWOOLE_PROCESS, SWOOLE_SOCK_UDP);

    $server->set([
        Constant::OPTION_LOG_FILE => '/dev/null',
        Constant::OPTION_MAX_WAIT_TIME => 1,
    ]);

    $server->on('packet', function () {
    });
    $server->addProcess(new OpenSwoole\Process(function () {
        pcntl_signal(SIGTERM, function () {
        });
        swoole_timer_tick(1000, function () {
        });
    }));
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECTF--
DONE
