// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>

#include <AS3/AS3.h>
#include <Flash++.h>

#include "FurElise.h" // tablature definition!

volatile bool shuttingDown = false;

static double noise(double min, double max)
{
    return (min + ((double)rand() / RAND_MAX) * (max - min)) / 10.0;
}

class KSString // Karplus-Strong algorithm
{
    int _sampleRate;
    int _bufSize;
    double *_buf;
    int _bufOffs;

public:
    KSString(int sampleRate = 44100) : _sampleRate(sampleRate), _buf(NULL) {
        pluck(0);
    }

    void pluck(double freq) { // pluck the string
        delete _buf;
        _bufSize = (freq == 0) ? 1 : (int)(_sampleRate / freq); // one period in size
        _buf = new double[_bufSize];
        _buf[0] = 0;
        for(int i = 1; i < _bufSize; i++) { // initialize with a burst of noise
            _buf[i] = noise(-1, 1);
        }
        _bufOffs = 0;
    }

    void hammer(double freq) { // hammer on (trivially rescale buffer)
        int oldBufSize = _bufSize;
        double *oldBuf = _buf;
        int oldOffs = _bufOffs;

        _bufSize = (freq == 0) ? 1 : (int)(_sampleRate / freq); // one period in size
        _buf = new double[_bufSize];
        _bufOffs = 0;
        for(int n = 0, p = 0; n < _bufSize; n++) { // scale old buffer into new
            _buf[n] = oldBuf[oldOffs];
            p += oldBufSize;
            oldOffs = (oldOffs + p / _bufSize) % oldBufSize;
            p %= _bufSize;
        }
        delete oldBuf;
    }

    double sample() {
        int offs = _bufOffs;
        double result = _buf[offs]; // fetch a var

        _bufOffs = (_bufOffs + 1) % _bufSize; // advance to next sample
        _buf[offs] = (result + _buf[_bufOffs]) * 0.498; // apply trivial attenuating low pass filter
        return result;
    }
};

// Simple thread safe one reader one writer sound buffer
class SoundBuffer
{
    enum { kBufEntries = 8192 };
    float _buf[kBufEntries];
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    volatile int _readHead;
    volatile int _writeHead;
    volatile int _bufCount;

public:
    SoundBuffer() : _mutex(PTHREAD_MUTEX_INITIALIZER), _cond(PTHREAD_COND_INITIALIZER), _readHead(0), _writeHead(0), _bufCount(0) {}

    // lock a section for reading with at least "min" samples
    // returned pointer contains *count samples
    const float *readLock(int *count, int min = 2) {
        pthread_mutex_lock(&_mutex);
        while(_bufCount < min) {
            pthread_cond_wait(&_cond, &_mutex);
        }
        *count = std::min(kBufEntries - _readHead, (int)_bufCount);
        pthread_mutex_unlock(&_mutex);
        return _buf + _readHead;
    }

    // unlock having processed "count" samples
    void readUnlock(int count) {
        pthread_mutex_lock(&_mutex);
        _readHead = (_readHead + count) % kBufEntries;
        _bufCount -= count;
        pthread_cond_signal(&_cond);
        pthread_mutex_unlock(&_mutex);
    }

    // lock a section for writing with a least "min" sample
    // slots available for writing
    // return pointer contains *count empty sample slots
    float *writeLock(int *count, int min = 2) {
        pthread_mutex_lock(&_mutex);
        while((kBufEntries - _bufCount) < min) {
            pthread_cond_wait(&_cond, &_mutex);
        }
        *count = std::min(kBufEntries - _writeHead, kBufEntries - _bufCount);
        pthread_mutex_unlock(&_mutex);
        return _buf + _writeHead;
    }

    // unlock having written "count" samples
    void writeUnlock(int count) {
        pthread_mutex_lock(&_mutex);
        _writeHead = (_writeHead + count) % kBufEntries;
        _bufCount += count;
        pthread_cond_signal(&_cond);
        pthread_mutex_unlock(&_mutex);
    }
};

using namespace AS3::local;

