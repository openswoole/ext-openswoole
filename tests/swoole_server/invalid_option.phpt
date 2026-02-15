--TEST--
swoole_server: invalid option
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Server;

$serv = new Server('127.0.0.1', 0, SWOOLE_BASE);
$options = [
    'worker_num' => 1,
    'backlog' => 128,
    'invalid_option' => true,
];

try {
    $serv->set($options);
} catch (\OpenSwoole\Exception $e) {
    echo $e->getMessage();
}

?>
--EXPECTF--
Fatal error: OpenSwoole\Server\Port::set(): Invalid server option: invalid_option in %s/invalid_option.php on line %d%A
