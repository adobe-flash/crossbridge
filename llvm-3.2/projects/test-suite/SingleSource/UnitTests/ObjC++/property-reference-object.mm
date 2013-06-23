#import <Foundation/Foundation.h>

namespace {
extern "C"
void objc_copyCppObjectAtomic(void *dest, const void *src, void (*copyHelper) (void *dest, const void *source)) {
   copyHelper(dest, src);
}
}

static int count;
class Foo
{
        static int sNextId;
        int     mId;
        int     mRefId;
public:
	Foo(const Foo& rhs){ 
	  mId = sNextId++;
          mRefId = rhs.mId;
          printf("Foo(%d,%d)\n", mId, mRefId);
        };

	Foo() { 
	  mId = sNextId++;
          mRefId = mId;
          printf("Foo(%d,%d)\n", mId,mRefId);
        }
	~Foo(){ 
	  printf("~Foo(%d, %d)\n", mId, mRefId);
        };
	
	Foo& operator=(const Foo& rhs){
	  mRefId = rhs.mRefId;
          return *this;
        };

	int Data() { return fData; };
	
private:
	int fData;
};

int Foo::sNextId = 0;


#pragma mark -


@interface TNSObject : NSObject
{
@private
	Foo _cppObjectNonAtomic;
	Foo _cppObjectAtomic;
	Foo _cppObjectDynamic;
}

@property (assign, readwrite, nonatomic) const Foo& cppObjectNonAtomic;
@property (assign, readwrite) const Foo  cppObjectAtomic;
@property (assign, readwrite, nonatomic) const Foo& cppObjectDynamic;
@end


#pragma mark -


@implementation TNSObject

@synthesize cppObjectNonAtomic = _cppObjectNonAtomic;
@synthesize cppObjectAtomic = _cppObjectAtomic;
@dynamic cppObjectDynamic;

- (id)init
{
    self = [super init];
    if (self) {
    
        // Add your subclass-specific initialization here.
        // If an error occurs here, send a [self release] message and return nil.

		Foo cppObject;
                self.cppObjectNonAtomic = cppObject;
		self.cppObjectAtomic = cppObject;
                self.cppObjectDynamic = cppObject;
    }
    return self;
}

- (const Foo&) cppObjectDynamic
{
	return _cppObjectDynamic;
}

- (void) setCppObjectDynamic: (const Foo&)cppObject
{
	_cppObjectDynamic = cppObject;
}
@end


#pragma mark -


int main (int argc, const char * argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    [[[TNSObject alloc] init] autorelease];

    [pool drain];
    return 0;
}

