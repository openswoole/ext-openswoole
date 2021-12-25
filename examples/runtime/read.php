<?php
Swoole\Runtime::enableCoroutine();

Co\run(function () {
	$fp = fopen(__DIR__.'/data.txt', 'r+');
	echo "len=".strlen(fread($fp, 8291))."\n";
});
