/*
 * Copyright (C) 2021 Igalia S.L. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"
#include "Realm.h"

#include "JSGlobalObject.h"
#include "ObjectPrototype.h"

namespace JSC {

STATIC_ASSERT_IS_TRIVIALLY_DESTRUCTIBLE(Realm);

} // namespace JSC

namespace JSC {

const ClassInfo Realm::s_info = { "Realm", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(Realm) };

Realm::Realm(VM& vm, Structure* structure)
    : Base(vm, structure)
{
}

template<typename Visitor>
void Realm::visitChildrenImpl(JSCell* cell, Visitor& visitor)
{
    Realm* thisObject = jsCast<Realm*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    Base::visitChildren(thisObject, visitor);

    visitor.append(thisObject->m_globalObject);
}

DEFINE_VISIT_CHILDREN(Realm);

Realm* Realm::create(VM& vm, Structure* structure)
{
    Realm* object = new (NotNull, allocateCell<Realm>(vm.heap)) Realm(vm, structure);
    object->finishCreation(vm);
    JSGlobalObject* globalObject = JSGlobalObject::create(vm, JSGlobalObject::createStructure(vm, jsNull()));
    object->m_globalObject.set(vm, object, globalObject);
    return object;
}

void Realm::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(vm, info()));
    JSC_TO_STRING_TAG_WITHOUT_TRANSITION();
}

} // namespace JSC
