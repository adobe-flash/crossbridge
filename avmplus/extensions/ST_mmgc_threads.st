// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component mmgc
%%category threads
%%ifdef     VMCFG_WORKERTHREADS

%%methods
using namespace MMgc;

class RCObjectNotifier : public RCObject
{
public:
        RCObjectNotifier(bool *isDead) : isDead(isDead) {}
        ~RCObjectNotifier() { *isDead = true; isDead = NULL; }
        bool *isDead;
};

%%decls

private:
    MMgc::GC *gc;
    MMgc::FixedAlloc *fa;
    MMgc::FixedMalloc *fm;
    bool waiting;
    bool result;
    bool isDead;
    pthread_t pthread;
    pthread_mutex_t pmutex;
    pthread_cond_t pcond;

    static void* slaveRunner(void *arg)
    {
        ((ST_mmgc_threads*)arg)->slaveRun();
        return NULL;
    }

    void slaveRun()
    {
        wait();
        {
      MMGC_ENTER_VOID;
         MMGC_GCENTER(gc);
         result &= !isDead;
         gc->ReapZCT();
         result &= !isDead;
         gc->Collect();
         result &= !isDead;
        }
        kick();
    }

    void startSlave()
    {
       pthread_create(&pthread, NULL, slaveRunner, this);
    }

    void kick()
    {
        pthread_mutex_lock (&pmutex);
        while(!waiting) {
            pthread_mutex_unlock (&pmutex);
            usleep(100);
            pthread_mutex_lock (&pmutex);
        }
        pthread_cond_signal (&pcond);
        while(waiting) {
            pthread_mutex_unlock (&pmutex);
            usleep(100);
            pthread_mutex_lock (&pmutex);
        }
        pthread_mutex_unlock (&pmutex);
    }

    void wait()
    {
        pthread_mutex_lock (&pmutex);
        GCAssert(waiting == false);
        waiting = true;
        pthread_cond_wait (&pcond, &pmutex);
        waiting = false;
        pthread_mutex_unlock (&pmutex);
    }

    static void kickAndWait(void* arg)
    {
        ST_mmgc_threads* self = (ST_mmgc_threads*)arg;
        self->kick();
        self->wait();
    }

%%prologue
    MMgc::GCConfig config;
    gc=new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);
    if (gc==NULL) {
        MMgc::GCHeap::Init();
        gc=new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);
    }
    pthread_mutex_init(&pmutex, NULL);
    pthread_cond_init(&pcond, NULL);
    result = true;
    isDead = false;
    waiting = false;

%%epilogue
    pthread_mutex_destroy(&pmutex);
    pthread_cond_destroy(&pcond);
    delete gc;

%%test mmgc_gc_root_thread
       startSlave();
       MMGC_GCENTER(gc);
       RCObjectNotifier *obj = new (gc) RCObjectNotifier(&isDead);
       {
          gc->CreateRootFromCurrentStack(kickAndWait, this);
       }

       %%verify result

       %%verify !isDead
       gc->ReapZCT();
       %%verify !isDead
       gc->Collect();
       %%verify !isDead

       pthread_join(pthread, NULL);

       printf("Ignore this: %d\n", *obj->isDead);
