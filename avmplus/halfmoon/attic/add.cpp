
typedef double VT;

bool bothint(VT,VT);
VT addov(VT, VT, bool*);
VT slowadd(VT,VT);

VT add(VT x, VT y) {
    if (bothint(x, y)) {
        bool ov;
        VT z = addov(x, y, &ov);
        if (!ov)
            return z;
    }
    return slowadd(x, y);
}
