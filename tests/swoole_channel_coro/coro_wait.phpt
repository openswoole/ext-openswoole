--TEST--
swoole_channel_coro: coroutine wait
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;

$pm->parentFunc = function () use ($pm) {
    go(function () use ($pm) {
        $data = httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
        Assert::assert(!empty($data));
        $json = json_decode($data, true);
        Assert::assert(is_array($json));
        Assert::true(isset($json['openswoole.com']) and $json['openswoole.com'] > 1024);
        Assert::true(isset($json['www.google.com']) and $json['www.google.com'] > 1024);
        $pm->kill();
    });
    Swoole\Event::wait();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm)
{
    $serv = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $serv->on("WorkerStart", function () use ($pm) {
        $pm->wakeup();
    });
    $serv->on('request', function ($req, $resp) {

        $chan = new chan(2);
        go(function () use ($chan) {
            $cli = new OpenSwoole\Coroutine\Http\Client('openswoole.com', 443, true);
            $cli->set(['timeout' => 10]);
            $cli->setHeaders([
                'Host' => "openswoole.com",
                "User-Agent" => 'Chrome/49.0.2587.3',
                'Accept' => 'text/html,application/xhtml+xml,application/xml',
                'Accept-Encoding' => 'gzip',
            ]);
            $ret = $cli->get('/');
            if ($ret)
            {
                $chan->push(['openswoole.com' => strlen($cli->body)]);
            }
            else
            {
                $chan->push(['openswoole.com' => 0]);
            }
        });

        go(function () use ($chan) {
            $cli = new OpenSwoole\Coroutine\Http\Client('www.google.com', 443, true);
            $cli->set(['timeout' => 10]);
            $cli->setHeaders([
                'Host' => "www.google.com",
                "User-Agent" => 'Chrome/49.0.2587.3',
                'Accept' => 'text/html,application/xhtml+xml,application/xml',
                'Accept-Encoding' => 'gzip',
            ]);
            $ret = $cli->get('/');
            if ($ret)
            {
                $chan->push(['www.google.com' => strlen($cli->body)]);
            }
            else
            {
                $chan->push(['www.google.com' => 0]);
            }
        });

        $result = [];
        for ($i = 0; $i < 2; $i++)
        {
            $result += $chan->pop();
        }
        $resp->end(json_encode($result));
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
