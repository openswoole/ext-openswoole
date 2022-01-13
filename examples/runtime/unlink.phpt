<?php
Swoole\Runtime::enableCoroutine();
Co\run(function () {
    var_dump(unlink('data.txt'));
});
