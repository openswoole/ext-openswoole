#!/bin/sh
__CURRENT__=`pwd`
__DIR__=$(cd "$(dirname "$0")";pwd)

export DOCKER_COMPOSE_VERSION="5.0.2"

export PHP_VERSION=${PHP_VERSION:-${1:-"8.2"}}
export CI_BRANCH=${CI_BRANCH:-${2:-"master"}}

[ -z "${CI_BRANCH}" ] && export CI_BRANCH="master"
[ -z "${CI_BUILD_DIR}" ] && export CI_BUILD_DIR=$(cd "$(dirname "$0")";cd ../;pwd)
if [ "${CI_BRANCH}" = "alpine" ]; then
    export PHP_VERSION="${PHP_VERSION}-alpine"
fi

# Use official PHP image
export DOCKER_IMAGE="php:${PHP_VERSION}"

echo "\n🗻 With PHP version ${PHP_VERSION} on ${CI_BRANCH} branch"

check_docker_dependency(){
    if [ "`docker -v 2>&1 | grep "version"`"x = ""x ]; then
        echo "\n❌ Docker not found!"
        exit 1
    elif [ "`docker ps 2>&1 | grep Cannot`"x != ""x ]; then
        echo "\n❌ Docker is not running!"
        exit 1
    else
        docker compose version > /dev/null 2>&1
        if [ $? -ne 0 ]; then
            echo "\n❌ Docker Compose plugin not found! Please install Docker Desktop or the Compose plugin."
            exit 1
        fi
    fi
}

start_docker_containers(){
    remove_docker_containers
    cd ${__DIR__} && \
    docker compose up -d && \
    docker ps -a
    if [ $? -ne 0 ]; then
        echo "\n❌ Create containers failed!"
        exit 1
    fi
}

remove_docker_containers(){
    cd ${__DIR__} && \
    docker compose kill > /dev/null 2>&1 && \
    docker compose rm -f > /dev/null 2>&1
}

run_tests_in_docker(){
    docker exec openswoole touch /.travisenv && \
    docker exec openswoole /ext-openswoole/ci/docker-route.sh
    if [ $? -ne 0 ]; then
        echo "\n❌ Run tests failed!"
        exit 1
    fi
}

remove_tests_resources(){
    remove_docker_containers
}

check_docker_dependency

echo "📦 Start docker containers...\n"
start_docker_containers # && trap "remove_tests_resources"

echo "\n⏳ Run tests in docker...\n"
run_tests_in_docker

echo "\n🚀🚀🚀Completed successfully🚀🚀🚀\n"
