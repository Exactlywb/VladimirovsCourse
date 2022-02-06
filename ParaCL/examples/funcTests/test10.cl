

foo = func (): smartRec {

    bar = func (x) {

        smartRec ();
    }

    bar (smartRec ());
}

print foo();