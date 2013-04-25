// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component mmgc
%%category externalalloc

%%prefix
using namespace MMgc;
class MyCallback : public MMgc::OOMCallback
{
public:
    MMgc::MemoryStatus status;

    MyCallback()
    {
        status = MMgc::GCHeap::GetGCHeap()->GetStatus();
        MMgc::GCHeap::GetGCHeap()->AddOOMCallback(this);
    }

    void memoryStatusChange(MMgc::MemoryStatus /*oldStatus*/, MMgc::MemoryStatus newStatus)
    {
        status = newStatus;
    }
};

%%decls
private:
    MyCallback *cb;

%%prologue
    cb = new MyCallback();

%%epilogue
    cb = NULL;

%%test externalAllocation
    MMGC_GCENTER(core->gc);
    size_t softlimit = GCHeap::GetGCHeap()->Config().heapSoftLimit;
    // Remove the heapSoftLimit, this should ensure that we are not in a MMgc::kMemSoftLimit state,
    // and hopefully we are in a kMemNormal state otherwise we are in a kMemAbort state and all bets are off.
    GCHeap::GetGCHeap()->Config().heapSoftLimit = 0;
    %%verify cb->status==MMgc::kMemNormal
    GCHeap::GetGCHeap()->Config().heapSoftLimit = GCHeap::GetGCHeap()->GetTotalHeapSize();
    MMgc::GCHeap::SignalExternalAllocation(1024*1024);
    %%verify cb->status==MMgc::kMemSoftLimit
    MMgc::GCHeap::SignalExternalDeallocation(1024*1024);
    %%verify cb->status==MMgc::kMemNormal
    GCHeap::GetGCHeap()->Config().heapSoftLimit = softlimit;
    softlimit = 0;


