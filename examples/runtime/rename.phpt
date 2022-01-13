<?php
Swoole\Runtime::enableCoroutine();
Co\run(function () {
    var_dump(rename('data.txt', 'data2.txt'));
});
