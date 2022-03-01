f = func (n) : foo {
  x = 1;
  if (n) {
    x = x + foo(n-1);
  }
  x;
}
a = foo(?);
print a;