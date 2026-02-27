<?php
require_once __DIR__ . "/http_server.php";


/*
class openswoole_http_server extends openswoole_server
{
    public openswoole_function on($name, $cb) {} // 与 tcp openswoole_server 的on接受的eventname 不同
}
class openswoole_http_response
{
    public openswoole_function cookie() {}
    public openswoole_function rawcookie() {}
    public openswoole_function status() {}
    public openswoole_function gzip() {}
    public openswoole_function header() {}
    public openswoole_function write() {}
    public openswoole_function end() {}
    public openswoole_function sendfile() {}
}
class openswoole_http_request
{
public openswoole_function rawcontent() {}
}
 */

$host = isset($argv[1]) ? $argv[1] : HTTP_SERVER_HOST;
$port = isset($argv[2]) ? $argv[2] : HTTP_SERVER_PORT;

(new HttpServer($host, $port, true))->start();