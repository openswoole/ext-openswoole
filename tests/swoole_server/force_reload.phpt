--TEST--
swoole_server: force reload in process mode
--SKIPIF--
<?php
 require __DIR__ . '/../include/skipif.inc';
if (swoole_cpu_num() === 1) {
	skip('not support on machine with single cpu');
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

const WORKER_NUM = 4;

$pm = new ProcessManager;
$atomic = new Swoole\Atomic;

$pm->parentFunc = function ($pid) use ($pm) {
	$n = WORKER_NUM;
	$clients = [];
	while ($n--) {
		$client = new Swoole\Client(SWOOLE_SOCK_TCP);
		if (!$client->connect('127.0.0.1', $pm->getFreePort())) {
			exit("connect failed\n");
		}
		$client->send('hello world');
		$clients[] = $client;
	}
	sleep(1);
	switch_process();
	//reload
	echo "[-1] start to reload\n";
	Swoole\Process::kill($pid, SIGUSR1);
	sleep(3);
	$pm->kill();
};

$pm->childFunc = function () use ($pm, $atomic) {
	$server = new Swoole\Server('127.0.0.1', $pm->getFreePort());
	$server->set([
		'worker_num' => WORKER_NUM,
		'max_wait_time' => 1,
		'enable_coroutine' => false,
	]);
	$server->on('workerStart', function (Swoole\Server $server, $worker_id) use ($pm, $atomic) {
		$worker_id++;
		sleep(1);
		echo "$worker_id [" . $server->worker_pid . "] start\n";
		$atomic->add(1);
		if ($atomic->get() === WORKER_NUM) {
			$pm->wakeup();
		}
	});
	$server->on('receive', function ($serv, $fd, $tid, $data) {
		sleep(100);
	});
	$server->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECTF--
%d [%d] start
%d [%d] start
%d [%d] start
%d [%d] start
%s start to reload
[%s]	INFO	Server is reloading all workers
[%s]	INFO	Server has done all workers reloading
[%s]	WARNING	Manager::kill_timeout_process() (ERRNO 9012): worker(pid=%d, id=%d) exit timeout, force kill the process
[%s]	WARNING	Manager::kill_timeout_process() (ERRNO 9012): worker(pid=%d, id=%d) exit timeout, force kill the process
[%s]	WARNING	Manager::kill_timeout_process() (ERRNO 9012): worker(pid=%d, id=%d) exit timeout, force kill the process
[%s]	WARNING	Manager::kill_timeout_process() (ERRNO 9012): worker(pid=%d, id=%d) exit timeout, force kill the process
[%s]	WARNING	Server::check_worker_exit_status(): worker(pid=%d, id=%d) abnormal exit, status=0, signal=9
[%s]	WARNING	Server::check_worker_exit_status(): worker(pid=%d, id=%d) abnormal exit, status=0, signal=9
[%s]	WARNING	Server::check_worker_exit_status(): worker(pid=%d, id=%d) abnormal exit, status=0, signal=9
[%s]	WARNING	Server::check_worker_exit_status(): worker(pid=%d, id=%d) abnormal exit, status=0, signal=9
[%s]	INFO	Server is shutdown now
%d [%d] start
%d [%d] start
%d [%d] start
%d [%d] start
