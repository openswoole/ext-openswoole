--TEST--
swoole_postgres_coro: issue227
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
      $pg->prepare('selectOne', 'SELECT * FROM weather');
      $result1 = $pg->execute('selectOne', []);
      $row1 = $pg->fetchAssoc($result1);
      var_dump($row1);
      co::sleep(2);
      $row2 = $pg->fetchAssoc($result1);
      var_dump($row2);
    });

    go(function() use ($pg) {
      co::sleep(1);
      // Any queries with no fetch methods
      $pg->query('SELECT 1'); // AFTER this all another fetching result reset iterator to begin result rows
        
      // Also prepared queries: 
      // $pg->prepare('selectTwo', 'SELECT 2');
      // $pg->execute('selectTwo', []);
        
      // or INSERT/UPDATE request (where most like will not using fetching result)
      // $pg->prepare('update', 'UPDATE weather SET prcp = 0.3 WHERE id = 1');
      // $pg->execute('update', []); // No need fetching results
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
