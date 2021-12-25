<?php
Swoole\Runtime::enableCoroutine();
Co\run(function () {
    var_dump(mkdir(__DIR__.'/test'));
    var_dump(rmdir(__DIR__.'/test'));
});