// sampleData Event listener for sound data
static var sampleDataEventListener(void *arg, var as3Args)
{
    SoundBuffer *sb = (SoundBuffer *)arg;
    flash::events::SampleDataEvent event = var(as3Args[0]); // "event" is AS3 argument one
    flash::utils::ByteArray data = event->data;
    data->endian = flash::utils::Endian::__LITTLE_ENDIAN;

    int count = 4096; // write 4096 samples

    while(count) {
        int readCount;
        const float *buf = sb->readLock(&readCount); // lock some samples
        readCount = std::min(readCount, count);

        flash::utils::ByteArray ram = internal::get_ram();
        data->writeBytes(ram, (unsigned int)buf, readCount * sizeof(float), (void*)(int)data->position);

        sb->readUnlock(readCount);
        count -= readCount;
    }

    if(shuttingDown) {
        printf("Background thread exiting");
        pthread_exit(NULL);
    }

    return internal::_undefined;
}

static void *sampleDataThreadProc(void *arg)
{
    flash::media::Sound sound = flash::media::Sound::_new(internal::_null, internal::_null);
    sound->addEventListener(flash::events::SampleDataEvent::SAMPLE_DATA, Function::_new(sampleDataEventListener, (void*)arg), false, 0, false);
    sound->play(0, 0, internal::_null);

    printf("Sound object playing\n");
    AS3_GoAsync(); // go event loop to serve sampleData events
    printf("Error: GoAsync returned!\n");
    return NULL;
}

// generate 1 note worth of samples into the SoundBuffer
static void genNoteSamples(SoundBuffer *sb, KSString strings[6])
{
    int count = 8820; // 300bpm

    while(count) { // for each left/right sample pair
        int writeCount;
        float *buf = sb->writeLock(&writeCount); // lock buffer to write to

        writeCount = std::min(writeCount & ~1 /* ensure even # */, count);

        // mix and write the samples
        for(int written = 0; written < writeCount; written += 2) {
            double sample = 0;

            for(int stringNum = 0; stringNum < 6; stringNum++) { // mix various "strings"
                sample += strings[stringNum].sample();
            }

            buf[0] = buf[1] = sample; // same sample for both right and left channels
            buf += 2;
        }

        sb->writeUnlock(writeCount);
        count -= writeCount;
    }

}

// simple, buggy tablature player!
// synchronously adds sound data to the SoundBuffer -- sampleData thread will drain the
// SoundBuffer
static void playTab(SoundBuffer *sb, KSString strings[6], double freqMult, const char * const *tab)
{
    static const double stringRootFreqs[] = { 82.407, 110, 146.83, 196, 246.94, 329.62 };
    const char *bar;

    while((bar = *tab)) { // iterate bars in the tab
        int barLen = strlen(bar) / 6;

        for(int barOffs = 2; barOffs < barLen; barOffs += 2) { // iterate across the bar
            char fretCh;

            for(int stringNum = 5; stringNum >= 0; stringNum--) { // iterate strings
                int offs = barOffs + (5 - stringNum) * barLen;
                char pfretCh = bar[offs - 1]; // previous fret char (to detect hammer on)

                fretCh = bar[offs]; // fret char
                if(fretCh != '-' && fretCh != '|') {
                    int fretNum = (fretCh >= 'a') ? fretCh - 'a' + 10 : fretCh - '0';
                    double freq = stringRootFreqs[stringNum] * exp(fretNum * 0.0578);

                    if(pfretCh == 'h' || pfretCh == '/' || pfretCh == '\\') { // hammer on in place of slide
                        strings[stringNum].hammer(freq * freqMult);
                    } else {
                        strings[stringNum].pluck(freq * freqMult);
                    }
                }
            }
            if(fretCh == '|') {
                barOffs++;
            } else {
                genNoteSamples(sb, strings);
            }
        }
        tab++;
    }
    shuttingDown = true;
}

int main()
{
    KSString strings[6];
    SoundBuffer sb;
    pthread_t thread;

    pthread_create(&thread, NULL, sampleDataThreadProc, &sb);
    playTab(&sb, strings, 1.0, furElise);
    pthread_join(thread, NULL);

    return 0;
}
