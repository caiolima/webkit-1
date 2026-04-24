// c.js

import "./d.js";
import "./e.js";

throw new Error(`deferred module should never be executed`);
