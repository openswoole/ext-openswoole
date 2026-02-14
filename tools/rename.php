<?php

$dirs = glob('./src/*');

foreach ($dirs as $d) {
    $files = glob($d.'/*.c');
    foreach ($files as $f) {
        shell_exec("git mv $f {$f}c");
    }
}