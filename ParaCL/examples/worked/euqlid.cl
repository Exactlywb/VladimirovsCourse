euqlid = func(a, b) {

while (a != 0 && b != 0)
{
    if (a > b)
       a = a % b;
    else
        b = b % a;
}
a + b;
}

a = ?;
b = ?;

print euqlid(a, b);