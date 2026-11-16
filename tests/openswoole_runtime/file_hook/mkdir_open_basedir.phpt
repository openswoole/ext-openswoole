--TEST--
openswoole_runtime/file_hook: mkdir respects open_basedir and reports errors
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$base = sys_get_temp_dir() . '/openswoole_mkdir_open_basedir';
@rmdir("$base/denied/a/b");
@rmdir("$base/denied/a");
@rmdir("$base/denied");
@rmdir("$base/allowed/x");
@rmdir("$base/allowed");
@rmdir($base);
mkdir("$base/allowed", 0777, true);

ini_set('open_basedir', "$base/allowed");

OpenSwoole\Runtime::enableCoroutine(OpenSwoole\Runtime::HOOK_ALL);
co::run(function () use ($base) {
    var_dump(mkdir("$base/denied"));
    var_dump(mkdir("$base/denied/a/b", 0777, true));
    var_dump(mkdir("$base/allowed/x"));
    var_dump(mkdir("$base/allowed/x"));
    var_dump(mkdir("$base/allowed/none/y"));
    var_dump(is_dir("$base/allowed/x"));
    var_dump(rmdir("$base/allowed/x"));
});
?>
--EXPECTF--
Warning: mkdir(): open_basedir restriction in effect. File(%s/denied) is not within the allowed path(s): (%s/allowed) in %s on line %d
bool(false)

Warning: mkdir(): open_basedir restriction in effect. File(%s/denied/a/b) is not within the allowed path(s): (%s/allowed) in %s on line %d
bool(false)
bool(true)

Warning: mkdir(): File exists in %s on line %d
bool(false)

Warning: mkdir(): No such file or directory in %s on line %d
bool(false)
bool(true)
bool(true)
