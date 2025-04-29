class B {
    s : String <- "Hello";
    g (y:String) : String {
        y.concat(s)
    };
    f (x:Int) : Int {
        x+1
    };
};

class A inherits B {
    a : Int;
    b : B <- new B;
    f(x:Int) : Int {
        x+a
    };
};

class Main {
    i : Int <- 0;
    main() : Int {
        {
            while i < 10
            loop
                i <- i + 1
            pool;
            1;
        }
    };
};
