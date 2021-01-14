//@ requireOptions("--usePrivateMethods=true")

function assert(actual, expected) {
    if (actual !== expected)
        throw Error("Expected: " + expected + " Actual: " + actual);
}

function classExpr() {
    return class {
        #method() {
            return 'foo';
        }

        access() {
           return this.#method();
        }
    }
}

let C1 = classExpr();
let C2 = classExpr();

let c1 = new C1();
let c2 = new C2();

let arr = [c1, c2];
for (let i = 0; i < 20; i++) {
    assert(arr[i % arr.length].access(), 'foo');
}

