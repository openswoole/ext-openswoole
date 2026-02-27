--TEST--
openswoole_postgres_coro: issue227
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
co::run(function() {
    $pg = new OpenSwoole\Coroutine\PostgreSQL();

    $conn = $pg->connect(PG_CONN);

    $retval = $pg->query('DROP TABLE IF EXISTS weather');

    $retval = $pg->query('CREATE TABLE weather (
        id SERIAL primary key NOT NULL,
        city character varying(80),
        temp_lo integer,
        temp_hi integer,
        prcp real,
        date date)');

    $retval = $pg->query("INSERT INTO weather(city, temp_lo, temp_hi, prcp, date) VALUES ('San Francisco', 46, 50, 0.25, '1994-11-27') RETURNING id;");
    $retval = $pg->query("INSERT INTO weather(city, temp_lo, temp_hi, prcp, date) VALUES ('New York', 35, 45, 0.5, '1994-11-27') RETURNING id;");

    go(function() use ($pg) {
      $selectOne = $pg->prepare('SELECT * FROM weather');
      $result1 = $selectOne->execute([]);
      $row1 = $selectOne->fetchAssoc();
      var_dump($row1);
      co::sleep(2);
      $row2 = $selectOne->fetchAssoc();
      var_dump($row2);
    });

    go(function() use ($pg) {
      co::sleep(1);
      $selectSecond = $pg->query('SELECT 1');
      $row3 = $selectSecond->fetchAssoc();
      var_dump($row3);
    });
});
?>
--EXPECT--
array(6) {
  ["id"]=>
  int(1)
  ["city"]=>
  string(13) "San Francisco"
  ["temp_lo"]=>
  int(46)
  ["temp_hi"]=>
  int(50)
  ["prcp"]=>
  float(0.25)
  ["date"]=>
  string(10) "1994-11-27"
}
array(1) {
  ["?column?"]=>
  int(1)
}
array(6) {
  ["id"]=>
  int(2)
  ["city"]=>
  string(8) "New York"
  ["temp_lo"]=>
  int(35)
  ["temp_hi"]=>
  int(45)
  ["prcp"]=>
  float(0.5)
  ["date"]=>
  string(10) "1994-11-27"
}
