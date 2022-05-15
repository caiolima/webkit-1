#include "config.h"
#include "RealmWrappedFunction.h"

#include "ExecutableBaseInlines.h"
#include "JSCInlines.h"

namespace JSC {

const ClassInfo RealmWrappedFunction::s_info = { "Function", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(RealmWrappedFunction) };

JSC_DEFINE_HOST_FUNCTION(realmWrappedThisNoArgsFunctionCall, (JSGlobalObject* globalObject, CallFrame* callFrame))
{
    RealmWrappedFunction* wrappedFunction = jsCast<RealmWrappedFunction*>(callFrame->jsCallee());

    MarkedArgumentBuffer args;
    for (unsigned i = 0; i < callFrame->argumentCount(); ++i)
        args.append(callFrame->uncheckedArgument(i));
    RELEASE_ASSERT(!args.hasOverflowed());

    JSFunction* targetFunction = jsCast<JSFunction*>(wrappedFunction->targetFunction());
    ExecutableBase* executable = targetFunction->executable();
    if (executable->hasJITCodeForCall()) {
        // Force the executable to cache its arity entrypoint.
        executable->entrypointFor(CodeForCall, MustCheckArity);
    }
    auto callData = getCallData(globalObject->vm(), targetFunction);
    ASSERT(callData.type != CallData::Type::None);
    return JSValue::encode(call(globalObject, targetFunction, callData, wrappedFunction->boundThis(), args));
}

JSC_DEFINE_HOST_FUNCTION(realmWrappedFunctionCall, (JSGlobalObject* globalObject, CallFrame* callFrame))
{
    VM& vm = globalObject->vm();
    auto scope = DECLARE_THROW_SCOPE(vm);
    RealmWrappedFunction* wrappedFunction = jsCast<RealmWrappedFunction*>(callFrame->jsCallee());

    MarkedArgumentBuffer args;
    for (unsigned i = 0; i < callFrame->argumentCount(); ++i)
        args.append(callFrame->uncheckedArgument(i));
    if (UNLIKELY(args.hasOverflowed())) {
        throwOutOfMemoryError(globalObject, scope);
        return encodedJSValue();
    }

    JSObject* targetFunction = wrappedFunction->targetFunction();
    auto callData = getCallData(vm, targetFunction);
    ASSERT(callData.type != CallData::Type::None);
    RELEASE_AND_RETURN(scope, JSValue::encode(call(globalObject, targetFunction, callData, wrappedFunction->boundThis(), args)));
}

JSC_DEFINE_HOST_FUNCTION(realmWrappedThisNoArgsFunctionConstruct, (JSGlobalObject* globalObject, CallFrame* callFrame))
{
    RealmWrappedFunction* wrappedFunction = jsCast<RealmWrappedFunction*>(callFrame->jsCallee());

    MarkedArgumentBuffer args;
    for (unsigned i = 0; i < callFrame->argumentCount(); ++i)
        args.append(callFrame->uncheckedArgument(i));
    RELEASE_ASSERT(!args.hasOverflowed());

    JSFunction* targetFunction = jsCast<JSFunction*>(wrappedFunction->targetFunction());
    auto constructData = getConstructData(globalObject->vm(), targetFunction);
    ASSERT(constructData.type != CallData::Type::None);

    JSValue newTarget = callFrame->newTarget();
    if (newTarget == wrappedFunction)
        newTarget = targetFunction;
    return JSValue::encode(construct(globalObject, targetFunction, constructData, args, newTarget));
}

JSC_DEFINE_HOST_FUNCTION(realmWrappedFunctionConstruct, (JSGlobalObject* globalObject, CallFrame* callFrame))
{
    VM& vm = globalObject->vm();
    auto scope = DECLARE_THROW_SCOPE(vm);
    RealmWrappedFunction* wrappedFunction = jsCast<RealmWrappedFunction*>(callFrame->jsCallee());

    MarkedArgumentBuffer args;
    for (unsigned i = 0; i < callFrame->argumentCount(); ++i)
        args.append(callFrame->uncheckedArgument(i));
    if (UNLIKELY(args.hasOverflowed())) {
        throwOutOfMemoryError(globalObject, scope);
        return encodedJSValue();
    }

    JSObject* targetFunction = wrappedFunction->targetFunction();
    auto constructData = getConstructData(vm, targetFunction);
    ASSERT(constructData.type != CallData::Type::None);

    JSValue newTarget = callFrame->newTarget();
    if (newTarget == wrappedFunction)
        newTarget = targetFunction;
    RELEASE_AND_RETURN(scope, JSValue::encode(construct(globalObject, targetFunction, constructData, args, newTarget)));
}

inline Structure* getRealmWrappedFunctionStructure(VM& vm, JSGlobalObject* globalObject, JSObject* targetFunction)
{
    auto scope = DECLARE_THROW_SCOPE(vm);
    JSValue prototype = targetFunction->getPrototype(vm, globalObject);
    RETURN_IF_EXCEPTION(scope, nullptr);
    JSFunction* targetJSFunction = jsDynamicCast<JSFunction*>(vm, targetFunction);

    // We only cache the structure of the bound function if the bindee is a JSFunction since there
    // isn't any good place to put the structure on Internal Functions.
    if (targetJSFunction) {
        Structure* structure = targetJSFunction->ensureRareData(vm)->getRealmWrappedFunctionStructure();
        if (structure && structure->storedPrototype() == prototype && structure->globalObject() == globalObject)
            return structure;
    }

    Structure* result = globalObject->realmWrappedFunctionStructure();

    // It would be nice if the structure map was keyed global objects in addition to the other things. Unfortunately, it is not
    // currently. Whoever works on caching structure changes for prototype transitions should consider this problem as well.
    // See: https://bugs.webkit.org/show_bug.cgi?id=152738
    if (prototype.isObject() && prototype.getObject()->globalObject(vm) == globalObject) {
        result = vm.structureCache.emptyStructureForPrototypeFromBaseStructure(globalObject, prototype.getObject(), result);
        ASSERT_WITH_SECURITY_IMPLICATION(result->globalObject() == globalObject);
    } else
        result = Structure::create(vm, globalObject, prototype, result->typeInfo(), result->classInfo());

    if (targetJSFunction)
        targetJSFunction->ensureRareData(vm)->setRealmWrappedFunctionStructure(vm, result);

    return result;
}

RealmWrappedFunction* RealmWrappedFunction::create(VM& vm, JSGlobalObject* globalObject, JSObject* targetFunction, JSValue boundThis, double length, JSString* nameMayBeNull)
{
    auto scope = DECLARE_THROW_SCOPE(vm);

    if (nameMayBeNull) {
        nameMayBeNull->value(globalObject); // Resolving rope.
        RETURN_IF_EXCEPTION(scope, nullptr);
    }

    bool isJSFunction = getJSFunction(targetFunction);
    bool canConstruct = targetFunction->isConstructor(vm);

    NativeExecutable* executable = vm.getRealmWrappedFunction(isJSFunction, canConstruct);
    Structure* structure = getRealmWrappedFunctionStructure(vm, globalObject, targetFunction);
    RETURN_IF_EXCEPTION(scope, nullptr);
    RealmWrappedFunction* function = new (NotNull, allocateCell<RealmWrappedFunction>(vm.heap)) RealmWrappedFunction(vm, executable, globalObject, structure, targetFunction, boundThis, nameMayBeNull, length);

    function->finishCreation(vm);
    return function;
}

bool RealmWrappedFunction::customHasInstance(JSObject* object, JSGlobalObject* globalObject, JSValue value)
{
    return jsCast<RealmWrappedFunction*>(object)->m_targetFunction->hasInstance(globalObject, value);
}

RealmWrappedFunction::RealmWrappedFunction(VM& vm, NativeExecutable* executable, JSGlobalObject* globalObject, Structure* structure, JSObject* targetFunction, JSValue boundThis, JSString* nameMayBeNull, double length)
    : Base(vm, executable, globalObject, structure)
    , m_targetFunction(vm, this, targetFunction)
    , m_boundThis(vm, this, boundThis)
    , m_nameMayBeNull(vm, this, nameMayBeNull, WriteBarrier<JSString>::MayBeNull)
    , m_length(length)
{
    ASSERT(!m_nameMayBeNull || !m_nameMayBeNull->isRope());
    ASSERT(m_length >= 0);
}

void RealmWrappedFunction::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(vm, info()));
}

template<typename Visitor>
void RealmWrappedFunction::visitChildrenImpl(JSCell* cell, Visitor& visitor)
{
    RealmWrappedFunction* thisObject = jsCast<RealmWrappedFunction*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    Base::visitChildren(thisObject, visitor);

    visitor.append(thisObject->m_targetFunction);
    visitor.append(thisObject->m_boundThis);
    visitor.append(thisObject->m_nameMayBeNull);
}

DEFINE_VISIT_CHILDREN(RealmWrappedFunction);

} // namespace JSC
