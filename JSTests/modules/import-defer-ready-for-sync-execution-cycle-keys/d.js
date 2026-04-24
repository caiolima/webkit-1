import { shouldThrow } from "../resources/assert.js";

import defer * as ns from "./a.js";

shouldThrow(() => { Object.getOwnPropertyNames(ns); }, "TypeError: Unable to evaluate deferred module import");

await 3;
