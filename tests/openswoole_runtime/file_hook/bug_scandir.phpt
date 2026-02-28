--TEST--
openswoole_runtime/file_hook: bug #3792
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$testDir = sys_get_temp_dir() . '/openswoole_scandir_bug';

if (!is_dir($testDir)) {
    mkdir($testDir);
}
for ($i = 0; $i++ < 3;) {
    touch("{$testDir}/{$i}.txt");
}

\OpenSwoole\Runtime::enableCoroutine(true);
co::run(
    function () use ($testDir) {
        for ($i = 0; $i < MAX_CONCURRENCY; $i++) {
            go(
                function () use ($testDir) {
                    $files = scandir($testDir);
                    Assert::same($files, [
                        '.',
                        '..',
                        '1.txt',
                        '2.txt',
                        '3.txt',
                    ]);
                }
            );
        }
    }
);

echo "DONE\n";

?>
--EXPECT--
DONE
