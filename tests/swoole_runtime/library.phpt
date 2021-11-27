--TEST--
swoole_runtime: library
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$useShortName = !in_array(
    strtolower(trim(str_replace('0', '',
        ini_get_all('openswoole')['swoole.use_shortname']['local_value']
    ))),
    ['', 'off', 'false'],
    true
);
phpt_var_dump($useShortName, SWOOLE_USE_SHORTNAME);
var_dump(Assert::eq($useShortName, SWOOLE_USE_SHORTNAME));
?>
--EXPECT--
bool(true)
