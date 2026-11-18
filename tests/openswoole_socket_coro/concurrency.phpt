--TEST--
openswoole_socket_coro: concurrency
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// fixed ports collide when tests run in parallel
$port = get_one_free_port();

const N = 100;

$socket = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, 0);
$socket->bind('127.0.0.1', $port);
$socket->listen(128);

go(function () use ($port, $socket)
{
    for ($i = 0; $i < N; $i++)
    {
        $client = $socket->accept();
        go(function () use ($port, $client)
        {
            while (true)
            {
                $data = $client->recv();
                if (empty($data))
                {
                    $client->close();
                    break;
                }
                $client->send("Server: $data");
            }
        });
    }
});

for ($i = 0; $i < N; $i++)
{
    go(function () use ($port)
    {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        $ret = $cli->connect('127.0.0.1', $port);
        if ($ret)
        {
            $cli->send("hello\n");
            Assert::same($cli->recv(), 'Server: hello'."\n");
            $cli->close();
        }
        else
        {
            echo "ERROR\n";
        }
    });
}
openswoole_event_wait();
?>
--EXPECT--
