<?php

// swoole socket 复用BUG

function onClose(openswoole_client $cli) {
    $fd = \EventUtil::getSocketFd($cli->getSocket());
    echo "close fd <$fd>\n";
}

function onError(openswoole_client $cli) {
    $fd = \EventUtil::getSocketFd($cli->getSocket());
    echo "error fd <$fd>\n";
}

$host = "127.0.0.1";
$port = 8050;

$cli = new \openswoole_client(SWOOLE_SOCK_TCP, true);
$cli->on("receive", function(openswoole_client $cli, $data){ });
$cli->on("error", "onError");
$cli->on("close", "onClose");

$cli->on("connect", function(openswoole_client $cli) use($host, $port) {
    $fd = \EventUtil::getSocketFd($cli->getSocket());
    echo "connected fd <$fd>\n";
    $cli->close(); // close(fd)


    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    $newCli = new \openswoole_client(SWOOLE_SOCK_TCP, true);
    $newCli->on("receive", function(openswoole_client $cli, $data){ });
    $newCli->on("error", "onError");
    $newCli->on("close", "onClose");
    $newCli->on("connect", function(openswoole_client $newCli) use($cli)  {
        $fd = \EventUtil::getSocketFd($cli->getSocket());
        echo "connected fd <$fd>, reuse!!!\n";

        echo "free socket\n";
        $cli->__destruct();
        echo "send\n";
        $r = $newCli->send("HELLO");
    });
    $newCli->connect($host, $port);
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

});

$cli->connect($host, $port);
