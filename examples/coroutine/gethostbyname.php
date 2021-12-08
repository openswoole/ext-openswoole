<?php
use Swoole\Coroutine as co;

co::create(function() {
    $ip = co::gethostbyname("www.google.com");
    echo "IP: $ip\n";
});
