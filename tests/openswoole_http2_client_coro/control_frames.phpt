--TEST--
openswoole_http2_client_coro: PING and GOAWAY frames from the server
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Http2\Client;
use OpenSwoole\Coroutine\Socket;

function frame(int $type, int $flags, int $streamId, string $payload): string
{
    return substr(pack('N', strlen($payload)), 1) . chr($type) . chr($flags) . pack('N', $streamId) . $payload;
}

// reads frames from the client until one of the given type and flags arrives
function read_frame(Socket $conn, string &$buffer, int $type, int $flags): ?string
{
    while (true) {
        while (strlen($buffer) >= 9) {
            $length = unpack('N', "\0" . substr($buffer, 0, 3))[1];
            if (strlen($buffer) < 9 + $length) {
                break;
            }
            $frame = substr($buffer, 0, 9 + $length);
            $buffer = substr($buffer, 9 + $length);
            if (ord($frame[3]) === $type && ord($frame[4]) === $flags) {
                return substr($frame, 9);
            }
        }
        $data = $conn->recv(65536, 5);
        if ($data === '' || $data === false) {
            return null;
        }
        $buffer .= $data;
    }
}

co::run(function () {
    $server = new Socket(AF_INET, SOCK_STREAM, 0);
    Assert::true($server->bind('127.0.0.1', 0));
    Assert::true($server->listen());
    $port = $server->getsockname()['port'];

    // 1: PING, then a GOAWAY that is too short
    // 2: a valid GOAWAY with debug data
    go(function () use ($server) {
        for ($case = 1; $case <= 2; $case++) {
            $conn = $server->accept(5);
            // the client preface, then its SETTINGS
            $buffer = '';
            while (strlen($buffer) < 24 && ($data = $conn->recv(65536, 5))) {
                $buffer .= $data;
            }
            Assert::same(substr($buffer, 0, 24), "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n");
            $buffer = substr($buffer, 24);
            read_frame($conn, $buffer, 4, 0);
            $conn->send(frame(4, 0, 0, ''));
            if ($case === 1) {
                $conn->send(frame(6, 0, 0, 'abcdefgh'));
                echo 'ping ack: ', read_frame($conn, $buffer, 6, 1), "\n";
                $conn->send(frame(7, 0, 0, "\0\0\0\0"));
            } else {
                $conn->send(frame(7, 0, 0, pack('NN', 0, 11) . 'bye'));
            }
            $conn->recv(65536, 1);
            $conn->close();
        }
    });

    $cli = new Client('127.0.0.1', $port);
    Assert::true($cli->connect());
    Assert::false($cli->recv(5));
    echo "short goaway rejected\n";

    $cli = new Client('127.0.0.1', $port);
    Assert::true($cli->connect());
    Assert::false($cli->recv(5));
    echo "goaway: {$cli->errCode} {$cli->errMsg}\n";
});
?>
--EXPECTF--
ping ack: abcdefgh
[%s]	WARNING	Client::parse_frame() (ERRNO 7102): http2 frame type=7 has an invalid length 4
short goaway rejected
goaway: 11 bye
