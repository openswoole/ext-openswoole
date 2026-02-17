--TEST--
swoole_coroutine_fiber: xdebug tracing support with fiber context
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_extension_not_exist('xdebug');
?>
--INI--
xdebug.mode=trace
xdebug.start_with_request=yes
xdebug.output_dir={TMP}
xdebug.use_compression=false
xdebug.trace_format=0
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['use_fiber_context' => true]);

function fetchData(string $url): string {
    Co::usleep(1000);
    return "data from $url";
}

function processItem(int $id): string {
    $data = fetchData("/api/item/$id");
    return "[$id] $data";
}

Co::run(function () {
    go(function () {
        echo processItem(1) . "\n";
    });
    go(function () {
        echo processItem(2) . "\n";
    });
});

// find and verify the trace file
$traceFiles = glob(sys_get_temp_dir() . '/trace.*.xt');
if (empty($traceFiles)) {
    echo "TRACE NOT FOUND\n";
    exit;
}

$trace = file_get_contents(end($traceFiles));

// xdebug trace must contain our function calls across yield points
Assert::assert(strpos($trace, 'processItem') !== false);
Assert::assert(strpos($trace, 'fetchData') !== false);

echo "TRACE OK\n";

// cleanup
foreach ($traceFiles as $f) {
    @unlink($f);
}
?>
--EXPECT--
[1] data from /api/item/1
[2] data from /api/item/2
TRACE OK
