// b.js

var global = (Function("return this"))();
global.bEvaluated = (global.bEvaluated || 0) + 1;
