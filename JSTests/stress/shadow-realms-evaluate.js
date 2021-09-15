//@ requireOptions("--useShadowRealm=1")

function shouldBe(actual, expected) {
    if (actual !== expected)
        throw new Error(`expected ${expected} but got ${actual}`);
}

function shouldThrow(func, errorType, assertionFn) {
    let error;
    try {
        func();
    } catch (e) {
        error = e;
    }

    if (!(error instanceof errorType))
        throw new Error(`Expected ${errorType.name}!`);

    assertionFn(error);
}

{
    let realm = new ShadowRealm();
    eval("1 + 1");
    shouldBe(realm.evaluate("1"), 1);
    realm.evaluate("var x = 1");
    shouldBe(realm.evaluate("x"), 1);
}

{
    let realm = new ShadowRealm();
    globalThis.hi = 6;

    shouldBe(realm.evaluate("globalThis.hi"), undefined);

    realm.evaluate("globalThis.hi = 'fala amigo'");

    shouldBe(realm.evaluate("globalThis.hi"), "fala amigo");
    shouldBe(globalThis.hi, 6);
}

{
    let realm = new ShadowRealm();
    shouldThrow(
      () => { realm.evaluate(".."); },
      SyntaxError,
      (err) => {
          shouldBe($.globalObjectFor(err), globalThis);
          shouldBe(String(err), `SyntaxError: Unexpected token '.'`);
      });
}

{
    let realm = new ShadowRealm();
    shouldThrow(
      () => { realm.evaluate("throw new Error('secret')"); },
      TypeError,
      (err) => {
          shouldBe($.globalObjectFor(err), globalThis);
          shouldBe(String(err), "TypeError: Error encountered during evaluation");
      });
}
