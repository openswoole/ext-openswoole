<?php
require __DIR__ . "/coro_include.php";
use Swoole\Coroutine as co;

co::create(function () {
    $ip = co::gethostbyname('www.google.com');
    var_dump($ip);
});
echo "111\n";

echo "222\n";
