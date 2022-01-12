<?php declare(strict_types = 1);

// Example of selector notified by channel close

Co::set(['hook_flags' => SWOOLE_HOOK_ALL]);

Co\run(function () {
    $chan1 = new chan(1);
    $chan2 = new chan(1);

    go(function() use ($chan1) {
        co::sleep(1);
        $chan1->close();
    });

    go(function() use ($chan2) {
        co::sleep(4);
    });

    go(function() use ($chan1, $chan2){
        while(1) {
            $ret = co::select([$chan1, $chan2], [], 10);
            array_values($ret['pull_chans'])[0]->pop();
            if(array_values($ret['pull_chans'])[0]->errCode === -2) {
                break;
            }
        }
    });
});