<?php
/**
 * A local HTTP and HTTPS server for tests that used outside sites.
 *
 * It runs in a child process and answers a small httpbin-like set of routes:
 *   /                     an HTML page (about 4KB) with "OpenSwoole test page" and a Last-Modified header
 *   /get, /anything       JSON: args, headers, url, origin, method
 *   /post, /put, /patch   JSON: args, headers, data, json, form, files
 *   /status/{code}        that status, empty body
 *   /cookies              JSON: cookies
 *   /cookies/set?a=b      sets the cookies, then JSON: cookies
 *   /basic-auth/{u}/{p}   200 with JSON when the credentials match, else 401
 *   /delay/{seconds}      waits, then like /get
 *   /bytes/{n}            n bytes of "x"
 *   /headers              JSON: headers
 *   /gzip                 like /get, compressed with gzip
 *
 * Usage:
 *   $server = SwooleTest\LocalServer::start();
 *   $server->url('/get');           // http://127.0.0.1:PORT/get
 *   $server->url('/get', true);     // https://127.0.0.1:PORT/get
 *   $server->host, $server->httpPort, $server->httpsPort
 */

namespace SwooleTest;

use OpenSwoole\Http\Request;
use OpenSwoole\Http\Response;
use OpenSwoole\Http\Server;
use OpenSwoole\Process;
use RuntimeException;

class LocalServer
{
    public $host = '127.0.0.1';
    public $httpPort;
    public $httpsPort;
    /** @var Process */
    protected $process;

    public static function start(): self
    {
        $server = new self();
        $server->httpPort = get_one_free_port();
        $server->httpsPort = get_one_free_port();
        $server->process = new Process(function () use ($server) {
            $server->serve();
        }, false, 0);
        $server->process->start();
        $server->waitUntilReady();
        register_shutdown_function(function () use ($server) {
            $server->stop();
        });
        return $server;
    }

    public function url(string $path = '/', bool $https = false): string
    {
        return ($https ? 'https' : 'http') . "://{$this->host}:" . ($https ? $this->httpsPort : $this->httpPort) . $path;
    }

    public function stop(): void
    {
        if ($this->process && $this->process->pid && Process::kill($this->process->pid, 0)) {
            // not reaped here: Process::wait() could take a child the test waits for, and pcntl
            // is disabled with the runtime hooks; it runs at shutdown, the exit reaps it
            Process::kill($this->process->pid, SIGKILL);
        }
        $this->process = null;
    }

    protected function waitUntilReady(): void
    {
        $hookFlags = \OpenSwoole\Runtime::getHookFlags();
        \OpenSwoole\Runtime::enableCoroutine(false);
        $deadline = microtime(true) + 10;
        try {
            foreach ([$this->httpPort, $this->httpsPort] as $port) {
                while (!($fp = @stream_socket_client("tcp://{$this->host}:{$port}", $errno, $errstr, 0.2))) {
                    if (microtime(true) > $deadline) {
                        throw new RuntimeException("LocalServer did not start on port {$port}");
                    }
                    usleep(10000);
                }
                fclose($fp);
            }
        } finally {
            \OpenSwoole\Runtime::enableCoroutine($hookFlags);
        }
    }

    protected function serve(): void
    {
        $http = new Server($this->host, $this->httpPort, SWOOLE_BASE);
        $http->set([
            'log_file' => '/dev/null',
            'worker_num' => 1,
            'open_http2_protocol' => true,
            'http_compression' => false,
        ]);
        $https = $http->listen($this->host, $this->httpsPort, SWOOLE_SOCK_TCP | SWOOLE_SSL);
        $https->set([
            'ssl_cert_file' => SSL_FILE_DIR . '/server.crt',
            'ssl_key_file' => SSL_FILE_DIR . '/server.key',
            'open_http_protocol' => true,
            'open_http2_protocol' => true,
        ]);
        $http->on('request', function (Request $request, Response $response) {
            static::handle($request, $response);
        });
        $http->start();
    }

