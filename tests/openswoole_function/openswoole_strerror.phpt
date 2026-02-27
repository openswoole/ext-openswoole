--TEST--
openswoole_function: OpenSwoole\Util::getErrorMessage
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
if (!is_musl_libc()) {
    Assert::assert(
        OpenSwoole\Util::getErrorMessage(IS_MAC_OS ? 4 : -4 /*EAI_FAIL*/, SWOOLE_STRERROR_GAI) ===
        'Non-recoverable failure in name resolution'
    );
    Assert::assert(
        OpenSwoole\Util::getErrorMessage(2 /*NO_ADDRESS*/, SWOOLE_STRERROR_DNS) ===
        'Host name lookup failure'
    );
}
echo OpenSwoole\Util::getErrorMessage(SOCKET_ECONNRESET) . "\n";
echo OpenSwoole\Util::getErrorMessage(SWOOLE_ERROR_FILE_NOT_EXIST) . "\n";
if (!is_musl_libc()) {
    $unknown = OpenSwoole\Util::getErrorMessage(SWOOLE_ERROR_MALLOC_FAIL - 1);
    $sw_unknown = OpenSwoole\Util::getErrorMessage(SWOOLE_ERROR_MALLOC_FAIL - 1, SWOOLE_STRERROR_SWOOLE);
    Assert::same($unknown, $sw_unknown);
} else {
    Assert::same(OpenSwoole\Util::getErrorMessage(SWOOLE_ERROR_MALLOC_FAIL - 1), 'No error information');
}
?>
--EXPECT--
Connection reset by peer
File not exist
