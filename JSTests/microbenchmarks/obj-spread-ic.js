function assert(actual, expected) {
    if (actual !== expected)
        throw new Error("Expected: " + expected + " bug got: " + actual);
}

let obj = {a: 1, b: 2};

for (i = 0; i < 100000; i++) {
    let o1 = {...obj};
    assert(o1.a, 1);
    assert(o1.b, 2);
}

