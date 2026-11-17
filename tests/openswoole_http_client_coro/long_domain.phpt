--TEST--
openswoole_http_client_coro: long domain
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Socket;

$server = SwooleTest\LocalServer::start();

function http_get(string $url, int $port)
{
    $url_info = parse_url($url);
    $domain = $url_info['host'];
    $cli = new OpenSwoole\Coroutine\Http\Client($domain, $port);
    $cli->set(['timeout' => 5]);
    $cli->setHeaders([
        'Host' => $domain,
        'User-Agent' => 'Chrome/49.0.2587.3',
        'Accept' => 'text/html,application/xhtml+xml,application/xml',
        'Accept-Encoding' => 'gzip'
    ]);
    Assert::assert($cli->get('/'), var_dump_return($cli->errCode, $cli->errMsg));
    Assert::same($cli->statusCode, 200);
    Assert::assert(!empty($cli->body));
}

// a local DNS server that answers every A query with 127.0.0.1
function dns_serve(Socket $dns)
{
    while (($query = $dns->recvfrom($peer)) !== false) {
        if (strlen($query) < 17) {
            continue;
        }
        $offset = 12;
        while ($offset < strlen($query) && ($len = ord($query[$offset])) !== 0) {
            $offset += $len + 1;
        }
        $question = substr($query, 12, $offset + 5 - 12);
        $qtype = unpack('n', substr($query, $offset + 1, 2))[1];
        $answer = $qtype === 1 ? "\xc0\x0c" . pack('nnNn', 1, 1, 60, 4) . inet_pton('127.0.0.1') : '';
        $response = substr($query, 0, 2) . pack('nnnnn', 0x8180, 1, $answer ? 1 : 0, 0, 0) . $question . $answer;
        $dns->sendto($peer['address'], $peer['port'], $response);
    }
}

Co::run(function () use ($server) {
    $dns = new Socket(AF_INET, SOCK_DGRAM, 0);
    Assert::true($dns->bind('127.0.0.1', 0));
    Co::set(['dns_server' => '127.0.0.1:' . $dns->getsockname()['port']]);
    go(function () use ($dns) {
        dns_serve($dns);
    });

    $urls = [
        'http://888888888888888888888888888888888888888888888888888888888888888.com',
        'http://www.abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk.com',
        'http://www.mamashuojiusuannizhucedeyumingzaichanggoogledounengsousuochulai.cn',
    ];
    $done = new OpenSwoole\Coroutine\Channel(count($urls));
    foreach ($urls as $url) {
        go(function () use ($done, $url, $server) {
            http_get($url, $server->httpPort);
            $done->push(true);
        });
    }
    foreach ($urls as $url) {
        $done->pop();
    }
    $dns->close();
});
?>
--EXPECT--
