// c.js

import "./e.js";

var global = (Function("return this"))();
if (global.count !== undefined)
  throw new Error(`bad value ${global.count}`);
global.count = 1;
