// a.js

import "./b.js";
import defer * as ns from "./c.js";

var global = (Function("return this"))();
if (global.count !== 3)
  throw new Error(`bad value ${global.count}`);
global.count = 4;
