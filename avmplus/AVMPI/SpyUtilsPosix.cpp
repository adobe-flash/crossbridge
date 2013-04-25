/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"

#ifdef MMGC_MEMORY_PROFILER

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

const char* spy_socket_channel = "/tmp/MMgc_Spy"; //name of unix domain for spy connections

bool spy_connected  = false;
bool spy_running = false;

pthread_cond_t spy_cond; //condition variable for synchronizing spy signalling and socket communication
pthread_mutex_t spy_mutex; //mutex for spy_signal and spy_cond condition variable
pthread_t spy_thread;

int serverSocket = -1; //fd for spy server socket
int clientSocket = -1; //fd for spy socket connection (one at a time)

void* SpyConnectionLoop(void*)
{
    struct sockaddr_un sockAddr;

    socklen_t len = sizeof(sockAddr);
    while(spy_running)
    {
        //wait for spy socket connection
        if((clientSocket = accept(serverSocket, (struct sockaddr*)&sockAddr, &len)) >= 0)
        {
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(clientSocket, &fds);

            select(clientSocket+1, NULL, &fds, NULL, NULL);

            if(FD_ISSET(clientSocket, &fds))
            {
                pthread_cond_signal(&spy_cond);

                pthread_mutex_lock(&spy_mutex);
                spy_connected = true; //set the

                //wait until we are woken up my the main program thread
                //we do this to avoid processing any new incoming spy request
                pthread_cond_wait(&spy_cond, &spy_mutex);
                pthread_mutex_unlock(&spy_mutex);
            }
            close(clientSocket); //we are done, close this connection
        }
    }

    return NULL;
}

bool SetupSpyServer()
{
    //clear remnants of previous execution
    unlink(spy_socket_channel);

    struct sockaddr_un sockAddr;

    //open a server socket
    if((serverSocket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        return false;

    memset(&sockAddr, 0, sizeof(struct sockaddr_un));

    sockAddr.sun_family = AF_UNIX;
    strcpy(sockAddr.sun_path, spy_socket_channel);

    //bind and listen
    socklen_t len = sizeof(sockAddr.sun_family) + strlen(sockAddr.sun_path) + 1;
    if( (bind(serverSocket, (struct sockaddr*)&sockAddr, len) < 0) || (listen(serverSocket, 5) < 0))
    {
        close(serverSocket);
        return false;
    }

    //initialize synchronization variables
    pthread_mutex_init(&spy_mutex, NULL);
    pthread_cond_init(&spy_cond, NULL);

    spy_running = true;

    //wait for spy connections on a separate thread
    if(pthread_create(&spy_thread, NULL, SpyConnectionLoop, NULL))
    {
        close(serverSocket);
        return false;
    }

    return true;
}

//log redirector function for outputting log messages to the spy
void SpyLog(const char* message)
{
    send(clientSocket, message, VMPI_strlen(message)+1, 0);
}

typedef void (*LoggingFunction)(const char*);
extern LoggingFunction GetCurrentLogFunction();
extern void RedirectLogOutput(LoggingFunction);

void AVMPI_spyCallback()
{
    if(spy_connected)
    {
        pthread_mutex_lock(&spy_mutex);
        if(spy_connected)
        {
            spy_connected = false;

            LoggingFunction oldLogFunc = GetCurrentLogFunction();
            RedirectLogOutput(SpyLog);
            MMgc::GCHeap::GetGCHeap()->DumpMemoryInfo();
            RedirectLogOutput(oldLogFunc);

            //we are done dumping memory info to the spy
            //signal the condition variable to
            //wake up SpyConnectionLoop thread
            pthread_cond_signal(&spy_cond);
        }
        pthread_mutex_unlock(&spy_mutex);
    }
}

bool AVMPI_spySetup()
{
    //setup server socket for spy connections
    return SetupSpyServer();
}

void AVMPI_spyTeardown()
{
    spy_running = false;

    if(spy_connected)
    {
        spy_connected = false;
        pthread_cond_signal(&spy_cond);
    }
}

bool AVMPI_hasSymbols()
{
    return true;
}

#endif //MMGC_MEMORY_PROFILER