    public static function handle(Request $request, Response $response): void
    {
        $path = $request->server['request_uri'] ?? '/';
        $method = $request->server['request_method'] ?? 'GET';
        $parts = array_values(array_filter(explode('/', $path), 'strlen'));

        $info = [
            'args' => $request->get ?? (object) [],
            'headers' => static::headers($request),
            'origin' => $request->server['remote_addr'] ?? '',
            'url' => 'http://' . ($request->header['host'] ?? '127.0.0.1') . $path .
                (isset($request->server['query_string']) ? '?' . $request->server['query_string'] : ''),
            'method' => $method,
        ];

        switch ($parts[0] ?? '') {
        case '':
            $response->header('Content-Type', 'text/html; charset=utf-8');
            $response->header('Last-Modified', 'Fri, 01 Jan 2021 00:00:00 GMT');
            $response->end('<html><head><title>OpenSwoole test page</title></head><body>' .
                str_repeat('<p>OpenSwoole test page</p>', 150) . '</body></html>');
            return;
        case 'get':
        case 'anything':
        case 'headers':
            if ($parts[0] === 'headers') {
                $info = ['headers' => $info['headers']];
            }
            static::json($response, $info);
            return;
        case 'post':
        case 'put':
        case 'patch':
        case 'delete':
            $body = (string) $request->getContent();
            $info['data'] = preg_match('//u', $body) ? $body : 'data:application/octet-stream;base64,' . base64_encode($body);
            $info['json'] = json_decode($body, true);
            $info['form'] = $request->post ?? (object) [];
            $files = [];
            foreach ($request->files ?? [] as $name => $file) {
                $files[$name] = isset($file['tmp_name']) ? file_get_contents($file['tmp_name']) : $file;
                // binary content is sent like httpbin does, as a base64 data URI
                if (is_string($files[$name]) && !preg_match('//u', $files[$name])) {
                    $files[$name] = 'data:application/octet-stream;base64,' . base64_encode($files[$name]);
                }
            }
            $info['files'] = $files ?: (object) [];
            static::json($response, $info);
            return;
        case 'status':
            $response->status((int) ($parts[1] ?? 200));
            $response->end();
            return;
        case 'cookies':
            $cookies = $request->cookie ?? [];
            if (($parts[1] ?? '') === 'set') {
                foreach ($request->get ?? [] as $name => $value) {
                    $response->cookie($name, $value, 0, '/');
                    $cookies[$name] = $value;
                }
            }
            static::json($response, ['cookies' => $cookies ?: (object) []]);
            return;
        case 'basic-auth':
            $expected = 'Basic ' . base64_encode(($parts[1] ?? '') . ':' . ($parts[2] ?? ''));
            if (($request->header['authorization'] ?? '') !== $expected) {
                $response->status(401);
                $response->header('WWW-Authenticate', 'Basic realm="Fake Realm"');
                $response->end();
                return;
            }
            static::json($response, ['authenticated' => true, 'user' => $parts[1] ?? '']);
            return;
        case 'delay':
            usleep((int) (((float) ($parts[1] ?? 1)) * 1000000));
            static::json($response, $info);
            return;
        case 'bytes':
            $response->end(str_repeat('x', (int) ($parts[1] ?? 0)));
            return;
        case 'gzip':
            $info['gzipped'] = true;
            $response->header('Content-Type', 'application/json');
            $response->header('Content-Encoding', 'gzip');
            $response->end(gzencode(json_encode($info)));
            return;
        default:
            $response->status(404);
            $response->end('Not Found');
        }
    }

    protected static function headers(Request $request): array
    {
        $headers = [];
        foreach ($request->header ?? [] as $name => $value) {
            $headers[implode('-', array_map('ucfirst', explode('-', $name)))] = $value;
        }
        return $headers;
    }

    protected static function json(Response $response, $data): void
    {
        $response->header('Content-Type', 'application/json');
        $response->end(json_encode($data, JSON_UNESCAPED_SLASHES));
    }
}
