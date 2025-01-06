--TEST--
swoole_coroutine: exit
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID >= 80400) die("Skipped"); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
ini_set('openswoole.display_errors', 'off');

$exit_status_list = [
    'undef',
    null,
    true,
    false,
    1,
    1.1,
    'exit',
    ['exit' => 'ok'],
    (object)['exit' => 'ok'],
    STDIN,
    0
];

function route()
{
    controller();
}

function controller()
{
    your_code();
}

function your_code()
{
    global $exit_status_list;
    co::usleep(1000);
    $exit_status = array_shift($exit_status_list);
    if ($exit_status === 'undef') {
        exit;
    } else {
        exit($exit_status);
    }
}

co::run(function() use ($exit_status_list) {

$chan = new OpenSwoole\Coroutine\Channel;

go(function () use ($chan, $exit_status_list) {
    foreach ($exit_status_list as $val) {
        $chan->push($val);
    }
});

for ($i = 0; $i < count($exit_status_list); $i++) {
    go(function () use ($exit_status_list, $chan) {
        try {
            // in coroutine
            route();
        } catch (\OpenSwoole\ExitException $e) {
            Assert::assert($e->getFlags() & OpenSwoole\Coroutine::EXIT_IN_COROUTINE);
            $exit_status = $chan->pop();
            $exit_status = $exit_status === 'undef' ? null : $exit_status;
            Assert::same($e->getStatus(), $exit_status);
            var_dump($e->getStatus());
            // exit coroutine
            return;
        }
        echo "never here\n";
    });
}

});

?>
--EXPECTF--
NULL
NULL
bool(true)
bool(false)
int(1)
float(1.1)
string(4) "exit"
array(1) {
  ["exit"]=>
  string(2) "ok"
}
object(stdClass)#%d (%d) {
  ["exit"]=>
  string(2) "ok"
}
resource(%d) of type (stream)
int(0)
