/* APPLE LOCAL file 3893112 */
/* { dg-do compile { target "powerpc*-*-darwin*" } } */
/* { dg-options "-O2" } */

#define ID_INLINE inline

class idVec3 {
public:
        float                   x;
        float                   y;
        float                   z;

        explicit idVec3( const float x, const float y, const float z );

        idVec3 &                operator=( const idVec3 &a );
        idVec3                  operator*( const float a ) const;
        idVec3                  operator+( const idVec3 &a ) const;
        idVec3                  operator-( const idVec3 &a ) const;
        idVec3 &                operator*=( const float a );
        friend idVec3   operator*( const float a, const idVec3 b );
};

ID_INLINE idVec3::idVec3( const float x, const float y, const float z ) {
        this->x = x; this->y = y; this->z = z; }

ID_INLINE idVec3 &idVec3::operator=( const idVec3 &a ) {
        x = a.x; y = a.y; z = a.z; return *this; }

ID_INLINE idVec3 idVec3::operator-( const idVec3 &a ) const {
        return idVec3( x - a.x, y - a.y, z - a.z ); }

ID_INLINE idVec3 idVec3::operator*( const float a ) const {
        return idVec3( x * a, y * a, z * a ); }

ID_INLINE idVec3 operator*( const float a, const idVec3 b ) {
        return idVec3( b.x * a, b.y * a, b.z * a ); }

ID_INLINE idVec3 idVec3::operator+( const idVec3 &a ) const {
        return idVec3( x + a.x, y + a.y, z + a.z ); }

#define FLOATSIGNBITSET(x) (x<0.0f)

int EdgeSplitPoint( idVec3 &v1, idVec3 &v2, idVec3 &split, float d1, float d2) {
        if ( FLOATSIGNBITSET( d1 ) == FLOATSIGNBITSET( d2 ) ) {
                return false;
        }
        split = v1 + (d1 / (d1 - d2)) * (v1 - v2);
        return true;
}
/* { dg-final { scan-assembler-not "\\-40\\(r1\\)" } } */
