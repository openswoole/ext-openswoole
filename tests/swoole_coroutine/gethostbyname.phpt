--TEST--
swoole_coroutine: gethostbyname and dns cache
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use  Swoole\Coroutine\System;
use  Swoole\Coroutine;

Co::run(function () {
    $map = [
        'www.google.com' => null,
        'www.youtube.com' => null,
        'www.facebook.com' => null,
    ];

    $first_time = microtime(true);
    for ($n = 4; $n--;) {
        foreach ($map as $host => &$ip) {
            $ip = System::gethostbyname($host);
            Assert::assert(preg_match(IP_REGEX, $ip));
        }
    }
    unset($ip);
    $first_time = microtime(true) - $first_time;
    phpt_var_dump($map);

    $cache_time = microtime(true);
    for ($n = 4; $n--;) {
        foreach ($map as $host => $ip) {
            $_ip = System::gethostbyname($host);
            Assert::same($ip, $_ip);
        }
    }
    $cache_time = microtime(true) - $cache_time;

    $no_cache_time = microtime(true);
    for ($n = 4; $n--;) {
        System::clearDNSCache();
        $ip = System::gethostbyname(array_rand($map));
        Assert::assert(preg_match(IP_REGEX, $ip));
    }
    $no_cache_time = microtime(true) - $no_cache_time;

    $chan = new Chan(4);
    $no_cache_multi_time = microtime(true);
    for ($c = 4; $c--;) {
        go(function () use ($map, $chan) {
            System::clearDNSCache();
            $ip = System::gethostbyname(array_rand($map));
            Assert::assert(filter_var($ip, FILTER_VALIDATE_IP, FILTER_FLAG_IPV4));
            $chan->push(Assert::assert(preg_match(IP_REGEX, $ip)));
        });
    }
    for ($c = 4; $c--;) {
        $chan->pop();
    }
    $no_cache_multi_time = microtime(true) - $no_cache_multi_time;

    phpt_var_dump($first_time, $cache_time, $no_cache_time, $no_cache_multi_time);
    if (!IS_IN_TRAVIS) {
        Assert::assert($cache_time < 0.01);
        Assert::assert($cache_time < $first_time);
        Assert::assert($cache_time < $no_cache_time);
        Assert::assert($cache_time < $no_cache_multi_time);
        Assert::assert($no_cache_multi_time < $no_cache_time);
    }
});
?>
--EXPECTF--
