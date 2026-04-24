// a.js

import defer * as ns from "./b.js";

var global = (Function("return this"))();
if (global.count !== 2)
  throw new Error(`bad value ${global.count}`);
global.count = 3;

ns.b;

if (global.count !== 4)
  throw new Error(`bad value ${global.count}`);
global.count = 5;
