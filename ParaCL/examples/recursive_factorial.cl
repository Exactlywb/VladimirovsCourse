fact = func(x) : factorial {
    res = 1;
    
    if (x > 0) {

        res = x * factorial(x - 1); 
    }
    
    res;
}

print fact(?);

