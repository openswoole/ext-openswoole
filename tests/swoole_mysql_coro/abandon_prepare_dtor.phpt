--TEST--
swoole_mysql_coro: mysql prepare dtor
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
go(function () {
    $mysql = new Co\MySQL;
    $mysql->connect([
        'host' => MYSQL_SERVER_HOST,
        'port' => MYSQL_SERVER_PORT,
        'user' => MYSQL_SERVER_USER,
        'password' => MYSQL_SERVER_PWD,
        'database' => MYSQL_SERVER_DB
    ]);
    for ($n = MAX_REQUESTS; $n--;) {
        $result = $mysql->query('show status like \'Prepared_stmt_count\'');
        $counterBeforeNewStmt = $result[0]['Value'];
        $statement = $mysql->prepare('SELECT ?');
        $statement = null;
        Co::usleep(1500);
        $result = $mysql->query('show status like \'Prepared_stmt_count\'');
        assert($result[0]['Value'] == 0 || $result[0]['Value'] == $counterBeforeNewStmt-1);
    }
});
Swoole\Event::wait();
echo "DONE\n";
?>
--EXPECT--
DONE
