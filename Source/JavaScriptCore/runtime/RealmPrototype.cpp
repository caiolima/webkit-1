#include "config.h"
#include "RealmPrototype.h"

#include "Completion.h"
#include "IntegrityInlines.h"
#include "JSArray.h"
#include "JSCBuiltins.h"
#include "JSCJSValue.h"
#include "JSGlobalObject.h"
#include "JSStringInlines.h"
#include "RegExpObject.h"
#include "RegExpObjectInlines.h"
#include "StringRecursionChecker.h"
#include "YarrFlags.h"
#include <wtf/text/StringBuilder.h>

namespace JSC {

static JSC_DECLARE_HOST_FUNCTION(realmProtoFuncEvaluate);
static JSC_DECLARE_HOST_FUNCTION(realmProtoFuncImportValue);

const ClassInfo RealmPrototype::s_info = { "Object", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(RealmPrototype) };

RealmPrototype::RealmPrototype(VM& vm, Structure* structure)
    : JSNonFinalObject(vm, structure)
{
}

void RealmPrototype::finishCreation(VM& vm, JSGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(vm, info()));

    JSC_NATIVE_FUNCTION_WITHOUT_TRANSITION("importValue", realmProtoFuncImportValue, static_cast<unsigned>(PropertyAttribute::DontEnum), 2);
    JSC_NATIVE_FUNCTION_WITHOUT_TRANSITION("evaluate", realmProtoFuncEvaluate, static_cast<unsigned>(PropertyAttribute::DontEnum), 1);
}

// ------------------------------ Functions ---------------------------

JSC_DEFINE_HOST_FUNCTION(realmProtoFuncEvaluate, (JSGlobalObject* globalObject, CallFrame* callFrame))
{
    VM& vm = globalObject->vm();
    auto scope = DECLARE_THROW_SCOPE(vm);

    if (callFrame->argumentCount() != 1) {
        throwTypeError(globalObject, scope, "Expected to be called with one argument"_s);
        RELEASE_AND_RETURN(scope, JSValue::encode(jsUndefined()));
    }

    JSValue thisValue = callFrame->thisValue();
    Realm* thisRealm = jsDynamicCast<Realm*>(vm, thisValue);
    if (UNLIKELY(!thisRealm))
        return throwVMTypeError(globalObject, scope, "'this' should be a Realm");

    JSValue sourceText = callFrame->uncheckedArgument(0);
    String sourceCode = callFrame->argument(0).toWTFString(globalObject);
    RETURN_IF_EXCEPTION(scope, encodedJSValue());

    JSGlobalObject* realmGlobalObject = thisRealm->globalObject();
    NakedPtr<Exception> evaluationException;
    SourceCode source = makeSource(sourceCode, callFrame->callerSourceOrigin(vm));
    JSValue result = JSC::evaluate(realmGlobalObject, source, realmGlobalObject, evaluationException);
    if (evaluationException) {
        if (vm.isTerminationException(evaluationException.get()))
            vm.setExecutionForbidden();

        return JSValue::encode(throwException(globalObject, scope, evaluationException));
    }

    if (result.isCallable(vm)) {
        WTF::dataLogLn("evaluate called ", sourceText, " callable result: ", result);
        RELEASE_AND_RETURN(scope, JSValue::encode(jsUndefined()));
    } else if (result.isPrimitive()) {
        WTF::dataLogLn("evaluate called ", sourceText, " primitive result: ", result);
        RELEASE_AND_RETURN(scope, JSValue::encode(result));
    }

    throwTypeError(globalObject, scope, "result of 'evaluate' was a non-function/non-primitive"_s);
    RELEASE_AND_RETURN(scope, JSValue::encode(jsUndefined()));
}

JSC_DEFINE_HOST_FUNCTION(realmProtoFuncImportValue, (JSGlobalObject* globalObject, CallFrame* callFrame))
{
    VM& vm = globalObject->vm();
    auto scope = DECLARE_THROW_SCOPE(vm);

    if (callFrame->argumentCount() != 2) {
        throwTypeError(globalObject, scope, "Expected to be called with two arguments"_s);
        RELEASE_AND_RETURN(scope, JSValue::encode(jsUndefined()));
    }

    JSValue thisValue = callFrame->thisValue().toThis(globalObject, ECMAMode::strict());

    if (!thisValue.isObject()) {
        throwTypeError(globalObject, scope, "'this' should be a Realm"_s);
        RELEASE_AND_RETURN(scope, JSValue::encode(jsUndefined()));
    }

    JSValue specifier = callFrame->uncheckedArgument(0);
    JSValue bindingName = callFrame->uncheckedArgument(1);
    WTF::dataLogLn("importValue called ", specifier, " ", bindingName);
    RELEASE_AND_RETURN(scope, JSValue::encode(jsUndefined()));
}

} // namespace JSC
