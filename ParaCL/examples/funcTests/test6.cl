b = func (x, y, z) : bar {

    b = func () : bar {
        
        return 3;
        
    }

    b() ;
} 

print b(1, 2, 3);