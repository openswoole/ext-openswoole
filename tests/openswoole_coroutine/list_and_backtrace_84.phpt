--TEST--
openswoole_coroutine: getBackTrace form listCoroutines
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID < 80400) die("Skipped"); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
co::run(function () {
    go(function () {
        go(function () {
            $main = go(function () {
                Co::yield();
                // ......
                $list = Co::list();
                $list->asort();
                foreach ($list as $cid) {
                    var_dump($cid);
                    var_dump(Co::getBackTrace($cid));
                }
            });
            go(function () use ($main) {
                go(function () {
                    Co::usleep(1000);
                });
                go(function () {
                    Co::readFile(__FILE__);
                });
                go(function () {
                    Co::getaddrinfo('localhost');
                });
                go(function () use ($main) {
                    Co::resume($main);
                });
            });
        });
    });
});
?>
--EXPECTF--
int(1)
array(2) {
  [0]=>
  array(4) {
    ["file"]=>
    string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
    ["line"]=>
    int(4)
    ["function"]=>
    string(2) "go"
    ["args"]=>
    array(1) {
      [0]=>
      object(Closure)#4 (3) {
        ["name"]=>
        string(92) "{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}"
        ["file"]=>
        string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
        ["line"]=>
        int(4)
      }
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(80) "{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}"
    ["args"]=>
    array(0) {
    }
  }
}
int(2)
array(2) {
  [0]=>
  array(4) {
    ["file"]=>
    string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
    ["line"]=>
    int(5)
    ["function"]=>
    string(2) "go"
    ["args"]=>
    array(1) {
      [0]=>
      object(Closure)#5 (3) {
        ["name"]=>
        string(104) "{closure:{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}:5}"
        ["file"]=>
        string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
        ["line"]=>
        int(5)
      }
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(92) "{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}"
    ["args"]=>
    array(0) {
    }
  }
}
int(3)
array(2) {
  [0]=>
  array(4) {
    ["file"]=>
    string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
    ["line"]=>
    int(16)
    ["function"]=>
    string(2) "go"
    ["args"]=>
    array(1) {
      [0]=>
      object(Closure)#7 (4) {
        ["name"]=>
        string(117) "{closure:{closure:{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}:5}:16}"
        ["file"]=>
        string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
        ["line"]=>
        int(16)
        ["static"]=>
        array(1) {
          ["main"]=>
          int(4)
        }
      }
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(104) "{closure:{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}:5}"
    ["args"]=>
    array(0) {
    }
  }
}
int(4)
array(2) {
  [0]=>
  array(6) {
    ["file"]=>
    string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
    ["line"]=>
    int(13)
    ["function"]=>
    string(12) "getBackTrace"
    ["class"]=>
    string(20) "OpenSwoole\Coroutine"
    ["type"]=>
    string(2) "::"
    ["args"]=>
    array(1) {
      [0]=>
      int(4)
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(116) "{closure:{closure:{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}:5}:6}"
    ["args"]=>
    array(0) {
    }
  }
}
int(5)
array(2) {
  [0]=>
  array(4) {
    ["file"]=>
    string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
    ["line"]=>
    int(26)
    ["function"]=>
    string(2) "go"
    ["args"]=>
    array(1) {
      [0]=>
      object(Closure)#11 (4) {
        ["name"]=>
        string(130) "{closure:{closure:{closure:{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}:5}:16}:26}"
        ["file"]=>
        string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
        ["line"]=>
        int(26)
        ["static"]=>
        array(1) {
          ["main"]=>
          int(4)
        }
      }
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(117) "{closure:{closure:{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}:5}:16}"
    ["args"]=>
    array(0) {
    }
  }
}
int(6)
array(2) {
  [0]=>
  array(6) {
    ["file"]=>
    string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
    ["line"]=>
    int(18)
    ["function"]=>
    string(6) "usleep"
    ["class"]=>
    string(20) "OpenSwoole\Coroutine"
    ["type"]=>
    string(2) "::"
    ["args"]=>
    array(1) {
      [0]=>
      int(1000)
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(130) "{closure:{closure:{closure:{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}:5}:16}:17}"
    ["args"]=>
    array(0) {
    }
  }
}
int(7)
array(2) {
  [0]=>
  array(6) {
    ["file"]=>
    string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
    ["line"]=>
    int(21)
    ["function"]=>
    string(8) "readFile"
    ["class"]=>
    string(20) "OpenSwoole\Coroutine"
    ["type"]=>
    string(2) "::"
    ["args"]=>
    array(1) {
      [0]=>
      string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(130) "{closure:{closure:{closure:{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}:5}:16}:20}"
    ["args"]=>
    array(0) {
    }
  }
}
int(8)
array(2) {
  [0]=>
  array(6) {
    ["file"]=>
    string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
    ["line"]=>
    int(24)
    ["function"]=>
    string(11) "getaddrinfo"
    ["class"]=>
    string(20) "OpenSwoole\Coroutine"
    ["type"]=>
    string(2) "::"
    ["args"]=>
    array(1) {
      [0]=>
      string(9) "localhost"
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(130) "{closure:{closure:{closure:{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}:5}:16}:23}"
    ["args"]=>
    array(0) {
    }
  }
}
int(9)
array(2) {
  [0]=>
  array(6) {
    ["file"]=>
    string(68) "/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php"
    ["line"]=>
    int(27)
    ["function"]=>
    string(6) "resume"
    ["class"]=>
    string(20) "OpenSwoole\Coroutine"
    ["type"]=>
    string(2) "::"
    ["args"]=>
    array(1) {
      [0]=>
      int(4)
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(130) "{closure:{closure:{closure:{closure:{closure:/ext-openswoole/tests/openswoole_coroutine/list_and_backtrace_84.php:3}:4}:5}:16}:26}"
    ["args"]=>
    array(0) {
    }
  }
}
