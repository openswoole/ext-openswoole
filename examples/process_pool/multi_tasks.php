<?php
declare(strict_types=1);

const N = 5;
$pool = new Swoole\Process\Pool(N);

$parallel_tasks = [];

for ($i=0; $i < N; $i++) { 
	$parallel_tasks[] = function($taskId) {
		while(1) {
			echo "Task#{$taskId} is executed, pid=". posix_getpid(). "\n";
			sleep(1);
		}
	};
}

$pool->on("WorkerStart", function ($pool, $workerId) use ($parallel_tasks) {
    echo "Worker#{$workerId} is started, pid=". posix_getpid(). "\n";
    $parallel_tasks[$workerId]($workerId);
});

$pool->start();