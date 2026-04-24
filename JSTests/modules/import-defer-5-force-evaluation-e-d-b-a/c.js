// c.js

import "./d.js";

var global = (Function("return this"))();
if (global.count !== 4)
  throw new Error(`bad value ${global.count}`);
global.count = 5;

await 0;

if (global.count !== 5)
  throw new Error(`bad value ${global.count}`);
global.count = 6;
