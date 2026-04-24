// b.js

import "./c.js";

throw new Error(`deferred module should never be executed`);
