/* APPLE LOCAL file radar 4783068 */
/* Don't issue error when objctive-c internally synthesizes dereferencing of a null object. */
/* { dg-options "-Werror -fobjc-gc" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */

#define NULL 0
    template <class R> class Returner;

    template <class R>
    class Ref {
    public:
        Ref()                                   :mValue(NULL) {}
        Ref( R *r )                             :mValue(r) {r->retain();}
        template <class T>
            Ref( T *t )                         :mValue(t) {t->retain();}

        Ref( const Ref &ref )                   :mValue(ref.mValue) {mValue->retain();}
        template <class T>
            Ref( const Returner<T> &ref )       :mValue(ref.extractValue()) {}
        ~Ref()                                  {mValue->release();}

        R* get() const                          {return mValue;}
        operator R* () const                    {return mValue;}
        R* operator-> () const                  {return mValue;}

        Ref& operator= (R* r)                   {mValue = (R*)transferRef(mValue,r); return *this;}
        Ref& operator= (const Ref &ref)         {return this->operator=( ref.mValue );}

        template <class T>
            Ref& operator= (T* t)               {R *r = t; mValue = (R*)transferRef(mValue,r); return *this;}
        template <class T>
            Ref& operator= (const Ref<T> &ref)  {return this->operator=( ref.get() );}
        template <class T>
            Ref& operator= (const Returner<T> &ref) {mValue->release(); 
                                                 mValue = ref.extractValue();
                                                 return *this;}
        
        bool operator== (const Ref &ref)        {return mValue==ref.mValue;}
        bool operator== (R* r)                  {return mValue==r;}
        
    private:
        R *extractValue() const                 {R *temp = mValue; mValue = NULL; return temp;}
        template <class T> friend class Returner;
        template <class T> friend bool IsEqual(const Ref<T> &a, const Ref<T> &b);
                        
        mutable R *mValue;
    };
    
struct ClientCore {
   int junk;
    void release();
};

@interface PSClientInternal
{
@public
    Ref<ClientCore> clientCore;

    id delegate;

}

- (void)setDelegate:(id)del;    // avoid C++ -fobjc-gc error

@end

@implementation PSClientInternal
- (void)setDelegate:(id)del;    // avoid C++ -fobjc-gc error
{
    delegate = del;
}
@end
