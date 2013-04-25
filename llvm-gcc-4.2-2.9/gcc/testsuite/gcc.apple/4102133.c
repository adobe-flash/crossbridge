/* APPLE LOCAL file 4102133 */
/* { dg-do compile } */
/* { dg-options "-O2" } */
struct {
int k_;
} detwfn_;
struct {
double x_[1];
} fmcom_;
void gciprt_(int *iw_)
{
int Tmp8, ibcon_, iwrk_, ibo_, jstsym_, lcivec_, iacon_, igmul_;
int i_, nsym_, ipica_, ipicb_;
ibo_ = iwrk_ + 43;
Tmp8 = detwfn_.k_;
for(i_ = 1;i_ <= Tmp8;i_++) {
  gcisym_(iw_,&fmcom_.x_[lcivec_ + -1],&fmcom_.x_[ipica_ + -1],&fmcom_.x_[ipicb_ + -1],
  &fmcom_.x_[iacon_ + -1],&fmcom_.x_[ibcon_ + -1],&fmcom_.x_[igmul_ + -1],&nsym_,
  &fmcom_.x_[iwrk_ + -1],&fmcom_.x_[ibo_ + -1],&jstsym_);
}
return ;
}
