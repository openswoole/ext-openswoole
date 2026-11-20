--TEST--
openswoole_postgres_coro: fetch methods select the given row
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
co::run(function () {
    $pg = new OpenSwoole\Coroutine\PostgreSQL();
    Assert::true($pg->connect(PG_CONN));

    $stmt = $pg->query('SELECT n, n * 10 AS t FROM generate_series(0, 2) AS n ORDER BY n');
    Assert::eq($stmt->fetchRow(2), ['2', '20']);
    Assert::eq($stmt->fetchAssoc(1), ['n' => '1', 't' => '10']);
    Assert::eq($stmt->fetchArray(0, OPENSWOOLE_PGSQL_ASSOC), ['n' => '0', 't' => '0']);
    Assert::eq($stmt->fetchObject(2)->t, '20');

    // a given row also moves the internal cursor to it, as pg_fetch_row() does
    Assert::eq($stmt->fetchRow(0), ['0', '0']);
    Assert::eq($stmt->fetchRow(), ['0', '0']);
    Assert::eq($stmt->fetchRow(), ['1', '10']);
    Assert::eq($stmt->fetchAssoc(null), ['n' => '2', 't' => '20']);
    Assert::false($stmt->fetchRow());
});
echo "DONE\n";
?>
--EXPECT--
DONE
