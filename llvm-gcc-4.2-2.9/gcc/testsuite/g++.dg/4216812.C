/* APPLE LOCAL file 4216812 */
/* { dg-do run } */
/* { dg-options "-O2" } */
struct ControlID {
  int signature;
  int id;
};
const ControlID kHIViewWindowContentID = { 7, 1 };
class HIContentView
{
  public:
     void SetID (ControlID Y) { fID = Y; }
     ControlID fID;
};
class HIWindowFrameView
{
  public:
    HIWindowFrameView(){}
    HIContentView *fContent;
};
int main() {
  HIContentView Y;
  Y.SetID(kHIViewWindowContentID);
  HIWindowFrameView X;  X.fContent = &Y;
  if (X.fContent->fID.signature != 7) 
    return 1;
  if (X.fContent->fID.id != 1) 
    return 1;
  return 0;
}
