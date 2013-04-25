#include <math.h>   // smallpt, a Path Tracer by Kevin Beason, 2008
#include <stdlib.h> // Make : g++ -O3 -fopenmp smallpt.cpp -o smallpt
#include <stdio.h>  //        Remove "-fopenmp" for g++ version < 4.2
#include <pthread.h>
#include <unistd.h>
#include <AS3/AS3.h>
#include <AS3/AS3++.h>
struct Vec {        // Usage: time ./smallpt 5000 && xv image.ppm
  double x, y, z;                  // position, also color (r,g,b)
  Vec(double x_=0, double y_=0, double z_=0){ x=x_; y=y_; z=z_; }
  Vec operator+(const Vec &b) const { return Vec(x+b.x,y+b.y,z+b.z); }
  Vec operator-(const Vec &b) const { return Vec(x-b.x,y-b.y,z-b.z); }
  Vec operator*(double b) const { return Vec(x*b,y*b,z*b); }
  Vec mult(const Vec &b) const { return Vec(x*b.x,y*b.y,z*b.z); }
  Vec& norm(){ return *this = *this * (1/sqrt(x*x+y*y+z*z)); }
  double dot(const Vec &b) const { return x*b.x+y*b.y+z*b.z; } // cross:
  Vec operator%(Vec&b){return Vec(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);}
};
struct Ray { Vec o, d; Ray(Vec o_, Vec d_) : o(o_), d(d_) {} };
enum Refl_t { DIFF, SPEC, REFR };  // material types, used in radiance()
struct Sphere {
  double rad;       // radius
  Vec p, e, c;      // position, emission, color
  Refl_t refl;      // reflection type (DIFFuse, SPECular, REFRactive)
  Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_):
    rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {}
  double intersect(const Ray &r) const { // returns distance, 0 if nohit
    Vec op = p-r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
    double t, eps=1e-4, b=op.dot(r.d), det=b*b-op.dot(op)+rad*rad;
    if (det<0) return 0; else det=sqrt(det);
    return (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
  }
};
//#define SKY 1
#define WADA 1
#if SKY
// Idea stolen from Picogen http://picogen.org/ by phresnel/greenhybrid
Vec Cen(50,40.8,-860);
Sphere spheres[] = {//Scene: radius, position, emission, color, material
  // center 50 40.8 62
  // floor 0
  // back  0

   Sphere(1600, Vec(1,0,2)*3000, Vec(1,.9,.8)*1.2e1*1.56*2,Vec(), DIFF), // sun
   Sphere(1560, Vec(1,0,2)*3500,Vec(1,.5,.05)*4.8e1*1.56*2, Vec(),  DIFF), // horizon sun2
//   Sphere(10000,Cen+Vec(0,0,-200), Vec(0.0627, 0.188, 0.569)*6e-2*8, Vec(.7,.7,1)*.25,  DIFF), // sky
   Sphere(10000,Cen+Vec(0,0,-200), Vec(0.00063842, 0.02001478, 0.28923243)*6e-2*8, Vec(.7,.7,1)*.25,  DIFF), // sky

  Sphere(100000, Vec(50, -100000, 0),  Vec(),Vec(.3,.3,.3),DIFF), // grnd
  Sphere(110000, Vec(50, -110048.5, 0),  Vec(.9,.5,.05)*4,Vec(),DIFF),// horizon brightener
  Sphere(4e4, Vec(50, -4e4-30, -3000),  Vec(),Vec(.2,.2,.2),DIFF),// mountains
//  Sphere(3.99e4, Vec(50, -3.99e4+20.045, -3000),  Vec(),Vec(.7,.7,.7),DIFF),// mountains snow

   Sphere(26.5,Vec(22,26.5,42),   Vec(),Vec(1,1,1)*.596, SPEC), // white Mirr
   Sphere(13,Vec(75,13,82),   Vec(),Vec(.96,.96,.96)*.96, REFR),// Glas
  Sphere(22,Vec(87,22,24),   Vec(),Vec(.6,.6,.6)*.696, REFR)    // Glas2
};
#elif WADA
double R=60;
//double R=120;
double T=30*M_PI/180.;
double D=R/cos(T);
double Z=60;
Sphere spheres[] = {//Scene: radius, position, emission, color, material
  // center 50 40.8 62
  // floor 0
  // back  0
  Sphere(1e5, Vec(50, 100, 0),      Vec(1,1,1)*3e0, Vec(), DIFF), // sky
  Sphere(1e5, Vec(50, -1e5-D-R, 0), Vec(),     Vec(.1,.1,.1),DIFF),           //grnd

  Sphere(R, Vec(50,40.8,62)+Vec( cos(T),sin(T),0)*D, Vec(), Vec(1,.3,.3)*.999, SPEC), //red
  Sphere(R, Vec(50,40.8,62)+Vec(-cos(T),sin(T),0)*D, Vec(), Vec(.3,1,.3)*.999, SPEC), //grn
  Sphere(R, Vec(50,40.8,62)+Vec(0,-1,0)*D,         Vec(), Vec(.3,.3,1)*.999, SPEC), //blue
  Sphere(R, Vec(50,40.8,62)+Vec(0,0,-1)*D,       Vec(), Vec(.53,.53,.53)*.999, SPEC), //back
  Sphere(R, Vec(50,40.8,62)+Vec(0,0,1)*D,      Vec(), Vec(1,1,1)*.999, REFR), //front

//   Sphere(R, Vec(50,35,Z)+Vec( cos(T),sin(T),0)*D, Vec(1,1,1)*1e-1, Vec(1,1,1)*.999, SPEC), //red
//   Sphere(R, Vec(50,35,Z)+Vec(-cos(T),sin(T),0)*D, Vec(1,1,1)*1e-1, Vec(1,1,1)*.999, SPEC), //grn
//   Sphere(R, Vec(50,35,Z)+Vec(0,-1,0)*D,           Vec(1,1,1)*1e-1, Vec(1,1,1)*.999, SPEC), //blue
//   Sphere(R, Vec(50,35,Z)+Vec(0,0,-1)*D*1.6,       Vec(1,1,1)*0e-1, Vec(0.275, 0.612, 0.949)*.999, SPEC), //back
//  Sphere(R, Vec(50,40.8,62)+Vec(0,0,1)*D*.2877,          Vec(1,1,1)*0e-1, Vec(1,1,1)*.999, REFR), //front

};
#else
Sphere spheres[] = {//Scene: radius, position, emission, color, material
  Sphere(1e5, Vec( 1e5+1,40.8,81.6), Vec(),Vec(.75,.25,.25),DIFF),//Left
  Sphere(1e5, Vec(-1e5+99,40.8,81.6),Vec(),Vec(.25,.25,.75),DIFF),//Rght
  Sphere(1e5, Vec(50,40.8, 1e5),     Vec(),Vec(.75,.75,.75),DIFF),//Back
  Sphere(1e5, Vec(50,40.8,-1e5+170), Vec(),Vec(),           DIFF),//Frnt
  Sphere(1e5, Vec(50, 1e5, 81.6),    Vec(),Vec(.75,.75,.75),DIFF),//Botm
  Sphere(1e5, Vec(50,-1e5+81.6,81.6),Vec(),Vec(.75,.75,.75),DIFF),//Top
  Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(1,1,1)*.999, SPEC),//Mirr
  Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1)*.999, REFR),//Glas
  Sphere(600, Vec(50,681.6-.27,81.6),Vec(12,12,12),  Vec(), DIFF) //Lite
};
#endif

