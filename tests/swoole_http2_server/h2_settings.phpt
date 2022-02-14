--TEST--
swoole_http2_server: settings
--SKIPIF--
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$http = new swoole_http_server('127.0.0.1', 0, SWOOLE_BASE);
$http->set([
    'worker_num' => 1,
    'log_file' => '/dev/null',
    'open_http2_protocol' => true,
    'http2_header_table_size' => 4095,
    'http2_initial_window_size' => 65534,
    'http2_max_concurrent_streams' => 1281,
    'http2_max_frame_size' => 16383,
    'http2_max_header_list_size' => 4095,
]);
var_dump($http->setting);
?>
--EXPECT--
array(8) {
  ["worker_num"]=>
  int(1)
  ["log_file"]=>
  string(9) "/dev/null"
  ["open_http2_protocol"]=>
  bool(true)
  ["http2_header_table_size"]=>
  int(4095)
  ["http2_initial_window_size"]=>
  int(65534)
  ["http2_max_concurrent_streams"]=>
  int(1281)
  ["http2_max_frame_size"]=>
  int(16383)
  ["http2_max_header_list_size"]=>
  int(4095)
}