--TEST--
openswoole_coroutine_system: getaddrinfo
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
co::run(function () {
    $ip_list = OpenSwoole\Coroutine\System::getaddrinfo('localhost', AF_INET);
    Assert::assert(!empty($ip_list) and is_array($ip_list));
    foreach ($ip_list as $ip) {
        Assert::same($ip, '127.0.0.1');
    }
    echo "DONE\n";
});
?>
--EXPECT--
DONE
