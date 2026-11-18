--TEST--
openswoole_coroutine: select() reports push channels, keeps array keys and checks types
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine;
use OpenSwoole\Coroutine\Channel;

co::run(function () {
    $empty = new Channel(1);
    $full = new Channel(1);
    $full->push('x');

    // pull: only $full has data; push: only $empty has room
    $result = Coroutine::select(['a' => $empty, 'b' => $full], ['c' => $empty, 'd' => $full], 1);
    var_dump(array_keys($result['read']), array_keys($result['write']));

    // non-sequential integer keys
    $result = Coroutine::select([5 => $empty, 9 => $full], [], 1);
    var_dump(array_keys($result['read']));

    try {
        Coroutine::select([1], []);
    } catch (TypeError $e) {
        echo get_class($e), "\n";
    }
});
?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "b"
}
array(1) {
  [0]=>
  string(1) "c"
}
array(1) {
  [0]=>
  int(9)
}
TypeError
