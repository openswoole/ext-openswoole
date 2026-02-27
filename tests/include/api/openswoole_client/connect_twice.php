<?php

$start = microtime(true);

$cli = new \openswoole_client(SWOOLE_SOCK_TCP, true);
$cli->on("connect", function(openswoole_client $cli) {
    Assert::true(false, 'never here');
});
$cli->on("receive", function(openswoole_client $cli, $data) {
    Assert::true(false, 'never here');
});
$cli->on("error", function(openswoole_client $cli) {
    echo "error\n";
});
$cli->on("close", function(openswoole_client $cli) {
    echo "close\n";
});

function refcount($var)
{
    ob_start();
    debug_zval_dump($var);
    preg_match('/refcount\((?<refcount>\d)\)/', ob_get_clean(), $matches);
    return intval($matches["refcount"]) - 3;
}

@$cli->connect("11.11.11.11", 9000, 0.1);
@$cli->connect("11.11.11.11", 9000, 0.1);
@$cli->connect("11.11.11.11", 9000, 0.1);
@$cli->connect("11.11.11.11", 9000, 0.1);
@$cli->connect("11.11.11.11", 9000, 0.1);
OpenSwoole\Event::wait();
// xdebug_debug_zval("cli");
// echo refcount($cli); // php7无效