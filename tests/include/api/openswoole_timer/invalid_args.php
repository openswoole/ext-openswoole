<?php

require_once __DIR__ . "/../../../include/bootstrap.php";

//openswoole_function openswoole_timer_after($ms, $callback, $param = null) {}
//openswoole_function openswoole_timer_tick($ms, $callback) {}
//openswoole_function openswoole_timer_clear($timer_id) {}

openswoole_timer_after(-1, function(){ });
openswoole_timer_tick(-1, function() { });
openswoole_timer_after(86400001, function(){ });
openswoole_timer_tick(86400001, function() { });
openswoole_timer_clear(-1);

for ($i = 0; $i < 1000; $i++) {
    openswoole_timer_clear(openswoole_timer_after(1, function() {}));
}

//openswoole_timer_after(1, null);
//openswoole_timer_after(1, "strlen");

function sw_timer_pass_ref(&$ref_func) {
    openswoole_timer_after(1, $ref_func);
}
$func = function() {};
sw_timer_pass_ref($func);
