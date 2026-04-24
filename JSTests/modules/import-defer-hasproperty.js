//@ requireOptions("--useImportDefer=1")
import { shouldBe } from "./resources/assert.js"

var global = (Function("return this"))();
global.count = 0;

import defer * as aNs from "./import-defer-basic/1.js";

shouldBe(global.count, 0);

"foo" in aNs;

shouldBe(global.count, 1);
