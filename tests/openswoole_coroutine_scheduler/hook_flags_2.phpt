--TEST--
openswoole_coroutine_scheduler: hook_flags
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

const URL = "http://www.gov.cn/";
const KEYWORDS = '中国政府网';
// 1
$sch = new OpenSwoole\Coroutine\Scheduler();
$sch->set(['hook_flags' => SWOOLE_HOOK_ALL,]);
$sch->add(function () {
    Assert::contains(file_get_contents(URL), KEYWORDS);
});
$sch->start();

Assert::contains(file_get_contents(URL), KEYWORDS);

// 2
$sch = new OpenSwoole\Coroutine\Scheduler();
$sch->set(['hook_flags' => SWOOLE_HOOK_ALL,]);
$sch->add(function ()  {
    Assert::contains(file_get_contents(URL), KEYWORDS);
});
$sch->start();

?>
--EXPECT--
