<?php
namespace OpenSwoole\Coroutine {
    class PostgreSQL {
        public function __construct() {}
        public function __destruct() {}
        public function connect(string $conninfo, float $timeout = 2): bool {}
        public function escape(string $string): false|string {}
        public function escapeLiteral(string $string): false|string {}
        public function escapeIdentifier(string $string): false|string {}
        public function query(string $query): false|PostgreSQLStatement {}
        public function prepare(string $query): false|PostgreSQLStatement {}
        public function metaData(string $table_name): false|array {}
        public function createLOB(): int|false {}
        /** @return resource|false */
        public function openLOB(int $oid, string $mode = "rb") {}
        public function unlinkLOB(int $oid): bool {}
        public function status(): int|false {}
        public function reset(float $timeout = 0): bool {}
    }

    class PostgreSQLStatement {
        public function execute(array $params = []): bool {}
        public function fetchAll(int $result_type = OPENSWOOLE_PGSQL_ASSOC): false|array {}
        public function affectedRows(): false|int {}
        public function numRows(): false|int {}
        public function fieldCount(): false|int {}
        public function fetchObject(?int $row = null, ?string $class_name = null, array $ctor_params = []): false|object {}
        public function fetchAssoc(?int $row = null): false|array {}
        public function fetchArray(?int $row = null, int $result_type = OPENSWOOLE_PGSQL_BOTH): false|array {}
        public function fetchRow(?int $row = null, int $result_type = OPENSWOOLE_PGSQL_NUM): false|array {}
    }
}
