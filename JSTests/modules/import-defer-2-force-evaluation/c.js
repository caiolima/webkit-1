// c.js

import "./e.js";

var global = (Function("return this"))();
if (global.count !== 3)
  throw new Error(`bad value ${global.count}`);
global.count = 4;

export const c = 1;
