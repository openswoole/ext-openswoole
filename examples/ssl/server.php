<?php
$server = new Swoole\Server("0.0.0.0", 9501, SWOOLE_BASE, SWOOLE_SOCK_TCP | SWOOLE_SSL);
// $serv = new Swoole\Server("0.0.0.0", 9501, SWOOLE_PROCESS, SWOOLE_SOCK_TCP | SWOOLE_SSL);
// $port2 = $serv->addlistener('0.0.0.0', 9502, SWOOLE_SOCK_TCP);
// $port2->on('receive', function($serv, $fd, $reactor_id, $data){
//     echo "port2: ".$data."\n";
// });

$server->set(array(
	'worker_num' => 1,
	'ssl_cert_file' => __DIR__.'/ca/server-cert.pem',
	'ssl_key_file' => __DIR__.'/ca/server-key.pem',
    'ssl_verify_peer' => true,
    'ssl_allow_self_signed' => true,
    'ssl_client_cert_file' => __DIR__.'/ca/ca-cert.pem',
    'ssl_verify_depth' => 10,
));

$server->on('connect', function (Swoole\Server $server, $fd, $reactor_id){
	echo "[#".posix_getpid()."]\tClient@[$fd:$reactor_id]: Connect.\n";
    $info = $server->getClientInfo($fd);
    var_dump($info);
});

$server->on('receive', function (Swoole\Server $server, $fd, $reactor_id, $data) {
	echo "[#".posix_getpid()."]\tClient[$fd]: $data\n";
	$server->send($fd, "Swoole: $data\n");
});

$server->on('close', function ($server, $fd, $reactor_id) {
	echo "[#".posix_getpid()."]\tClient@[$fd:$reactor_id]: Close.\n";
});

$server->start();
