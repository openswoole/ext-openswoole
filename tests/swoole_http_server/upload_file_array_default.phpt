--TEST--
swoole_http_server: upload files array default format
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_function_not_exist('curl_init');
skip_if_function_not_exist('curl_file_create');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;

$pm->parentFunc = function () use ($pm) {
    $formData = [
        'file'              => curl_file_create(TEST_IMAGE, 'application/octet-stream', 'image.jpg'),
        'form[file]'        => curl_file_create(TEST_IMAGE, 'image/jpeg', 'photo.jpg'),
        'form[group][file]' => curl_file_create(TEST_IMAGE2, 'image/svg+xml', 'swoole-logo.svg'),
    ];
    
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "http://127.0.0.1:{$pm->getFreePort()}");
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_HTTPHEADER, ['Expect:']);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $formData);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    $result = curl_exec($ch);
    curl_close($ch);

    echo "$result\n";
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $http = new swoole_http_server('127.0.0.1', $pm->getFreePort());
    $http->set([
        'log_file' => '/dev/null',
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function (swoole_http_request $request, swoole_http_response $response) {
        $files = $request->files;
        if (!is_array($files)
            || empty($files['file']['tmp_name'])
            || empty($files['form']['file']['tmp_name'])
            || empty($files['form']['group']['file']['tmp_name'])
        ) {
            $response->end();
            return;
        }
        $files['file']['tmp_name']                  = '/tmp/swoole.upfile.fixture1';
        $files['form']['file']['tmp_name']          = '/tmp/swoole.upfile.fixture2';
        $files['form']['group']['file']['tmp_name'] = '/tmp/swoole.upfile.fixture3';
        $response->end(var_export($files, true));
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
array (
  'file' => 
  array (
    'name' => 'image.jpg',
    'type' => 'application/octet-stream',
    'tmp_name' => '/tmp/swoole.upfile.fixture1',
    'error' => 0,
    'size' => 15144,
  ),
  'form' => 
  array (
    'file' => 
    array (
      'name' => 'photo.jpg',
      'type' => 'image/jpeg',
      'tmp_name' => '/tmp/swoole.upfile.fixture2',
      'error' => 0,
      'size' => 15144,
    ),
    'group' => 
    array (
      'file' => 
      array (
        'name' => 'swoole-logo.svg',
        'type' => 'image/svg+xml',
        'tmp_name' => '/tmp/swoole.upfile.fixture3',
        'error' => 0,
        'size' => 4742,
      ),
    ),
  ),
)
