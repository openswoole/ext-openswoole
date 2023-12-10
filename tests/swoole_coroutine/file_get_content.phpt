--TEST--
swoole_coroutine: file_get_content
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new SwooleTest\ProcessManager;
$pm->useConstantPorts = true;
$pm->initFreePorts();

use OpenSwoole\Constant;
use OpenSwoole\Http\Request;
use OpenSwoole\Http\Response;
use OpenSwoole\Http\Server;
use OpenSwoole\Runtime;
use OpenSwoole\Util;

$pm->parentFunc = function (int $pid) use ($pm) {
    co::run(function () use ($pm) {
    	$data = httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
    	echo $data;
    });
    echo "DONE\n";
    unlink('FILENAME');
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
	$Server = new Server('127.0.0.1', $pm->getFreePort());
	$Server->set([
	  'reactor_num' => Util::getCPUNum() * 1,
	  'hook_flags' => Runtime::HOOK_ALL,
	  'log_level' => Constant::LOG_DEBUG,
	  'log_file' => '/proc/1/fd/1',
	  'tcp_fastopen' => true,
	  'open_tcp_keepalive' => true,
	  'http_compression' => true,
	  'http_compression_level' => 6,
	  'open_http2_protocol' => true,
	  'compression_min_length' => 128,
	  'backlog' => 100000,
	  'reload_async' => false,
	]);
	$Server->on('Request', static function (Request $Request, Response $Response): void {
	  file_put_contents('FILENAME', 'CONTENTS');
	  $Response->end(file_get_contents('FILENAME'));
	});
	$Server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECTF--
CONTENTSDONE
