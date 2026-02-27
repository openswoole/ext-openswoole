--TEST--
openswoole_event: deprecated_event_wait
--SKIPIF--
<?php

use SebastianBergmann\CodeCoverage\Report\PHP;



require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

error_reporting(E_ALL & E_DEPRECATED);

co::run(function () {
    throw new Exception("Error Processing Request", 1);
});

?>
--EXPECT--
