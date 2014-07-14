"This tests arrays!
"
define p(x,y) {
  auto i;
  for (i=x; i<y; i++) a[i];
}

for (i=0; i<10; i++) a[i] = i;
for (i=0; i<10; i++) a[i];

for (i=1000; i<1030; i++) a[i] = i;
for (i=1000; i<1030; i++) a[i];
for (i=0; i<10; i++) a[i];

