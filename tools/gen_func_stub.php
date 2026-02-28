#!/usr/bin/env php
<?php
/*
  +----------------------------------------------------------------------+
  | OpenSwoole                                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | hello@swoole.co.uk so we can mail you a copy immediately.            |
  +----------------------------------------------------------------------+
 */

define('T', '    ');
define('N', PHP_EOL);

$functions = array();
$classes = array('Swoole\Event');
$constant_prefix = 'X_';

$php = '<?php' . N;
$php .= '/**' . N . ' * This file is generated from OpenSwoole, do not modify.' . N . ' */';

foreach (get_defined_constants() as $cname => $cvalue) {
    if (strpos($cname, $constant_prefix) === 0) {
        $php .= 'define(\'' . $cname . '\', ' . $cvalue . ');' . N;
    }
}

$php .= N;

foreach ($functions as $function) {
    $refl = new ReflectionFunction($function);
    $php .= 'function ' . $refl->getName() . '(';
    foreach ($refl->getParameters() as $i => $parameter) {
        if ($i >= 1) {
            $php .= ', ';
        }
        if($parameter->hasType()) {
            $php .= $parameter->getType() . ' ';
        }
        $php .= '$' . $parameter->getName();
        if ($parameter->isDefaultValueAvailable()) {
            $php .= ' = ' . $parameter->getDefaultValue();
        }
    }
    $php .= ')';
    if($refl->getReturnType()) {
        $php.= ': '. $refl->getReturnType();
    }
    $php .= ' {}' . N;
}

$php .= N;

foreach ($classes as $class) {
    $refl = new ReflectionClass($class);
    $php .= 'class ' . $refl->getName();
    if ($parent = $refl->getParentClass()) {
        $php .= ' extends ' . $parent->getName();
    }
    $php .= N . '{' . N;
    foreach ($refl->getProperties() as $property) {
        $php .= T . '$' . $property->getName() . ';' . N;
    }
    foreach ($refl->getMethods() as $method) {
        if ($method->isPublic()) {
            if ($method->getDocComment()) {
                $php .= T . $method->getDocComment() . N;                
            }
            $php .= T . 'public function ';
            if ($method->returnsReference()) {
                $php .= '&';
            }
            $php .= $method->getName() . '(';
            foreach ($method->getParameters() as $i => $parameter) {
                if ($i >= 1) {
                    $php .= ', ';
                }
                if($parameter->hasType()) {
                    $php .= $parameter->getType() . ' ';
                }
                $php .= '$' . $parameter->getName();
                if ($parameter->isDefaultValueAvailable()) {
                    $php .= ' = ' . $parameter->getDefaultValue();
                }
            }
            $php .= ')';
            if($method->getReturnType()) {
                $php.= ': '. $method->getReturnType();
            }
            $php .= ' {}' . N;
        }
    }
    $php .= '}';
}

echo $php . N;
