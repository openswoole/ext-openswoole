--TEST--
openswoole_io_uring: concurrent coroutine socket I/O with io_uring reactor
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);

Co::run(function () {
    $n = 5;
    $chan = new OpenSwoole\Coroutine\Channel($n);

    for ($i = 0; $i < $n; $i++) {
        go(function () use ($i, $chan) {
            $pair = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
            $read_sock = $pair[0];
            $write_sock = $pair[1];
            stream_set_blocking($read_sock, false);
            stream_set_blocking($write_sock, false);

            $data = "coroutine $i data";

            // Writer
            go(function () use ($write_sock, $data) {
                Co::sleep(1);
                fwrite($write_sock, $data);
                fclose($write_sock);
            });

            // Reader
            $content = '';
            while (!feof($read_sock)) {
                $chunk = @fread($read_sock, 8192);
                if ($chunk === false || $chunk === '') {
                    Co::sleep(1);
                    continue;
                }
                $content .= $chunk;
            }
            fclose($read_sock);

            $chan->push($content === $data ? "coroutine $i ok" : "coroutine $i FAIL");
        });
    }

    $results = [];
    for ($i = 0; $i < $n; $i++) {
        $results[] = $chan->pop();
    }
    sort($results);
    foreach ($results as $r) {
        echo "$r\n";
    }
});

echo "DONE\n";
?>
--EXPECT--
coroutine 0 ok
coroutine 1 ok
coroutine 2 ok
coroutine 3 ok
coroutine 4 ok
DONE
