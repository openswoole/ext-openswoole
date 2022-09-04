--TEST--
swoole_process: name
--CONFLICTS--
all
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_darwin();
skip_if_in_valgrind();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$name = "SWOOLE_PROCESS_TEST_" . rand(1, 100);

$proc = new \swoole_process(function ($childProc) {
    global $name;
    OpenSwoole\Util::setProcessName($name);
    sleep(PHP_INT_MAX);
});

$pid = $proc->start();
$count = (int)trim(`ps aux|grep $name|grep -v grep|wc -l`);
Assert::same($count, 1);
\swoole_process::kill($pid, SIGKILL);

\swoole_process::wait(true);
echo "SUCCESS";
?>
--EXPECT--
SUCCESS
