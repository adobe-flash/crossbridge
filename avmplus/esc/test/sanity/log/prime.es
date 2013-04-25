function prime(n) {
    var m /*: int*/ = n;
    var limit /*: int*/ = n/2;
    var i /*: int*/ = 2;
    while (i<=limit) {
        if(!(m%i)) {
            return false;
        }
        i++;
    }
    return true;
}
var j = 1000;
while(j!=0) 
{
    if( prime(j) ) 
    {
        print(j);
    }
    j--;
}
true