inline double clamp(double x){ return x<0 ? 0 : x>1 ? 1 : x; }
inline int toInt(double x){ return int(pow(clamp(x),1/2.2)*255+.5); }
inline bool intersect(const Ray &r, double &t, int &id){
  double n=sizeof(spheres)/sizeof(Sphere), d, inf=t=1e20;
  for(int i=int(n);i--;) if((d=spheres[i].intersect(r))&&d<t){t=d;id=i;}
  return t<inf;
}
#define MAX_DEPTH 200
Vec radiance(const Ray &r, int depth, unsigned short *Xi){
  double t;                               // distance to intersection
  int id=0;                               // id of intersected object
  if (!intersect(r, t, id)) return Vec(); // if miss, return black
  const Sphere &obj = spheres[id];        // the hit object
  Vec x=r.o+r.d*t, n=(x-obj.p).norm(), nl=n.dot(r.d)<0?n:n*-1, f=obj.c;
  double p = f.x>f.y && f.x>f.z ? f.x : f.y>f.z ? f.y : f.z; // max refl
  if (++depth>5) if (erand48(Xi)<p) f=f*(1/p); else return obj.e; //R.R.
  if(depth > MAX_DEPTH) return obj.e; //don't recurse forever
  if (obj.refl == DIFF){                  // Ideal DIFFUSE reflection
    double r1=2*M_PI*erand48(Xi), r2=erand48(Xi), r2s=sqrt(r2);
    Vec w=nl, u=((fabs(w.x)>.1?Vec(0,1):Vec(1))%w).norm(), v=w%u;
    Vec d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).norm();
    return obj.e + f.mult(radiance(Ray(x,d),depth,Xi));
  } else if (obj.refl == SPEC)            // Ideal SPECULAR reflection
    return obj.e + f.mult(radiance(Ray(x,r.d-n*2*n.dot(r.d)),depth,Xi));
  Ray reflRay(x, r.d-n*2*n.dot(r.d));     // Ideal dielectric REFRACTION
  bool into = n.dot(nl)>0;                // Ray from outside going in?
  double nc=1, nt=1.5, nnt=into?nc/nt:nt/nc, ddn=r.d.dot(nl), cos2t;
  if ((cos2t=1-nnt*nnt*(1-ddn*ddn))<0)    // Total internal reflection
    return obj.e + f.mult(radiance(reflRay,depth,Xi));
  Vec tdir = (r.d*nnt - n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).norm();
  double a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.dot(n));
  double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
  return obj.e + f.mult(depth>2 ? (erand48(Xi)<P ?   // Russian roulette
    radiance(reflRay,depth,Xi)*RP:radiance(Ray(x,tdir),depth,Xi)*TP) :
    radiance(reflRay,depth,Xi)*Re+radiance(Ray(x,tdir),depth,Xi)*Tr);
}

