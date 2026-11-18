--TEST--
openswoole_runtime: mysqli_init() and real_connect() in a coroutine
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
if (!extension_loaded('mysqli')) {
    skip('no mysqli');
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

co::run(function () {
    for ($i = 0; $i < 3; $i++) {
        // without arguments, mysqli opens its connection as a persistent stream
        $db = new mysqli();
        $db->real_connect(MYSQL_SERVER_HOST, MYSQL_SERVER_USER, MYSQL_SERVER_PWD, MYSQL_SERVER_DB, MYSQL_SERVER_PORT);
        Assert::same($db->query('SELECT 1')->fetch_row()[0], '1');
        $db->close();
    }
    // the server drops the connection, then the object is released
    $db = new mysqli();
    $db->real_connect(MYSQL_SERVER_HOST, MYSQL_SERVER_USER, MYSQL_SERVER_PWD, MYSQL_SERVER_DB, MYSQL_SERVER_PORT);
    $id = $db->thread_id;
    $killer = new mysqli(MYSQL_SERVER_HOST, MYSQL_SERVER_USER, MYSQL_SERVER_PWD, MYSQL_SERVER_DB, MYSQL_SERVER_PORT);
    $killer->query("KILL {$id}");
    $killer->close();
    try {
        $db->query('SELECT 1');
    } catch (mysqli_sql_exception $e) {
        echo "query after kill failed\n";
    }
    unset($db);
    echo "done\n";
});
?>
--EXPECT--
query after kill failed
done
