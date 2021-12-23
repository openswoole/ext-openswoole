Compile openswoole.so
------
`git clone` the repo then run `export SWOOLE_DIR=~/swoole-src`

```shell
cd swoole-src
phpize
./configure
cmake .
make -j
```

Example applications
-----
```shell
cd swoole-src/core-tests/samples
cmake .
make -j
./bin/core_samples
```
