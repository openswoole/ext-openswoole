<?php
Swoole\Runtime::enableCoroutine();
Co\run(function () {
    var_dump(gethostbyname("openswoole.com"));
});

