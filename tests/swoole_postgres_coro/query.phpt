--TEST--
swoole_postgres_coro: query tests
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
Co\run(function() {
    $pg = new Swoole\Coroutine\PostgreSQL();

    Assert::false($pg->connect(''));

    Assert::false($pg->escape(''));
    Assert::false($pg->escapeLiteral(''));
    Assert::false($pg->escapeIdentifier(''));
    Assert::false($pg->query(''));
    Assert::false($pg->prepare('', ''));
    Assert::false($pg->execute('', []));
    Assert::false($pg->metaData(''));

    $conn = $pg->connect(PG_CONN);
    Assert::assert($conn);
    Assert::same((string)$pg->error, '');
    $result = $pg->escape("' or 1=1 & 2");
    Assert::same($result, "'' or 1=1 & 2");

    //

    $result = $pg->query('SELECT 11, 22');
    $arr = $pg->fetchAll($result);

    Assert::same($arr[0]['?column?'], 11);
    Assert::same($arr[0]['?column?1'], 22);

    $retval = $pg->query('DROP TABLE IF EXISTS weather');
    if (!$retval) {
        var_dump($retval, $pg->error, $pg->notices);
    }

    $retval = $pg->query('CREATE TABLE weather (
        id SERIAL primary key NOT NULL,
        city character varying(80),
        temp_lo integer,
        temp_hi integer,
        prcp real,
        date date)');

    if (!$retval) {
        var_dump($retval, $pg->error, $pg->notices);
    }

    $retval = $pg->query("INSERT INTO weather(city, temp_lo, temp_hi, prcp, date) VALUES ('San Francisco', 46, 50, 0.25, '1994-11-27') RETURNING id;");
    if (!$retval) {
        var_dump($retval, $pg->error, $pg->notices);
    }

    $rows = $pg->numRows($retval);
    Assert::same($rows, 1);

    $result = $pg->query('SELECT * FROM weather;');
    $arr = $pg->fetchAll($result);

    Assert::same($arr[0]['city'], 'San Francisco');
});
?>
--EXPECT--
