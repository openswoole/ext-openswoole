--TEST--
swoole_coroutine: $this private access in PHP70 (EG(scope))
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

class Bar
{
    static private $s_private = 's_private';
    static protected $s_protect = 's_protect';
    static public $s_public = 's_public';

    private $private = 'private';
    protected $protect = 'protect';
    public $public = 'public';

    public function foo()
    {
        go(function () {
            var_dump(self::$s_private);
            var_dump(self::$s_protect);
            var_dump(self::$s_public);
            var_dump($this->private);
            var_dump($this->protect);
            var_dump($this->public);
            co::usleep(1000);
            var_dump(self::$s_private);
            var_dump(self::$s_protect);
            var_dump(self::$s_public);
            var_dump($this->private);
            var_dump($this->protect);
            var_dump($this->public);
        });
        var_dump(self::$s_private);
        var_dump(self::$s_protect);
        var_dump(self::$s_public);
        var_dump($this->private);
        var_dump($this->protect);
        var_dump($this->public);
        $cid = go(function () {
            var_dump(self::$s_private);
            var_dump(self::$s_protect);
            var_dump(self::$s_public);
            var_dump($this->private);
            var_dump($this->protect);
            var_dump($this->public);
            Co::yield();
            var_dump(self::$s_private);
            var_dump(self::$s_protect);
            var_dump(self::$s_public);
            var_dump($this->private);
            var_dump($this->protect);
            var_dump($this->public);
        });
        go(function () use ($cid) {
            var_dump(self::$s_private);
            var_dump(self::$s_protect);
            var_dump(self::$s_public);
            var_dump($this->private);
            var_dump($this->protect);
            var_dump($this->public);
            Co::resume($cid);
            var_dump(self::$s_private);
            var_dump(self::$s_protect);
            var_dump(self::$s_public);
            var_dump($this->private);
            var_dump($this->protect);
            var_dump($this->public);
        });
        go(function () {
            var_dump(self::$s_private);
            var_dump(self::$s_protect);
            var_dump(self::$s_public);
            var_dump($this->private);
            var_dump($this->protect);
            var_dump($this->public);
            Co::usleep(1000);
            var_dump(self::$s_private);
            var_dump(self::$s_protect);
            var_dump(self::$s_public);
            var_dump($this->private);
            var_dump($this->protect);
            var_dump($this->public);
        });
        var_dump(self::$s_private);
        var_dump(self::$s_protect);
        var_dump(self::$s_public);
        var_dump($this->private);
        var_dump($this->protect);
        var_dump($this->public);
        OpenSwoole\Event::wait();
    }
}

(new Bar)->foo();

?>
--EXPECT--
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
string(9) "s_private"
string(9) "s_protect"
string(8) "s_public"
string(7) "private"
string(7) "protect"
string(6) "public"
