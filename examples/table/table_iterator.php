<?php

$table = new Swoole\Table(1024);
$table->column('name', Swoole\Table::TYPE_STRING, 64);
$table->column('id', Swoole\Table::TYPE_INT, 4);       //1,2,4,8
$table->column('num', Swoole\Table::TYPE_FLOAT);
$table->create();

$table->set('a', array('id' => 1, 'name' => 'swoole-co-uk', 'num' => 3.1415));
$table->set('b', array('id' => 2, 'name' => "swoole-uk", 'num' => 3.1415));
$table->set('hello@swoole.co.uk', array('id' => 3, 'name' => 'swoole', 'num' => 3.1415));

var_dump($table->get('a'));
var_dump($table->get('b', 'name'));

foreach($table as $row) {
    var_dump($row);
}
echo count($table);