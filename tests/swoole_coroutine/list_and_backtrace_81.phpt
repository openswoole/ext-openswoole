--TEST--
swoole_coroutine: getBackTrace form listCoroutines
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID < 80100) die("Skipped: php version >= 8.1"); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
go(function () {
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
swoole_event_wait();
?>
--EXPECTF--
int(1)
array(2) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(2) "go"
    ["args"]=>
    array(1) {
      [0]=>
      object(Closure)#2 (0) {
      }
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(9) "{closure}"
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
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(2) "go"
    ["args"]=>
    array(1) {
      [0]=>
      object(Closure)#4 (0) {
      }
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(9) "{closure}"
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
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(2) "go"
    ["args"]=>
    array(1) {
      [0]=>
      object(Closure)#6 (1) {
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
    string(9) "{closure}"
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
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(12) "getBackTrace"
    ["class"]=>
    string(16) "Swoole\Coroutine"
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
    string(9) "{closure}"
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
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(2) "go"
    ["args"]=>
    array(1) {
      [0]=>
      object(Closure)#10 (1) {
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
    string(9) "{closure}"
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
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(6) "usleep"
    ["class"]=>
    string(16) "Swoole\Coroutine"
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
    string(9) "{closure}"
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
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(8) "readFile"
    ["class"]=>
    string(16) "Swoole\Coroutine"
    ["type"]=>
    string(2) "::"
    ["args"]=>
    array(1) {
      [0]=>
      string(%d) "%s"
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(9) "{closure}"
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
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(11) "getaddrinfo"
    ["class"]=>
    string(16) "Swoole\Coroutine"
    ["type"]=>
    string(2) "::"
    ["args"]=>
    array(1) {
      [0]=>
      string(%d) "%s"
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(9) "{closure}"
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
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(6) "resume"
    ["class"]=>
    string(16) "Swoole\Coroutine"
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
    string(9) "{closure}"
    ["args"]=>
    array(0) {
    }
  }
}
