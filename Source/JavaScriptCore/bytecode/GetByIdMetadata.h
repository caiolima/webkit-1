/*
 * Copyright (C) 2018-2019 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "LLIntPrototypeLoadAdaptiveStructureWatchpoint.h"

namespace JSC {

enum class GetByIdMode : uint8_t {
    ProtoLoad = 0, // This must be zero to reuse the higher bits of the pointer as this ProtoLoad mode.
    Default = 1,
    Unset = 2,
    ArrayLength = 3,
};

struct GetByIdModeMetadataDefault {
    StructureID structureID;
    PropertyOffset cachedOffset;
    unsigned padding1;
};
static_assert(sizeof(GetByIdModeMetadataDefault) == 12);

struct GetByIdModeMetadataUnset {
    StructureID structureID;
    unsigned padding1;
    unsigned padding2;
};
static_assert(sizeof(GetByIdModeMetadataUnset) == 12);

struct GetByIdModeMetadataArrayLength {
    ArrayProfile arrayProfile;
};
static_assert(sizeof(GetByIdModeMetadataArrayLength) == 12);

struct ProtoLoadEntry {
    StructureID structureID;
    PropertyOffset cachedOffset;
    JSObject* cachedSlot;
};

struct GetByIdModeMetadataProtoLoad {
    size_t numCases() const
    {
        if (!cases)
            return 0;
        size_t i = 0;
        for (auto* ptr = cases; ptr->structureID; ++ptr)
            ++i;
        return i;
    }

    template <typename F>
    void forEachCase(F f)
    {
        if (!cases)
            return;
        for (auto* ptr = cases; ptr->structureID; ++ptr)
            f(*ptr);
    }

    void addOrReplaceCase(ProtoLoadEntry entry)
    {
        size_t numCases = this->numCases();

        if (numCases >= Options::maxAccessVariantListSize()) {
            for (size_t i = numCases - 1; i > 0; i--)
                cases[i] = cases[i - 1];
            cases[0] = entry;
            return;
        }

        ASSERT(numCases < Options::maxAccessVariantListSize());
        ProtoLoadEntry* array = static_cast<ProtoLoadEntry*>(fastMalloc(sizeof(ProtoLoadEntry) * (numCases + 2)));
        if (cases) {
            memcpy(array + 1, cases, sizeof(ProtoLoadEntry) * (numCases + 1));
            ASSERT(array[numCases + 1].structureID == 0);
            fastFree(cases);
        } else
            array[numCases + 1].structureID = 0;

        cases = array;
        *cases = entry;
    }

    Bag<LLIntInlineCacheClearingStructureTransitionWatchpoint>& watchpoints()
    {
        return *bitwise_cast<Bag<LLIntInlineCacheClearingStructureTransitionWatchpoint>*>(&m_watchpoints);
    }

    static_assert(sizeof(uintptr_t) == sizeof(Bag<LLIntInlineCacheClearingStructureTransitionWatchpoint>));
    uintptr_t m_watchpoints; // Bag<LLIntInlineCacheClearingStructureTransitionWatchpoint> 
    ProtoLoadEntry* cases;
};
#if CPU(LITTLE_ENDIAN) && CPU(ADDRESS64)
static_assert(sizeof(GetByIdModeMetadataProtoLoad) == 16);
#endif

// In 64bit Little endian architecture, this union shares ProtoLoad's JSObject* cachedSlot with "hitCountForLLIntCaching" and "mode".
// This is possible because these values must be zero if we use ProtoLoad mode.
#if CPU(LITTLE_ENDIAN) && CPU(ADDRESS64)
union GetByIdModeMetadata {
    GetByIdModeMetadata()
    {
        defaultMode.structureID = 0;
        defaultMode.cachedOffset = 0;
        defaultMode.padding1 = 0;
        mode = GetByIdMode::Default;
        hitCountForLLIntCaching = Options::prototypeHitCountForLLIntCaching();
    }

    void clearToDefaultModeWithoutCache();
    void setUnsetMode(Structure*);
    void setArrayLengthMode();
    void setProtoLoadMode();
    void freeOldIfNeeded();

    struct {
        uint32_t padding1;
        uint32_t padding2;
        uint32_t padding3;
        uint16_t padding4;
        GetByIdMode mode;
        uint8_t hitCountForLLIntCaching; // This must be zero when we use ProtoLoad mode.
    };
    GetByIdModeMetadataDefault defaultMode;
    GetByIdModeMetadataUnset unsetMode;
    GetByIdModeMetadataArrayLength arrayLengthMode;
    GetByIdModeMetadataProtoLoad protoLoadMode;
};
static_assert(sizeof(GetByIdModeMetadata) == 16);
#else
struct GetByIdModeMetadata {
    GetByIdModeMetadata()
    {
        defaultMode.structureID = 0;
        defaultMode.cachedOffset = 0;
        defaultMode.padding1 = 0;
        mode = GetByIdMode::Default;
        hitCountForLLIntCaching = Options::prototypeHitCountForLLIntCaching();
    }

    void clearToDefaultModeWithoutCache();
    void setUnsetMode(Structure*);
    void setArrayLengthMode();
    void setProtoLoadMode();
    void freeOldIfNeeded();

    union {
        GetByIdModeMetadataDefault defaultMode;
        GetByIdModeMetadataUnset unsetMode;
        GetByIdModeMetadataArrayLength arrayLengthMode;
        GetByIdModeMetadataProtoLoad protoLoadMode;
    };
    GetByIdMode mode;
    uint8_t hitCountForLLIntCaching;
};
#endif

inline void GetByIdModeMetadata::freeOldIfNeeded()
{
    if (mode == GetByIdMode::ProtoLoad) {
        //dataLogLn("Freeing GetByIdModeMetadata");
        protoLoadMode.watchpoints().clear();
        if (protoLoadMode.cases)
            fastFree(protoLoadMode.cases);
    }
}

inline void GetByIdModeMetadata::clearToDefaultModeWithoutCache()
{
    // OOPS: stop leaking memory.

    freeOldIfNeeded();
    mode = GetByIdMode::Default;
    defaultMode.structureID = 0;
    defaultMode.cachedOffset = 0;
}

inline void GetByIdModeMetadata::setUnsetMode(Structure* structure)
{
    freeOldIfNeeded();
    mode = GetByIdMode::Unset;
    unsetMode.structureID = structure->id();
}

inline void GetByIdModeMetadata::setArrayLengthMode()
{
    freeOldIfNeeded();
    mode = GetByIdMode::ArrayLength;
    new (&arrayLengthMode.arrayProfile) ArrayProfile;
    // Prevent the prototype cache from ever happening.
    hitCountForLLIntCaching = 0;
}

inline void GetByIdModeMetadata::setProtoLoadMode()
{
    if (mode == GetByIdMode::ProtoLoad)
        return;
    mode = GetByIdMode::ProtoLoad; // This must be first set. In 64bit architecture, this field is shared with protoLoadMode.cachedSlot.
    ASSERT(mode == GetByIdMode::ProtoLoad);

    // OOPS: This struct gets zeroed during metadata construction, so we won't go down this path
    // the first time we setProtoLoadMode. That's somewhat problematic since it means we won't
    // call this Bags ctor. However, Bag's ctor just zeroes the pointer. Maybe we can rely on that.
    new (&protoLoadMode.watchpoints()) Bag<LLIntInlineCacheClearingStructureTransitionWatchpoint>;
    protoLoadMode.cases = nullptr;
}

} // namespace JSC
