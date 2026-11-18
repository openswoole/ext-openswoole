--TEST--
openswoole_channel_coro: coroutine wait
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
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
        Assert::true(isset($json['http']) and $json['http'] > 1024);
        Assert::true(isset($json['https']) and $json['https'] > 1024);
        $pm->kill();
    });
    OpenSwoole\Event::wait();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm)
{
    $local = SwooleTest\LocalServer::start();
    $serv = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $serv->on("WorkerStart", function () use ($pm) {
        $pm->wakeup();
    });
    $serv->on('request', function ($req, $resp) use ($local) {

        $chan = new chan(2);
        go(function () use ($chan, $local) {
            $cli = new OpenSwoole\Coroutine\Http\Client($local->host, $local->httpPort);
            $cli->set(['timeout' => 10]);
            $cli->setHeaders([
                'Host' => "{$local->host}:{$local->httpPort}",
                "User-Agent" => 'Chrome/49.0.2587.3',
                'Accept' => 'text/html,application/xhtml+xml,application/xml',
                'Accept-Encoding' => 'gzip',
            ]);
            $ret = $cli->get('/');
            if ($ret)
            {
                $chan->push(['http' => strlen($cli->body)]);
            }
            else
            {
                $chan->push(['http' => 0]);
            }
        });

        go(function () use ($chan, $local) {
            $cli = new OpenSwoole\Coroutine\Http\Client($local->host, $local->httpsPort, true);
            $cli->set(['timeout' => 10, 'ssl_verify_peer' => false]);
            $cli->setHeaders([
                'Host' => "{$local->host}:{$local->httpsPort}",
                "User-Agent" => 'Chrome/49.0.2587.3',
                'Accept' => 'text/html,application/xhtml+xml,application/xml',
                'Accept-Encoding' => 'gzip',
            ]);
            $ret = $cli->get('/');
            if ($ret)
            {
                $chan->push(['https' => strlen($cli->body)]);
            }
            else
            {
                $chan->push(['https' => 0]);
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
