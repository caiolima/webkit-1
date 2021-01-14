//@ requireOptions("--usePrivateMethods=true")

function assert(actual, expected) {
    if (actual !== expected)
        throw Error("Expected: " + expected + " Actual: " + actual);
}

class C {
    #method() {
        return 'foo';
    }

    access() {
       return this.#method();
    }
}

let c = new C();
for (let i = 0; i < 10000; i++) {
    assert(c.access(), 'foo');
}