#define STATS 1

#if STATS
static volatile unsigned sRgnLockSpins = 0;
static volatile unsigned sRgnLockSpinEvents = 0;
static volatile unsigned sTotalSamples = 0;
#endif

struct ThreadArgs
{
  const Ray &cam; // camera Ray
  int w, h; // width and height (must be powers of 2)
  const Vec &cx, &cy;

  Vec *accum; // color accumulator for each pixel
  unsigned *samps; // sample count for each pixel
  unsigned *rgb; // live rgb values

  unsigned poly; // lfsr poly
  
  volatile unsigned *rgnLocks; // locks to alloc atomic update of accum, samps, rgb
  unsigned rgnLockCount; // # of rgn locks (must be power of 2)

  int id; // thread id

  volatile bool *quit;
};

static inline void fence()
{
  __asm("" : : : "memory");
}

static void *threadProc(void *arg)
{
  const ThreadArgs &args = *(const ThreadArgs *)arg;
  unsigned lfsrInit = args.id + 1;
  unsigned lfsr = lfsrInit;
  unsigned yShift = 0;
  unsigned xMask;

  // figure out shift to compute y from offset
  while(args.w > (1 << yShift))
    yShift++;
  // figure out mask for x
  xMask = (1 << yShift) - 1;

  // set up rng
  unsigned short Xi[3]={0,0,args.id};
  int w = args.w;
  int h = args.h;
  const Vec &cx = args.cx;
  const Vec &cy = args.cy;
  const Ray &cam = args.cam;

  for(;;)
  {
    unsigned offset = lfsr;

    lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & args.poly);

    unsigned y = h - (offset >> yShift) - 1;
    unsigned x = offset & xMask;
    Vec c, r;

    for (int sy=0; sy<2; sy++)     // 2x2 subpixel rows
      for (int sx=0; sx<2; sx++, r=Vec()){        // 2x2 subpixel cols
          double r1=2*erand48(Xi), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
          double r2=2*erand48(Xi), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
          Vec d = cx*( ( (sx+.5 + dx)/2 + x)/w - .5) +
                  cy*( ( (sy+.5 + dy)/2 + y)/h - .5) + cam.d;
          r = r + radiance(Ray(cam.o+d*140,d.norm()),0,Xi);
          c = c + Vec(clamp(r.x),clamp(r.y),clamp(r.z))*.25; //TODO move clamp?
        }

    volatile unsigned *lock = args.rgnLocks + (offset & (args.rgnLockCount - 1));

#if STATS
    bool spun = false;
#endif
    // lock our region
    while(__sync_val_compare_and_swap(lock, 0, 1))
    {
#if STATS
      __sync_fetch_and_add(&sRgnLockSpins, 1);
      spun = true;
#endif
    }
#if STATS
    if(spun) __sync_fetch_and_add(&sRgnLockSpinEvents, 1);
#endif
    Vec ac = (args.accum[offset] = args.accum[offset] + c); // update accumulator (and retain result)
    unsigned div = (++args.samps[offset]) + 1; // update sample count (and retain result) -- add 1 for a fade-in effect
    // generate new rgb value
    args.rgb[offset] = (toInt(ac.x / div) << 16) | (toInt(ac.y / div) << 8) | toInt(ac.z / div);
    fence();
    *lock = 0; // unlock

#if STATS
    __sync_fetch_and_add(&sTotalSamples, 1);
#endif
    // check for quit every once in a while
    if(!(lfsr & 1023) && *args.quit)
      break;
  }
  return NULL;
}

struct EntryThreadArgs
{
  volatile int w, h; // out
  unsigned * volatile rgb; // out
  pthread_mutex_t initMutex;
  pthread_cond_t initCond;
};

