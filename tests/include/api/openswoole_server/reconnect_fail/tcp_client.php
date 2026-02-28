<?php
ini_set("memory_limit", "1024m");

function reconn() {
    echo "Reconnect\n";
    $cli = new openswoole_client(SWOOLE_SOCK_TCP, true);
    $cli->on("connect", function(openswoole_client $cli) {
        // client 发送 大包数据
        $cli->send(str_repeat("\0", 1024 * 1024 * 1.9));
    });
    $cli->on("receive", function(openswoole_client $cli, $data) {
        $cli->send($data);
    });
    $cli->on("error", function(openswoole_client $cli) { echo "error\n"; });
    $cli->on("close", function(openswoole_client $cli) { echo "close\n"; reconn(); });
    $cli->connect('127.0.0.1', 9001);
}

reconn();