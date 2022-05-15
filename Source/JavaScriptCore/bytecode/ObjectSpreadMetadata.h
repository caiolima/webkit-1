/*
 * Copyright (C) 2022 Caio Lima. All rights reserved.
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

namespace JSC {

struct ObjectSpreadMetadata {
    ObjectSpreadMetadata()
    {
        cachedSrcOffsets = nullptr;
        cachedDstOffsets = nullptr;
        size = 0;
    }

    void fillOffsets(Vector<PropertyOffset, 8>& dstOffsets, Vector<PropertyOffset, 8>& srcOffsets)
    {
        ASSERT(dstOffsets.size() == srcOffsets.size());
        ASSERT(cachedSrcOffsets == nullptr);
        ASSERT(cachedDstOffsets == nullptr);
        ASSERT(!size);

        size = dstOffsets.size();

        cachedDstOffsets = static_cast<PropertyOffset*>(fastMalloc(sizeof(PropertyOffset) * size));
        cachedSrcOffsets = static_cast<PropertyOffset*>(fastMalloc(sizeof(PropertyOffset) * size));

        memcpy(cachedDstOffsets, dstOffsets.data(), sizeof(PropertyOffset) * (size));
        memcpy(cachedSrcOffsets, srcOffsets.data(), sizeof(PropertyOffset) * (size));
    }

    void clear();

    PropertyOffset* cachedSrcOffsets;
    PropertyOffset* cachedDstOffsets;
    unsigned size;
};

inline void ObjectSpreadMetadata::clear()
{
    if (cachedSrcOffsets)
        fastFree(cachedSrcOffsets);

    if (cachedDstOffsets)
        fastFree(cachedDstOffsets);

    cachedSrcOffsets = nullptr;
    cachedDstOffsets = nullptr;
    size = 0;
}

} // namespace JSC
