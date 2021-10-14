<?php
if (substr(PHP_OS, 0, 3) == 'WIN')
{
    exit("skip for Windows");
}
if (!extension_loaded("openswoole"))
{
    exit("openswoole extension is required");
}
