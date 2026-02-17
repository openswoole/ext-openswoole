--TEST--
swoole_coroutine_fiber: coroutine context with fiber context
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['use_fiber_context' => true]);
co::run(function () {
    $context = Co::getContext();
    Assert::isInstanceOf($context, Swoole\Coroutine\Context::class);
    $context['key1'] = 'value1';

    go(function () {
        $context = Co::getContext();
        Assert::isInstanceOf($context, Swoole\Coroutine\Context::class);
        $context['key2'] = 'value2';

        // child context is independent from parent
        Assert::false(isset($context['key1']));
        Assert::same($context['key2'], 'value2');
        echo "child context OK\n";
    });

    Assert::same($context['key1'], 'value1');
    Assert::false(isset($context['key2']));
    echo "parent context OK\n";
});
?>
--EXPECT--
child context OK
parent context OK
