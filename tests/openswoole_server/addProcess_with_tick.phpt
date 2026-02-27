--TEST--
openswoole_server: addProcess with openswoole_timer_tick fatal error
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$atomic = new OpenSwoole\Atomic;
$pm = new SwooleTest\ProcessManager;

class Process3 extends OpenSwoole\Process
{
    public function __construct()
    {
        parent::__construct([$this, 'run']);
    }

    public function run()
    {
        openswoole_timer_tick(100, function () {
            global $atomic;
            if ($atomic->add() > 5) {
                global $pm;
                $pm->wakeup();
                Co::yield();
                return;
            }
            echo "sleep start then ";
            co::usleep(10000);
            echo "sleep end\n";
            trigger_error('ERROR', E_USER_ERROR);
        });
        OpenSwoole\Event::wait();
    }
}

$pm->parentFunc = function () use ($pm) {
    $pm->kill();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm) {
    $server = new OpenSwoole\Server('127.0.0.1', get_one_free_port(), SWOOLE_PROCESS, SWOOLE_SOCK_UDP);
    $server->set([
        'log_file' => '/dev/null',
//        'worker_num' => 1,
    ]);
    $server->on('packet', function () {
    });
    $server->addProcess(new Process3);
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECTF--
sleep start then sleep end

Fatal error: ERROR in %s/tests/openswoole_server/addProcess_with_tick.php on line %d
%Asleep start then sleep end

Fatal error: ERROR in %s/tests/openswoole_server/addProcess_with_tick.php on line %d
%Asleep start then sleep end

Fatal error: ERROR in %s/tests/openswoole_server/addProcess_with_tick.php on line %d
%Asleep start then sleep end

Fatal error: ERROR in %s/tests/openswoole_server/addProcess_with_tick.php on line %d
%Asleep start then sleep end

Fatal error: ERROR in %s/tests/openswoole_server/addProcess_with_tick.php on line %d
%ADONE