static void *entryThreadProc(void *arg){
  EntryThreadArgs &args = *(EntryThreadArgs *)arg;
  int w=512, h=512;
  Ray cam(Vec(50,52,295.6), Vec(0,-0.042612,-1).norm()); // cam pos, dir
  Vec cx=Vec(w*.5135/h), cy=(cx%cam.d).norm()*.5135, *c=new Vec[w*h];

//  unsigned poly = (1 << 19) | (1 << 16); // poly for 20 bit lfsr (1024x1024)
  unsigned poly = (1 << 17) | (1 << 10); // poly for 18 bit lfsr (512x512)
//  unsigned poly = (1 << 15) | (1 << 13) | (1 << 12) | (1 << 10); // poly for 16 bit lfsr (256x256)
  unsigned *rgb = (unsigned *)calloc(sizeof(unsigned), w*h);
  unsigned *samps = (unsigned *)calloc(sizeof(unsigned), w*h);
  volatile bool quit = false;

  args.w = w;
  args.h = h;
  args.rgb = rgb;

  pthread_mutex_lock(&args.initMutex);
  pthread_cond_signal(&args.initCond);
  pthread_mutex_unlock(&args.initMutex);

#define THREAD_COUNT 4
#define RGN_LOCK_COUNT 65536
  unsigned rgnLocks[RGN_LOCK_COUNT] = { 0 };
  pthread_t threads[THREAD_COUNT];

  for(int i = 0; i < THREAD_COUNT; i++)
  {
    ThreadArgs args = { cam, w, h, cx, cy, c, samps, rgb, poly, rgnLocks, RGN_LOCK_COUNT, i, &quit };
    pthread_create(threads + i, NULL, threadProc, new ThreadArgs(args));
  }
#define SLEEP_TIME 10
#ifndef __AVM2__ //TODO
  sleep(15);
#else
  __asm("yield(%0)" : : "r"(SLEEP_TIME * 1000));
#endif
#if 0 // go forever!
  quit = true;
#endif
  for(int i = 0; i < THREAD_COUNT; i++)
    pthread_join(threads[i], NULL);
  FILE *f = fopen("image.ppm", "w");         // Write image to PPM file.
  fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
  for (int i=0; i<w*h; i++)
    fprintf(f,"%d %d %d ", rgb[i] >> 16, (rgb[i] >> 8) & 255, rgb[i] & 255);
#if STATS
  fprintf(stderr, "rgnLock spins: %d spin events: %d total samples: %d\n", sRgnLockSpins, sRgnLockSpinEvents, sTotalSamples);
#endif
}

struct UpdateArgs
{
  int w, h;
  unsigned *rgb;
};

static AS3::value updateProc(void *arg, AS3::value vargs)
{
  UpdateArgs &args = *(UpdateArgs *)arg;
  static bool sFirst = true;
  static AS3::value sBMD;

  if(sFirst)
  {
    AS3::value namespaceClass = AS3::getlex(AS3::new_String("Namespace"));
    AS3::value flashDisplayNS = AS3::construct(namespaceClass, AS3::new_String("flash.display"));
    AS3::value bitmapDataClass = AS3::getlex(flashDisplayNS, AS3::new_String("BitmapData"));

    sBMD = AS3::construct(bitmapDataClass, AS3::new_int(args.w), AS3::new_int(args.h), AS3::_false);
    
    AS3::value bitmapClass = AS3::getlex(flashDisplayNS, AS3::new_String("Bitmap"));
    AS3::value bitmap = AS3::construct(bitmapClass, sBMD);
    AS3::value stage = AS3::get_Stage();

    AS3::call(stage["addChild"], stage, 1, &bitmap);
    sFirst = false;
  }

  AS3::value setPixelsArgs[] = { sBMD["rect"], AS3::get_ram() };
  AS3::call(sBMD["setPixels"], sBMD, 2, setPixelsArgs, args.rgb);
  return AS3::_undefined;
}

int main(int argc, char *argv[]) {
  EntryThreadArgs entryArgs = { 0, 0, NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER };
  pthread_t entryThread;

  pthread_mutex_lock(&entryArgs.initMutex);
  pthread_create(&entryThread, NULL, entryThreadProc, &entryArgs);
  pthread_cond_wait(&entryArgs.initCond, &entryArgs.initMutex);
  pthread_mutex_unlock(&entryArgs.initMutex);
  printf("%d %d %p\n", entryArgs.w, entryArgs.h, entryArgs.rgb);

  UpdateArgs *updateArgs = new UpdateArgs;

  updateArgs->w = entryArgs.w;
  updateArgs->h = entryArgs.h;
  updateArgs->rgb = entryArgs.rgb;

  AS3::value fun = AS3::new_Function(updateProc, updateArgs);
  AS3::value stage = AS3::get_Stage();
  stage["frameRate"] = AS3::new_Number(10); // 10fps

  AS3::value aelArgs[] = { AS3::new_String("enterFrame"), fun };
  AS3::call(stage["addEventListener"], stage, 2, aelArgs);
  AS3_LibInit();
  return 0;
}
