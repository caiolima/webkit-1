//@ requireOptions("--useShadowRealm=1")

var abort = $vm.abort;

function shouldThrow(func, errorType) {
    let error;
    try {
        func();
    } catch (e) {
        error = e;
    }

    if (!(error instanceof errorType))
        throw new Error(`Expected ${errorType.name}!`);
}

async function shouldThrowAsync(func, errorType) {
    let error;
    try {
        await func();
    } catch (e) {
        error = e;
    }

    if (!(error instanceof errorType))
        throw new Error(`Expected ${errorType.name}!`);
}


(async function () {
    const importPath = "./resources/shadow-realm-example-module.js";
    const { shouldBe } = await import('./import-tests/should.js');
    const { answer, putInGlobal, getFromGlobal } = await import(importPath);

    {
        let realm = new ShadowRealm();
        let answerVal = await realm.importValue(importPath, "answer");
        shouldBe(answerVal, answerVal);

        await shouldThrowAsync(async () => { let x = await realm.importValue(importPath, "anObject"); }, TypeError);

        let putInGlobalFn = await realm.importValue(importPath, "putInGlobal");
        let getFromGlobalFn = await realm.importValue(importPath, "getFromGlobal");
        putInGlobalFn("salutation", "sarava");
        shouldBe(getFromGlobalFn("saluation"), "sarava");

        putInGlobal("saluation", "hello world!");
        shouldBe(getFromGlobal("saluation"), "hello world!");

        shouldBe(getFromGlobalFn("saluation"), "sarava");

        shouldThrow(() => { putInGlobalFn("treasure", new Object()); }, TypeError);

        let getAnObjectFn = await realm.importValue(importPath, "getAnObject");
        shouldThrow(() => { getAnObjectFn(); }, TypeError);

    }
}()).catch((error) => {
    print(String(error));
    abort();
});
