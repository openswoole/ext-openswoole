# How to run tests

```shell
export PHP_VERSION=7.4
export CI_BRANCH=master
./ci/route.sh
```

### Enter the Docker testing environment

> You can cancel the unit test by `CTRL+C`

```shell
docker exec -it -e LINES=$(tput lines) -e COLUMNS=$(tput cols) openswoole /bin/bash
```
