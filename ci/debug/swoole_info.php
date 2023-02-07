<?php
var_dump([
    'version' => OpenSwoole\Util::getVersion(),
    'cpu_num' => OpenSwoole\Util::getCPUNum(),
    'local_mac' => OpenSwoole\Util::getLocalMac(),
    'local_ip' => OpenSwoole\Util::getLocalIp()
]);
