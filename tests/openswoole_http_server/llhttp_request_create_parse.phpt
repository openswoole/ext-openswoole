--TEST--
openswoole_http_server: llhttp - Request::create() and parse() API
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Http\Request;

// Test 1: parse a simple GET request
$req = Request::create();
$data = "GET /api/users?page=1&limit=10 HTTP/1.1\r\n" .
    "Host: example.com\r\n" .
    "Accept: text/html\r\n" .
    "X-Custom: hello\r\n" .
    "\r\n";
$parsed = $req->parse($data);
Assert::eq($parsed, strlen($data));
Assert::true($req->isCompleted());
Assert::same($req->getMethod(), 'GET');
Assert::same($req->server['request_method'], 'GET');
Assert::same($req->server['request_uri'], '/api/users');
Assert::same($req->server['path_info'], '/api/users');
Assert::same($req->server['query_string'], 'page=1&limit=10');
Assert::same($req->server['server_protocol'], 'HTTP/1.1');
Assert::same($req->get['page'], '1');
Assert::same($req->get['limit'], '10');
Assert::same($req->header['host'], 'example.com');
Assert::same($req->header['accept'], 'text/html');
Assert::same($req->header['x-custom'], 'hello');
echo "TEST 1 PASSED\n";

// Test 2: parse a POST request with body
$req2 = Request::create();
$body = 'username=admin&password=test';
$data2 = "POST /login HTTP/1.1\r\n" .
    "Host: example.com\r\n" .
    "Content-Type: application/x-www-form-urlencoded\r\n" .
    "Content-Length: " . strlen($body) . "\r\n" .
    "\r\n" .
    $body;
$parsed = $req2->parse($data2);
Assert::eq($parsed, strlen($data2));
Assert::true($req2->isCompleted());
Assert::same($req2->getMethod(), 'POST');
Assert::same($req2->post['username'], 'admin');
Assert::same($req2->post['password'], 'test');
Assert::same($req2->rawContent(), $body);
echo "TEST 2 PASSED\n";

// Test 3: parse request with cookies
$req3 = Request::create();
$data3 = "GET /dashboard HTTP/1.1\r\n" .
    "Host: example.com\r\n" .
    "Cookie: sid=abc123; theme=dark\r\n" .
    "\r\n";
$parsed = $req3->parse($data3);
Assert::eq($parsed, strlen($data3));
Assert::true($req3->isCompleted());
Assert::same($req3->cookie['sid'], 'abc123');
Assert::same($req3->cookie['theme'], 'dark');
echo "TEST 3 PASSED\n";

// Test 4: parse HTTP/1.0 request
$req4 = Request::create();
$data4 = "GET / HTTP/1.0\r\n" .
    "Host: example.com\r\n" .
    "\r\n";
$parsed = $req4->parse($data4);
Assert::eq($parsed, strlen($data4));
Assert::true($req4->isCompleted());
Assert::same($req4->server['server_protocol'], 'HTTP/1.0');
echo "TEST 4 PASSED\n";

// Test 5: incremental parsing
$req5 = Request::create();
$part1 = "GET /incremental HTTP/1.1\r\n";
$part2 = "Host: example.com\r\n";
$part3 = "X-Token: secret\r\n\r\n";
$req5->parse($part1);
Assert::false($req5->isCompleted());
$req5->parse($part2);
Assert::false($req5->isCompleted());
$req5->parse($part3);
Assert::true($req5->isCompleted());
Assert::same($req5->server['request_uri'], '/incremental');
Assert::same($req5->header['x-token'], 'secret');
echo "TEST 5 PASSED\n";

echo "DONE\n";
?>
--EXPECT--
TEST 1 PASSED
TEST 2 PASSED
TEST 3 PASSED
TEST 4 PASSED
TEST 5 PASSED
DONE
