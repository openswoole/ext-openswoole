#!/bin/sh -e
__CURRENT__=`pwd`
__DIR__=$(cd "$(dirname "$0")";pwd)

[ -z "${CI_BRANCH}" ] && export CI_BRANCH="master"

#-------------PHPT-------------
cd ${__DIR__} && cd ../tests/

echo "" && echo "⭐️ Testing environment..." && echo ""
ulimit -a
nproc --all

# initialization
echo "" && echo "⭐️ Initialization for tests..." && echo ""
./init
cd ./include/lib
composer update -vvv
cd -
echo ""

# debug
for debug_file in ${__DIR__}/debug/*.php
do
    if test -f "${debug_file}";then
        debug_file_basename="`basename ${debug_file}`"
        echo "" && echo "====== RUN ${debug_file_basename} ======" && echo ""
        php "${debug_file}"
        echo "" && echo "========================================" && echo ""
    fi
done

# run tests @params($1=testrun, $2=list_file, $3=options)
run_tests(){
    export TEST_PHP_JUNIT=test-result-${1}.xml
    ./start.sh \
    "`tr '\n' ' ' < ${2} | xargs`" \
    -w ${2} \
    ${3}
}

has_failures(){
    cat tests.list
}

should_exit_with_error(){
    if [ "${CI_BRANCH}" = "valgrind" ]; then
        set +e
        find ./ -type f -name "*.mem"
        set -e
    else
        has_failures
    fi
}

touch tests.list
trap "rm -f tests.list; echo ''; echo '⌛ Done on '`date "+%Y-%m-%d %H:%M:%S"`;" EXIT

cpu_num=`nproc --all`
options="-j${cpu_num}"

echo "" && echo "🌵️️ Current branch is ${CI_BRANCH}" && echo ""
if [ "${CI_BRANCH}" = "valgrind" ]; then
    dir="base"
    options="${options} -m"
else
    dir="swoole_*"
fi
echo "${dir}" > tests.list
for i in 1 2 3 4 5 6
do
    if [ "`has_failures`" ]; then
        if [ ${i} -gt "1" ]; then
            sleep ${i}
            echo "" && echo "😮 Retry failed tests#${i}:" && echo ""
        fi
        cat tests.list
        timeout=`echo | expr ${i} \* 15 + 15`
        options="${options} --set-timeout ${timeout}"
        echo ${options}
        run_tests ${i} tests.list "${options}"
    else
        break
    fi
done
./merge-test-results >test-results.xml
if [ "`should_exit_with_error`" ]; then
    exit 255
fi
