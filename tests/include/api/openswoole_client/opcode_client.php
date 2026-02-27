<?php


require_once __DIR__ . "/../../../include/bootstrap.php";



// suicide(5000);

$cli = new \openswoole_client(SWOOLE_SOCK_TCP, true);

/** @noinspection PhpVoidFunctionResultUsedInspection */
assert($cli->set([
    'open_length_check' => 1,
    'package_length_type' => 'N',
    'package_length_offset' => 0,
    'package_body_offset' => 0,
]));

$cli->on("connect", function(openswoole_client $cli) {
    openswoole_timer_clear($cli->timeo_id);
    Assert::true($cli->isConnected());

});

$cli->on("receive", function(openswoole_client $cli, $data){

    $cli->close();
    Assert::false($cli->isConnected());
});

$cli->on("error", function(openswoole_client $cli) {
    openswoole_timer_clear($cli->timeo_id);
    echo "ERROR";
});

$cli->on("close", function(openswoole_client $cli) {
    openswoole_timer_clear($cli->timeo_id);
    echo "CLOSE";
});

$cli->connect(TCP_SERVER_HOST, TCP_SERVER_PORT);

$cli->timeo_id = openswoole_timer_after(1000, function() use($cli) {
    debug_log("connect timeout");
    $cli->close();
    Assert::false($cli->isConnected());
});
