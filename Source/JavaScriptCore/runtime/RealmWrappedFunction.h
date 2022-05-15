#pragma once

#include "JSFunction.h"

namespace JSC {

JSC_DECLARE_HOST_FUNCTION(realmWrappedThisNoArgsFunctionCall);
JSC_DECLARE_HOST_FUNCTION(realmWrappedFunctionCall);
JSC_DECLARE_HOST_FUNCTION(realmWrappedThisNoArgsFunctionConstruct);
JSC_DECLARE_HOST_FUNCTION(realmWrappedFunctionConstruct);

class RealmWrappedFunction final : public JSFunction {
public:
    typedef JSFunction Base;
    static constexpr unsigned StructureFlags = Base::StructureFlags & ~ImplementsDefaultHasInstance;
    static_assert(StructureFlags & ImplementsHasInstance, "");

    template<typename CellType, SubspaceAccess mode>
    static IsoSubspace* subspaceFor(VM& vm)
    {
        return vm.realmWrappedFunctionSpace<mode>();
    }

    static RealmWrappedFunction* create(VM&, JSGlobalObject*, JSObject* targetFunction, JSValue boundThis, double length, JSString* nameMayBeNull);

    static bool customHasInstance(JSObject*, JSGlobalObject*, JSValue);

    JSObject* targetFunction() { return m_targetFunction.get(); }
    JSValue boundThis() { return m_boundThis.get(); }
    JSString* nameMayBeNull() { return m_nameMayBeNull.get(); }
    const String& nameString()
    {
        if (!m_nameMayBeNull)
            return emptyString();
        ASSERT(!m_nameMayBeNull->isRope());
        bool allocationAllowed = false;
        return m_nameMayBeNull->tryGetValue(allocationAllowed);
    }

    double length(VM&) { return m_length; }

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue prototype)
    {
        ASSERT(globalObject);
        return Structure::create(vm, globalObject, prototype, TypeInfo(JSFunctionType, StructureFlags), info());
    }

    static ptrdiff_t offsetOfTargetFunction() { return OBJECT_OFFSETOF(RealmWrappedFunction, m_targetFunction); }
    static ptrdiff_t offsetOfBoundThis() { return OBJECT_OFFSETOF(RealmWrappedFunction, m_boundThis); }

    DECLARE_INFO;

private:
    RealmWrappedFunction(VM&, NativeExecutable*, JSGlobalObject*, Structure*, JSObject* targetFunction, JSValue boundThis, JSString* nameMayBeNull, double length);

    void finishCreation(VM&);
    DECLARE_VISIT_CHILDREN;

    WriteBarrier<JSObject> m_targetFunction;
    WriteBarrier<Unknown> m_boundThis;
    WriteBarrier<JSString> m_nameMayBeNull;
    double m_length;
};

} // namespace JSC
