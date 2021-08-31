#pragma once

#include "JSObject.h"
#include "Realm.h"

namespace JSC {

class RealmPrototype final : public JSNonFinalObject {
public:
    using Base = JSNonFinalObject;
    static constexpr unsigned StructureFlags = Base::StructureFlags;

    template<typename CellType, SubspaceAccess>
    static IsoSubspace* subspaceFor(VM& vm)
    {
        STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(RealmPrototype, Base);
        return &vm.plainObjectSpace;
    }

    static RealmPrototype* create(VM& vm, JSGlobalObject* globalObject, Structure* structure)
    {
        RealmPrototype* prototype = new (NotNull, allocateCell<RealmPrototype>(vm.heap)) RealmPrototype(vm, structure);
        prototype->finishCreation(vm, globalObject);
        return prototype;
    }

    DECLARE_INFO;

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(vm, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), info());
    }

private:
    RealmPrototype(VM&, Structure*);
    void finishCreation(VM&, JSGlobalObject*);
};

} // namespace JSC
