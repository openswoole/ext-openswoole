<?php
Swoole\Runtime::enableCoroutine();
Co\run(function () {
    sleep(1);
});
