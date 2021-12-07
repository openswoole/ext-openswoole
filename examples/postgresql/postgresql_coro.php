<?php

// Find more example at https://www.swoole.co.uk/docs/modules/swoole-coroutine-postgres

use Swoole\Coroutine\PostgreSQL;

Co\run(function() {

    $pg = new PostgreSQL();

    $conn = $pg->connect("host=127.0.0.1;port=5432;dbname=test;user=postgres;password=***");

    if(!$conn)
    {
        var_dump($pg->error);
        return;
    }

    $result = $pg->query('SELECT * FROM test;');

    $arr = $pg->fetchAll($result);
    var_dump($arr);
});
