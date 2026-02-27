--TEST--
openswoole_coroutine_scheduler/preemptive: child coroutine timer
--SKIPIF--
<?php 
require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

co::set(['enable_preemptive_scheduler' => true]);
go(function (){
    $exit = false;
    while (true){
        $res = OpenSwoole\Coroutine::stats();
        $num = $res['coroutine_num'];
        if ($num < 10){
            go(function () use(&$exit){
                OpenSwoole\Coroutine::sleep(1);
                $exit = true;
            });
        }
        if ($exit) {            
            break;
        }
    }
    echo "coro exit\n";
});
echo "main end\n";
OpenSwoole\Event::wait();
?>
--EXPECTF--
main end
coro exit
