// e.js

import defer * as ns from "./a.js";

var global = (Function("return this"))();
if (global.count !== undefined)
  throw new Error(`bad value ${global.count}`);
global.count = 1;

await 0;

if (global.count !== 1)
  throw new Error(`bad value ${global.count}`);
global.count = 2;
