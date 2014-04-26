/* APPLE LOCAL file 5562718 */
/* { dg-do compile } */
/* { dg-options "-O2" } */
namespace Foundation {
  class RefCounted {
  public:
    virtual unsigned long getCFHashCode() const;
    __attribute__((nothrow)) const RefCounted* retain() const
    {
      if (this)
	++mRefCount;
      return this;
    }
    mutable unsigned mRefCount;
  };
  template <class R> class Ref {
  public:
    Ref() :mValue(__null)
    {}
    Ref( R *r ) :mValue((R*)r->retain())
    {}
    __attribute__((always_inline)) inline R* get() const
    {
      return mValue;
    }
    mutable R *mValue;
  };
  template <class R> class Returner
  {
  public:
    template <class T> __attribute__((always_inline)) inline Returner( const Ref<T> &ref ) : mValue((R*)ref.get()->retain()) {}
    mutable R *mValue;
  };
}
using namespace Foundation;
namespace SQLite
{
  class Row :
    public RefCounted {};
}
using namespace SQLite;
Returner<Row> foobar() {
  return Ref<Row>(__null);
}
/* Compiler should not emit increment of address "4".  */
/* { dg-final { scan-assembler-not "addl\t\\\$1, 4" } } */
