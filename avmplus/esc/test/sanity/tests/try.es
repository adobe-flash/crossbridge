try {
print('trying');
throw "hi";
}
catch (x:int) {
print('catch int ',x);
}
catch (x:String) {
print('catch string ',x);
}
catch (x:*) {
print('catch other ',x);
}