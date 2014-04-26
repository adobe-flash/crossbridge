/* APPLE LOCAL file Radar 4382844 */
/* { dg-do compile } */

#include <vector>
struct vec3 {
 inline vec3() : x(0), y(0), z(0) {}
 inline vec3(const vec3 &V) : x(V.x), y(V.y), z(V.z) {}

 union {
  struct {
   float x,y,z;
  };
  float v[3];
 };
};

namespace WorldEngine
{
class Water 
{
 protected:
  struct Patch;
  typedef std::vector< Patch > PatchVec;
 public:
  Water( void );
  virtual void aboutToBeDeleted( void );
 protected:
  virtual ~Water( void ){}
  struct Patch
  {
   int indices[4];
   bool under[4];
   vec3 normal0;
   Patch( void )
   {
    for ( int i = 0; i < 4; i++ )
    {
    }
   }
   Patch( const Patch &c ):
    normal0( c.normal0 )
   {
    for ( int i = 0; i < 4; i++ )
    {
     indices[i] = c.indices[i];

    }
   }
  };
  class EdgeCell
  {
   public:
    PatchVec _patches;
    EdgeCell( );
    virtual void aboutToBeDeleted( Patch p1);
  };
};
}
extern void foo (int i);
namespace WorldEngine
{
Water::Water( void )
{
}
Water::EdgeCell::EdgeCell( )
{}

void Water::EdgeCell::aboutToBeDeleted(Patch p1)
{


 PatchVec::iterator it( _patches.begin() ), end( _patches.end() );

 for ( ; it != end; ++it )
 {
  Patch p( *it );
  foo (p.indices[3]);
 }

}
}
