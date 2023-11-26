// let a = 5;

clazz Doughnut {
  cook() {
    konsole "Fry until golden brown.";
  }
}

// clazz BostonCream < Doughnut
clazz BostonCream {
  cook() {
    let a = 5;
    // super.cook();
    konsole "Pipe full of custard and coat with chocolate.";
  }
}

// break;

BostonCream().cook();
