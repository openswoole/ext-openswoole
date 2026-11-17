--TEST--
openswoole_event: add event after server start
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Server;

const FILE = __DIR__.'/add_after_server_start_tmp_result.txt';

// a local HTTP server answers with a redirect status
$server = SwooleTest\LocalServer::start();

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    $pm->kill();
    $str = (string)file_get_contents(FILE);
    Assert::true(strpos($str, 'HTTP/1.1 302 ') > -1 or strpos($str, 'HTTP/1.1 301 ') > -1);
};

$pm->childFunc = function () use ($pm, $server) {
    $serv = new Server('127.0.0.1', $pm->getFreePort());
    $serv->set(array(
        "worker_num" => 1,
        'log_file' => '/dev/null',
    ));
    $serv->on("start", function (Server $serv) use ($pm, $server) {
        $fp = stream_socket_client("tcp://{$server->host}:{$server->httpPort}", $errno, $errstr, 30);
        fwrite($fp, "GET /status/302 HTTP/1.1\r\nHost: {$server->host}\r\n\r\n");

        openswoole_event_add($fp, function ($fp) use ($pm) {
            $resp = fread($fp, 8192);
            openswoole_event_del($fp);
            fclose($fp);
            file_put_contents(FILE, $resp);
            $pm->wakeup();
        });
    });
    $serv->on('receive', function (Server $serv, $fd, $rid, $data) {

    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
unlink(FILE);
?>
--EXPECT--
