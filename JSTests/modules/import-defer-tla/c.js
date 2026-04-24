// c.js

import "./d.js";
import "./f.js";

var global = (Function("return this"))();
global.cEvaluated = (global.cEvaluated || 0) + 1;

export let value = 2;
