--TEST--
openswoole_server/ssl: client certificate larger than the certificate buffer
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
skip_if_openssl_version_lower_than('1.1.0');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

// self-signed client certificate whose PEM is larger than 64 KiB
$dir = sys_get_temp_dir() . '/openswoole_large_cert_' . getmypid();
@mkdir($dir);
$cnf = "$dir/openssl.cnf";
file_put_contents($cnf, "[req]\ndistinguished_name = dn\n[dn]\n[big]\nnsComment = \"" . str_repeat('A', 60000) . "\"\n");
$options = ['config' => $cnf, 'x509_extensions' => 'big', 'digest_alg' => 'sha256'];
$key = openssl_pkey_new(['config' => $cnf, 'private_key_bits' => 2048, 'private_key_type' => OPENSSL_KEYTYPE_RSA]);
$csr = openssl_csr_new(['commonName' => 'large'], $key, $options);
$cert = openssl_csr_sign($csr, null, $key, 1, $options);
openssl_x509_export($cert, $pem);
openssl_pkey_export($key, $keyPem, null, ['config' => $cnf]);
Assert::greaterThan(strlen($pem), 65536);
file_put_contents("$dir/cert.pem", $pem);
file_put_contents("$dir/key.pem", $keyPem);

$pm = new SwooleTest\ProcessManager;

$pm->parentFunc = function ($pid) use ($pm, $dir) {
    co::run(function () use ($pm, $dir) {
        $certs = [
            ["$dir/cert.pem", "$dir/key.pem"],
            [__DIR__ . '/../../include/api/ssl-ca/client-cert.pem', __DIR__ . '/../../include/api/ssl-ca/client-key.pem'],
        ];
        foreach ($certs as $i => [$certFile, $keyFile]) {
            $client = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP | SWOOLE_SSL);
            $client->set(['ssl_cert_file' => $certFile, 'ssl_key_file' => $keyFile]);
            if (!$client->connect('127.0.0.1', $pm->getFreePort())) {
                exit("connect failed\n");
            }
            $client->send("hello $i");
            echo $client->recv(), "\n";
            $client->close();
        }
    });
    $pm->kill();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm) {
    $serv = new OpenSwoole\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE, SWOOLE_SOCK_TCP | SWOOLE_SSL);
    $serv->set([
        'log_file' => '/dev/null',
        'worker_num' => 1,
        'ssl_cert_file' => __DIR__ . '/../../include/api/ssl-ca/server-cert.pem',
        'ssl_key_file' => __DIR__ . '/../../include/api/ssl-ca/server-key.pem',
        'ssl_client_cert_file' => __DIR__ . '/../../include/api/ssl-ca/ca-cert.pem',
    ]);
    $serv->on("workerStart", function ($serv) use ($pm) {
        $pm->wakeup();
    });
    $serv->on('receive', function ($serv, $fd, $tid, $data) {
        $serv->send($fd, "echo $data");
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
foreach (glob("$dir/*") as $file) {
    unlink($file);
}
rmdir($dir);
?>
--EXPECT--
echo hello 0
echo hello 1
DONE
