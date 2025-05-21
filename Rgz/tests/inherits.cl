class A inherits B {
  x : Int <- 0;
};

class B inherits C {
  y : Int <- 1;
};

class C inherits D {
  z : Int <- 2;
};

class D inherits E {
  w : Int <- 3;
};

class E inherits B { 
  v : Int <- 4;
};

class Main {
  main(): Int {
    10
  };
};
