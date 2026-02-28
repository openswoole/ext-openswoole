--TEST--
openswoole_timer: bug Github#2342
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

class workerInfo
{
    public $data;
    public function __construct() {
        $this->data = str_repeat('A', 1024 * 1024 * 1);
    }
}

function worker($timerId, $info)
{
    openswoole_timer_clear($timerId);
}
function manager($timerID)
{
    openswoole_timer_tick( 10, 'worker', new workerInfo());
}
$mem = memory_get_usage();
$timerId = openswoole_timer_tick(50, 'manager');
openswoole_timer_after(500, function()use($timerId){
    openswoole_timer_clear($timerId);
});
openswoole_event::wait();
Assert::assert($mem + 1024 * 1024 * 1 > memory_get_usage());
echo "DONE\n";

?>
--EXPECT--
DONE
