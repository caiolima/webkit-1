/*
 * Copyright (C) 2021 Phillip Mates <pmates@igalia.com>.
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

@globalPrivate
function wrap(target)
{
    "use strict";

    if (@isCallable(target)) {
        let wrapped = (...args) => {
            // specialization of Array.prototype.map
            var argsLength = @toLength(args.length);
            var wrappedArgs = @arraySpeciesCreate(args, argsLength);

            for (var i = 0; i < argsLength; i++) {
                if (!(i in args))
                    continue;
                var mappedValue = @wrap.@call(@undefined, args[i], i, args);
                @putByValDirect(wrappedArgs, i, mappedValue);
            }

            const result = target.@apply(@undefined, wrappedArgs);
            return @wrap(result);
        };
        delete wrapped['name'];
        delete wrapped['length'];
        return wrapped;
    } else if (@isObject(target)) {
        @throwTypeError("value passing between realms must be callable or primitive");
    }
    return target;
}

function evaluate(sourceText)
{
    "use strict";

    if (!@isShadowRealm(this))
        @throwTypeError("`%ShadowRealm%.evaluate requires that |this| be a ShadowRealm instance");

    if (typeof sourceText !== 'string')
        @throwTypeError("`%ShadowRealm%.evaluate requires that the |sourceText| argument be a string");

    let result = @evalInRealm(this, sourceText)
    return @wrap(result);
}

function importValue(specifier, exportName)
{
    "use strict";

    if (!@isShadowRealm(this))
        @throwTypeError("`%ShadowRealm%.importValue requires that |this| be a ShadowRealm instance");

    let exportNameStr = @toString(exportName);
    let specifierStr = @toString(specifier);

    let lookupBinding = (module) => {
        let lookup = module[exportNameStr]
        if (lookup === @undefined)
            @throwTypeError("%ShadowRealm%.importValue requires |exportName| to exist in the |specifier|");

        return @wrap(lookup);
    };

    let crossRealmThrow = (error) => {
        // re-throw because import issues raise errors using the realm's global object
        @throwTypeError(@toString(error));
    };

    return @importInRealm(this, specifierStr).@then(lookupBinding, crossRealmThrow);
}
