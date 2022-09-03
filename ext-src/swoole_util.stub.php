<?php
/*
 +----------------------------------------------------------------------+
 | Open Swoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2021-now Open Swoole Group                             |
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
namespace Swoole {
	final class Util {
		public static function getVersion(): string {}
        public static function getCPUNum(): int {}
        public static function getLocalIp(): array {}
        public static function getLocalMac(): array {}
        public static function getLastErrorCode(): int {}
        public static function getErrorMessage(int $errorCode, ?int $errorType): string {}
        public static function errorCode(): int {}
        public static function clearError(): void {}
        public static function log(int $level, string $message): void {}
        public static function hashcode(string $content, int $type): int|bool {}
        public static function mimeTypeAdd(string $suffix, string $mimeType): bool {}
        public static function mimeTypeSet(string $suffix, string $mimeType): bool {}
        public static function mimeTypeDel(string $suffix): bool {}
        public static function mimeTypeGet(string $filename): string {}
        public static function mimeTypeList(): array {}
        public static function mimeTypeExists(string $filename): string {}
        public static function setProcessName(string $name): void {}
        public static function setAio(array $settings): bool {}
	}
}