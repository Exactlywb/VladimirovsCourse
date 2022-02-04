
fib = func (x): Fibonacci {

    if (x == 0) {
        return 0;
    }
    if (x == 1 || x == 2) {
        return 1;    
    }

    return Fibonacci (x - 1) + Fibonacci (x - 2);

};

print fib (?);

