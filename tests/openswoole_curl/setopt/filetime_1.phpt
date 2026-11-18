--TEST--
openswoole_curl/setopt: CURLOPT_FILETIME
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

// the local server's page has a Last-Modified header
$server = SwooleTest\LocalServer::start();

$cm = new \SwooleTest\CurlManager();
$cm->run(function ($host) use ($server) {

    $ch = curl_init();

    $options = array(
        CURLOPT_URL => $server->url('/', true),
        CURLOPT_SSL_VERIFYPEER => false,
        CURLOPT_RETURNTRANSFER => 1,
        CURLOPT_FILETIME => true,
        CURLOPT_NOBODY => true,
    );

    curl_setopt_array($ch, $options);
    curl_exec($ch);
    $info = curl_getinfo($ch);
    Assert::assert(!empty($info['filetime']));
    Assert::greaterThanEq($info['filetime'], 1000000);
    curl_close($ch);
    echo "END\n";
}, false);

?>
--EXPECT--
END
