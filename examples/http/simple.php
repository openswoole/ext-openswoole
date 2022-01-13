<?php

use Swoole\Http\Server;
use Swoole\Http\Request;
use Swoole\Http\Response;

$table = new Swoole\Table(1024);
$table->column('name', Swoole\Table::TYPE_STRING, 64);
$table->column('id', Swoole\Table::TYPE_INT, 4);       //1,2,4,8
$table->column('num', Swoole\Table::TYPE_FLOAT);
$table->create();

$table1 = new Swoole\Table(1024);
$table1->column('name', Swoole\Table::TYPE_STRING, 64);
$table1->column('id', Swoole\Table::TYPE_INT, 4);       //1,2,4,8
$table1->column('num', Swoole\Table::TYPE_FLOAT);
$table1->create();

$server = new Swoole\HTTP\Server("127.0.0.1", 9501);
$server->set([
    'worker_num' => 4,
    'task_worker_num' => 4,
    //'max_request' => 10000,
    //'max_request_grace' => 0,
]);

$process = new Swoole\Process(function($process) use ($server) 
{
    while(true) 
    {
        $msg = $process->read();

        foreach($server->connections as $conn) 
        {
            $server->send($conn, $msg);
        }
    }
});

$server->addProcess($process);

$server->on("Start", function(Server $server)
{
    echo "Swoole http server is started at http://127.0.0.1:9501\n";
});

class A {
    public string $a = '';

    function __construct() {
        $this->a = str_repeat('abcd', 1000);
    }
};

$server->on("Request", function(Request $request, Response $response) use ($server)
{
    // var_dump($request);
    // // memory leak example
    // global $c;
    // $c[] = new A();
    // global $d;
    // $d[] = 'a';
    $response->header("Content-Type", "text/plain");
    //return $response->end($server->stats(\SWOOLE_STATS_MODE_OPENMETRICS));
    print_r($server->stats());
    return $response->end();

});



$server->on('Task', function (Swoole\Server $server, $task_id, $reactorId, $data)
{
    echo "Task Worker Process received data";

    echo "#{$server->worker_id}\tonTask: [PID={$server->worker_pid}]: task_id=$task_id, data_len=" . strlen($data) . "." . PHP_EOL;

    $server->finish($data);
});

$server->start();