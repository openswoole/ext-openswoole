--TEST--
openswoole_http_server: Request::create() and parse() with a multipart body
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Http\Request;

function multipart_request(string $body): string
{
    return "POST / HTTP/1.1\r\n" .
        "Host: example.com\r\n" .
        "Content-Type: multipart/form-data; boundary=XX\r\n" .
        "Content-Length: " . strlen($body) . "\r\n" .
        "\r\n" .
        $body;
}

$req = Request::create();
$data = multipart_request("--XX\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\nhello\r\n" .
    "--XX\r\nContent-Disposition: form-data; name=\"f\"; filename=\"a.txt\"\r\nContent-Type: text/plain\r\n\r\nfile\r\n" .
    "--XX--\r\n");
Assert::eq($req->parse($data), strlen($data));
Assert::true($req->isCompleted());
Assert::same($req->post, ['a' => 'hello']);
Assert::same($req->files['f']['name'], 'a.txt');
Assert::same($req->files['f']['type'], 'text/plain');
Assert::same($req->files['f']['size'], 4);
Assert::same(file_get_contents($req->files['f']['tmp_name']), 'file');
echo "TEST 1 PASSED\n";

// field without closing boundary, then a normal request
$req = Request::create();
$req->parse(multipart_request("--XX\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\nFIRSTVALUE"));
Assert::true($req->isCompleted());
unset($req);
$req = Request::create();
$req->parse(multipart_request("--XX\r\nContent-Disposition: form-data; name=\"b\"\r\n\r\nsecond\r\n--XX--\r\n"));
Assert::same($req->post, ['b' => 'second']);
echo "TEST 2 PASSED\n";

// upload part without closing boundary leaves no temp file
$req = Request::create();
$req->parse(multipart_request("--XX\r\nContent-Disposition: form-data; name=\"f\"; filename=\"a.txt\"\r\n\r\nFILEDATA"));
Assert::true($req->isCompleted());
Assert::count($req->tmpfiles, 1);
Assert::false(file_exists($req->tmpfiles[0]));
unset($req);
echo "TEST 3 PASSED\n";

// parts with no name, or with a repeated Content-Disposition header
$bodies = [
    "--XX\r\nContent-Disposition: form-data; foo=\"a\"\r\n\r\nx\r\n--XX--\r\n",
    "--XX\r\nContent-Disposition: form-data; name=\"f\"; filename=\"a\"\r\n" .
        "Content-Disposition: form-data; name=\"g\"; filename=\"b\"\r\n\r\nx\r\n--XX--\r\n",
    "--XX\r\nContent-Disposition: form-data; name=\"f\"; filename=\"a\"\r\n" .
        "Content-Disposition: form-data; name=\"g\"\r\n\r\nx\r\n--XX--\r\n",
];
foreach ($bodies as $body) {
    for ($i = 0; $i < 100; $i++) {
        $req = Request::create();
        $req->parse(multipart_request($body));
        unset($req);
    }
    $m0 = memory_get_usage();
    for ($i = 0; $i < 2000; $i++) {
        $req = Request::create();
        $req->parse(multipart_request($body));
        unset($req);
    }
    Assert::lessThan(memory_get_usage() - $m0, 16 * 1024);
}
$req = Request::create();
$req->parse(multipart_request($bodies[1]));
Assert::same(array_keys($req->files), ['g']);
Assert::same($req->files['g']['name'], 'b');
$req = Request::create();
$req->parse(multipart_request($bodies[2]));
Assert::same($req->post, ['g' => 'x']);
Assert::null($req->files);
echo "TEST 4 PASSED\n";
?>
--EXPECT--
TEST 1 PASSED
TEST 2 PASSED
TEST 3 PASSED
TEST 4 PASSED
