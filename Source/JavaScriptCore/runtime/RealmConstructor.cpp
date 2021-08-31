#include "config.h"
#include "RealmConstructor.h"

#include "Realm.h"
#include "JSCInlines.h"


namespace JSC {

const ClassInfo RealmConstructor::s_info = { "Function", &InternalFunction::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(RealmConstructor) };

STATIC_ASSERT_IS_TRIVIALLY_DESTRUCTIBLE(RealmConstructor);

static JSC_DECLARE_HOST_FUNCTION(callRealm);
static JSC_DECLARE_HOST_FUNCTION(constructWithRealmConstructor);

RealmConstructor::RealmConstructor(VM& vm, Structure* structure)
    : InternalFunction(vm, structure, callRealm, constructWithRealmConstructor)
{
}

void RealmConstructor::finishCreation(VM& vm, RealmPrototype* realmPrototype)
{
    Base::finishCreation(vm, 0, vm.propertyNames->Realm.string(), PropertyAdditionMode::WithoutStructureTransition);
    putDirectWithoutTransition(vm, vm.propertyNames->prototype, realmPrototype, PropertyAttribute::DontEnum | PropertyAttribute::DontDelete | PropertyAttribute::ReadOnly);
}

JSObject* constructRealm(JSGlobalObject* globalObject, JSValue, const ArgList& args)
{
    VM& vm = globalObject->vm();
    auto scope = DECLARE_THROW_SCOPE(vm);
    int numArgs = args.size();
    Structure* realmStructure = Realm::createStructure(vm, globalObject, globalObject->realmPrototype());
    if (numArgs == 1) {
      return Realm::create(vm, realmStructure);
    }
    return Realm::create(vm, realmStructure);
}

JSC_DEFINE_HOST_FUNCTION(constructWithRealmConstructor, (JSGlobalObject* globalObject, CallFrame* callFrame))
{
    ArgList args(callFrame);
    return JSValue::encode(constructRealm(globalObject, callFrame->newTarget(), args));
}

JSC_DEFINE_HOST_FUNCTION(callRealm, (JSGlobalObject* globalObject, CallFrame*))
{
    auto scope = DECLARE_THROW_SCOPE(globalObject->vm());
    return JSValue::encode(throwConstructorCannotBeCalledAsFunctionTypeError(globalObject, scope, "Realm"));
}

} // namespace JSC

