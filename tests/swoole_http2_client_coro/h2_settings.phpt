--TEST--
swoole_http2_client_coro: settings
--SKIPIF--
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Coroutine\Http2\Client;
use Swoole\Http2\Request;

Co\run(function () {
    $domain = 'cloudflare.com';
    $c = new Client($domain, 443, true);
    $c->set([
        'http2_header_table_size' => 4095,
        'http2_initial_window_size' => 65534,
        'http2_max_concurrent_streams' => 1281,
        'http2_max_frame_size' => 16383,
        'http2_max_header_list_size' => 4095,
    ]);
    var_dump($c->stats('local_settings'));
});
?>
--EXPECT--
array(5) {
  ["header_table_size"]=>
  int(4095)
  ["window_size"]=>
  int(65534)
  ["max_concurrent_streams"]=>
  int(1281)
  ["max_frame_size"]=>
  int(16383)
  ["max_header_list_size"]=>
  int(4095)
}