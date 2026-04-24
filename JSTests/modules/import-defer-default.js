import { shouldBe } from "./resources/assert.js"

import defer from "./import-defer-default/foo.js";

shouldBe(defer, "foo");
