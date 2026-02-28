--TEST--
openswoole_coroutine: coro with args
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

class TestCo
{
    public function foo(...$args)
    {
        Co::usleep(1000);
        $cid = go(function () {
            co::yield();
        });
        co::resume($cid);
        echo @$this->test;

        foreach ($args as $index => $arg) {
            $recv = new Chan;
            $send = new Chan;
            $data = $args[$index];
            go(function () use ($recv, $data) {
                Co::usleep(1000);
                $recv->push($data); // response
            });
            go(function () use ($send, $data) {
                $data = $send->pop();
                if (Assert::assert($data === $data)) {
                    Co::usleep(1000);
                    $send->push(true); // send ok
                }
            });
            $ret = $send->push($data);
            Assert::assert($ret);
            $response = $recv->pop();
            Assert::same($response, $data);
        }
    }
}

$php_args = [
    'undef',
    null,
    true,
    false,
    1,
    1.1,
    str_repeat('exit', 1),
    array_merge(['exit' => 'ok'], []),
    (object)['exit' => 'ok'],
    STDIN,
    0
];

go([new TestCo, 'foo'], ...$php_args);

?>
--EXPECTF--
