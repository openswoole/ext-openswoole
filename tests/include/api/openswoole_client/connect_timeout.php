<?php
$cli = new openswoole_client(SWOOLE_SOCK_TCP, true);
$cli->on("connect", function(openswoole_client $cli) {
    Assert::true(false, 'never here');
});
$cli->on("receive", function(openswoole_client $cli, $data) {
    Assert::true(false, 'never here');
});
$cli->on("error", function(openswoole_client $cli) { echo "connect timeout\n"; });
$cli->on("close", function(openswoole_client $cli) { echo "close\n"; });
$cli->connect("11.11.11.11", 9000, 0.5);