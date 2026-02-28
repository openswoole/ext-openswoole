--TEST--
openswoole_server_port: connections
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Constant;

$pm = new SwooleTest\ProcessManager;
$pm->initFreePorts(2);

$pm->parentFunc = function ($pid) use ($pm) {
    $sch = new \OpenSwoole\Coroutine\Scheduler();
    $sch->parallel(
        3,
        function () use ($pm) {
            $c = new OpenSwoole\Coroutine\Http\Client(TCP_SERVER_HOST, $pm->getFreePort(0));
            $c->upgrade('/');
            $c->recv();
        }
    );
    $sch->parallel(
        2,
        function () use ($pm) {
            \OpenSwoole\Coroutine\System::usleep(1000);
            $c = new OpenSwoole\Coroutine\Http\Client(TCP_SERVER_HOST, $pm->getFreePort(1));
            $c->upgrade('/');
            $c->recv();
        }
    );

    //all
    $sch->add(
        function () use ($pm) {
            \OpenSwoole\Coroutine\System::usleep(1500);
            $c = new OpenSwoole\Coroutine\Http\Client(TCP_SERVER_HOST, $pm->getFreePort(0));
            $c->upgrade('/');
            $c->push('all');
            $frame = $c->recv();
            Assert::assert(is_object($frame));
            $json = json_decode($frame->data);
            Assert::eq($json->count, 8);
            Assert::eq($json->list, range(1, 8));
        }
    );

    //port-0
    $sch->add(
        function () use ($pm) {
            \OpenSwoole\Coroutine\System::usleep(1000);
            $c = new OpenSwoole\Coroutine\Http\Client(TCP_SERVER_HOST, $pm->getFreePort(0));
            $c->upgrade('/');
            $c->push('port-0');
            $frame = $c->recv();
            Assert::assert(is_object($frame));
            $json = json_decode($frame->data);
            Assert::eq($json->count, 5);
            Assert::eq($json->list, [1,2,3,7,8]);
        }
    );

    //port-1
    $sch->add(
        function () use ($pm) {
            \OpenSwoole\Coroutine\System::usleep(1000);
            $c = new OpenSwoole\Coroutine\Http\Client(TCP_SERVER_HOST, $pm->getFreePort(1));
            $c->upgrade('/');
            $c->push('port-1');
            $frame = $c->recv();
            Assert::assert(is_object($frame));
            $json = json_decode($frame->data);
            Assert::eq($json->count, 3);
            Assert::eq($json->list, [4,5,6]);
        }
    );

    $sch->add(
        function () use ($pm) {
            \OpenSwoole\Coroutine\System::usleep(500000);
            $pm->kill();
        }
    );
    $sch->start();
};

$pm->childFunc = function () use ($pm)
{
    $server = new OpenSwoole\WebSocket\Server("0.0.0.0", $pm->getFreePort(0));
    $server->set(
        [
            Constant::OPTION_LOG_FILE => '/dev/null',
            Constant::OPTION_WORKER_NUM => 1,
        ]
    );
    $server->on(
        'open',
        function (OpenSwoole\WebSocket\Server $server, $request) {
        }
    );
    $server->on(
        Constant::EVENT_WORKER_START,
        function () use ($pm) {
            $pm->wakeup();
        }
    );

    $server->listen('127.0.0.1', $pm->getFreePort(1), SWOOLE_SOCK_TCP);

    $server->on(
        'message',
        function (OpenSwoole\WebSocket\Server $server, $frame) {
            if ($frame->data == 'all') {
                $iterator = $server->connections;
            } elseif ($frame->data == 'port-0') {
                $iterator = $server->ports[0]->connections;
            } elseif ($frame->data == 'port-1') {
                $iterator = $server->ports[1]->connections;
            } else {
                return;
            }

            $data['count'] = count($iterator);
            $data['list'] = array_values(iterator_to_array($iterator));
            $server->push($frame->fd, json_encode($data));
        }
    );

    $server->on('close', function ($ser, $fd) {
    });

    $server->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
