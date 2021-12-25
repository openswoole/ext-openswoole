<?php

Co\run(function() {
	$c = new Co\Http\Client('openswoole.com', 443, true);
	$c->set(['ssl_host_name' => 'openswoole.com']);
	$c->get('/');
	var_dump($c->body, $c->headers);
});
