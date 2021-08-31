#pragma once

#include "InternalFunction.h"
#include "RealmPrototype.h"

namespace JSC {

class GetterSetter;

class RealmConstructor final : public InternalFunction {
public:
    typedef InternalFunction Base;
    static constexpr unsigned StructureFlags = Base::StructureFlags | HasStaticPropertyTable;

    static RealmConstructor* create(VM& vm, Structure* structure, RealmPrototype* realmPrototype, GetterSetter*)
    {
        RealmConstructor* constructor = new (NotNull, allocateCell<RealmConstructor>(vm.heap)) RealmConstructor(vm, structure);
        constructor->finishCreation(vm, realmPrototype);
        return constructor;
    }

    DECLARE_INFO;

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(vm, globalObject, prototype, TypeInfo(InternalFunctionType, StructureFlags), info());
    }

private:
    RealmConstructor(VM&, Structure*);
    void finishCreation(VM&, RealmPrototype*);
};
STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(RealmConstructor, InternalFunction);

} // namespace JSC

