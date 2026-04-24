//@ requireOptions("--useImportDefer=1")
import { shouldBe } from "./resources/assert.js"

var global = (Function("return this"))();

import "./import-defer-tla/b.js";
import defer * as c from "./import-defer-tla/c.js";

shouldBe(global.bEvaluated, 1);
shouldBe(global.cEvaluated, undefined);
shouldBe(global.dEvaluated, 1);
shouldBe(global.eEvaluated, 1);
shouldBe(global.fEvaluated, undefined);

shouldBe(c.value, 2);

shouldBe(global.bEvaluated, 1);
shouldBe(global.cEvaluated, 1);
shouldBe(global.dEvaluated, 1);
shouldBe(global.eEvaluated, 1);
shouldBe(global.fEvaluated, 1);
