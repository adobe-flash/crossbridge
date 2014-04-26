/* APPLE LOCAL file radar 5080710 */
/* Test that we generate correct message call for method returning a C++ class
   with copy constructor. Such methods return the result in memory and not in
   registers. */
/* { dg-options "-fnext-runtime -m64 -mmacosx-version-min=10.5" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */

class DTLock {
public:

    DTLock();
    ~DTLock();
    bool operator==(const DTLock &L) const;
    void Lock(void) const;
private:
    DTLock(const DTLock &ToC);
    DTLock &operator=(const DTLock &ToC);


    void *mutexLock;

};

class DTDoubleArrayStorage {
public:
    DTDoubleArrayStorage(long int mv,long int nv,long int ov);
    ~DTDoubleArrayStorage();

    DTLock accessLock;
    long int m,n,o,mn,length;
    int referenceCount;
    int mutableReferences;
    double *Data;

private:
    DTDoubleArrayStorage(const DTDoubleArrayStorage &);
    DTDoubleArrayStorage &operator=(const DTDoubleArrayStorage &);
};

class DTDoubleArray {

public:
    DTDoubleArray();
    virtual ~DTDoubleArray();
    DTDoubleArray(const DTDoubleArray &A);
    DTDoubleArray &operator=(const DTDoubleArray &A);

protected:

    explicit DTDoubleArray(long int mv,long int nv=1,long int ov=1);
public:


    long int m() const;
    long int n() const;
    long int o() const;
    long int Length() const;
    bool IsEmpty() const;
    bool NotEmpty() const;


    int ReferenceCount() const;
    int MutableReferences() const;
    const double *Pointer() const;

    double operator()(long int i) const;
    double operator()(long int i,long int j) const;
    double operator()(long int i,long int j,long int k) const;
    double e(int i) const;
    double e(int i,int j) const;
    double e(int i,int j,int k) const;
    void pinfo(void) const;
    void pi(int i) const;
    void pj(int j) const;
    void pall(void) const;
    void prange(int s,int e) const;
    void pcrange(int s,int e) const;

    long int Find(double) const;


protected:
    DTDoubleArrayStorage *Storage;
    DTLock accessLock;
    double invalidEntry;


    void PrintErrorMessage(long int i) const;
    void PrintErrorMessage(long int i,long int j) const;
    void PrintErrorMessage(long int i,long int j,long int k) const;
};

@interface DoubleArrayContainer
- (DTDoubleArray)doubleArray;
@end

@interface TestController
{
}
- (void)myAction:(id)sender;
@end

@implementation TestController

- (void)myAction:(id)sender
{
 DoubleArrayContainer *dA;
 DTDoubleArray b = [dA doubleArray];
}

@end
/* { dg-final { scan-assembler "objc_msgSend_stret_fixup" } } */
