import { shouldThrow } from "../resources/assert.js";

import defer * as ns from "./a.js";

shouldThrow(() => { delete ns.foo; }, "TypeError: Unable to evaluate deferred module import");

await 3;
