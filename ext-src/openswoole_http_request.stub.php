<?php
/*
 +----------------------------------------------------------------------+
 | OpenSwoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2021-now OpenSwoole Group                             |
 +----------------------------------------------------------------------+
 | This source file is subject to version 2.0 of the Apache license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.apache.org/licenses/LICENSE-2.0.html                      |
 | If you did not receive a copy of the Apache2.0 license and are unable|
 | to obtain it through the world-wide-web, please send a note to       |
 | hello@swoole.co.uk so we can mail you a copy immediately.            |
 +----------------------------------------------------------------------+
*/

/** @not-serializable */
namespace OpenSwoole\Http {
	final class Request {
		public static function create(?array $options): Request|bool {}
		public function parse(string $data): int|false {}
		public function isCompleted(): bool {}
		public function getMethod(): bool|string {}
		public function rawContent(): bool|string {}
		public function getContent(): bool|string {}
		public function getData(): bool|string {}
		public function __destruct() {}

	}
}