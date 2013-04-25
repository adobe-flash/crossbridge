/* APPLE LOCAL file radar 6084601 */
/* block are OK with -std=c99 when objective-c programs are involved. */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC++ -std=c++98 -Werror" { target *-*-darwin* } } */

typedef int (^test_block_t)();

int main(int argc, char **argv) {
    test_block_t tb = ^(){ return 1; };

    return tb();
}
