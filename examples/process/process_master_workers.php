<?php

// Example of Master-Workers Process IPC

$workers = [];
$worker_num = 10;

//Swoole\Process::daemon(0, 1);

function onReceive($pipe) {
    global $workers;
    $worker = $workers[$pipe];
    $data = $worker->read();
    echo "RECV: " . $data;
}

for($i = 0; $i < $worker_num; $i++)
{
    $process = new Swoole\Process(function(Swoole\Process $process) {
        $i = 1;
        while($i++)
        {
            $process->write("Worker#{$process->id}: hello master\n");
            if ($i > 5 and $process->id == 1) $process->exit();
            sleep(1);
        }
    });
    $process->id = $i;
    $pid = $process->start();
    $workers[$process->pipe] = $process;
}

Swoole\Process::signal(SIGCHLD, function(){
    $status = Swoole\Process::wait();
    echo "Worker#{$status['pid']} exit\n";
});

foreach($workers as $process)
{
    Swoole\Event::add($process->pipe, 'onReceive');
}
