// e.js

import "./f.js";

throw new Error(`deferred module should never be executed`);
