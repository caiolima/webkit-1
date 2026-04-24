//@ requireOptions("--useImportDefer=1")
import { shouldBe, shouldThrow } from "./resources/assert.js"

var global = (Function("return this"))();
global.count = 0;

import defer * as aNs from "./import-defer-then/export-then.js";

shouldBe(global.count, 0);

shouldBe(aNs.then, undefined);
shouldBe(Object.getOwnPropertyDescriptor(aNs, "then"), undefined);
shouldThrow(() => Object.defineProperty(aNs, "then", { }), "TypeError: Attempting to define property on object that is not extensible.");
shouldThrow(() => aNs.then = "foo", "TypeError: Attempted to assign to readonly property.");
shouldBe(delete aNs.then, true);
shouldBe(Object.keys(aNs).length, 0);
shouldBe("then" in aNs, false);

shouldBe(global.count, 0);
