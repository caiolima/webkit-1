// a.js

import defer * as ns from "./b.js";

var global = (Function("return this"))();
if (global.count !== 5)
  throw new Error(`bad value ${global.count}`);
global.count = 6;

global.nsE.e;
ns.b;
