/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebIDBCallbacksImpl_h
#define WebIDBCallbacksImpl_h

#if ENABLE(INDEXED_DATABASE)

#include "WebIDBCallbacks.h"
#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>

namespace WebCore { class IDBCallbacks; }

namespace WebKit {

class WebIDBCallbacksImpl : public WebIDBCallbacks {
public:
    WebIDBCallbacksImpl(PassRefPtr<WebCore::IDBCallbacks>);
    virtual ~WebIDBCallbacksImpl();

    virtual void onError(const WebIDBDatabaseError&);
    virtual void onSuccess(const WebDOMStringList&);
    virtual void onSuccess(WebIDBCursor*, const WebIDBKey&, const WebIDBKey& primaryKey, const WebSerializedScriptValue&);
    virtual void onSuccess(WebIDBDatabase*);
    virtual void onSuccess(const WebIDBKey&);
    virtual void onSuccess(const WebSerializedScriptValue&);
    virtual void onSuccess(const WebSerializedScriptValue&, const WebIDBKey&, const WebIDBKeyPath&);
    virtual void onSuccess(const WebIDBKey&, const WebIDBKey& primaryKey, const WebSerializedScriptValue&);
    virtual void onSuccess(long long);
    virtual void onSuccess();
    virtual void onBlocked();
    virtual void onBlocked(long long oldVersion);
    virtual void onUpgradeNeeded(long long oldVersion, WebIDBTransaction*, WebIDBDatabase*);

private:
    RefPtr<WebCore::IDBCallbacks> m_callbacks;
    bool m_didCreateDatabaseProxy;
};

} // namespace WebKit

#endif // ENABLE(INDEXED_DATABASE)

#endif // WebIDBCallbacksImpl_h
