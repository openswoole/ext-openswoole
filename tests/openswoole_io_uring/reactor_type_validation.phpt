--TEST--
openswoole_io_uring: reactor_type validation rejects invalid values
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// Valid constants exist
Assert::true(defined('OPENSWOOLE_IO_URING'));
Assert::true(defined('OPENSWOOLE_EPOLL'));
Assert::true(defined('OPENSWOOLE_KQUEUE'));
Assert::true(defined('OPENSWOOLE_POLL'));
Assert::true(defined('OPENSWOOLE_SELECT'));
echo "constants defined\n";

// Invalid reactor_type should produce a warning
@Co::set(['reactor_type' => 999]);
$err = error_get_last();
Assert::contains($err['message'], 'unsupported reactor_type');
echo "invalid rejected\n";

echo "DONE\n";
?>
--EXPECT--
constants defined
invalid rejected
DONE
