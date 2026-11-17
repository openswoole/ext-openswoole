--TEST--
openswoole_coroutine: user coroutine
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
use OpenSwoole\Coroutine\Http\Client as HttpClient;
$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm)
{
    go(function () use ($pm) {
        $data= httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
        Assert::assert(strlen($data) > 100);
        Assert::contains($data, 'OpenSwoole test page');
        $pm->kill();
    });
    OpenSwoole\Event::wait();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm)
{
    $local = SwooleTest\LocalServer::start();
    $http = new openswoole_http_server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set(array(
        'log_file' => '/dev/null'
    ));
    $http->on("WorkerStart", function (\openswoole_server $serv)
    {
        /**
         * @var $pm ProcessManager
         */
        global $pm;
        $pm->wakeup();
    });
    $http->on('request', function (openswoole_http_request $request, openswoole_http_response $response) use ($local)
    {
        OpenSwoole\Coroutine::create(function () use ($response, $local)
        {
            $url = "http://localhost:{$local->httpPort}/";
            $components = parse_url($url);

            if (!isset($components['host']))
            {
                throw new \Exception("{$url} parse no host");
            }

            $host = $components['host'];

            $ip = \OpenSwoole\Coroutine::dnsLookup($host);
            $port = isset($components['port']) ? $components['port'] : 80;
            $client = new HttpClient($ip, $port);

            $client->setHeaders([
                'Host' => "{$host}:{$port}",
                'User-Agent' => 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:28.0) Gecko/20100101 Firefox/28.0',
            ]);
            $client->set(['timeout' => 10]);
            $client->get(isset($components['path']) ? $components['path'] : '/');

            $response->end($client->body);
        });
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
