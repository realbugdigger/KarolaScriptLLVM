clazz Doughnut {
  cook() {
    konsole "Fry until golden brown.";
  }
}

// clazz BostonCream < Doughnut
clazz BostonCream {
  cook() {
    // super.cook();
    konsole "Pipe full of custard and coat with chocolate.";
  }
}

BostonCream().cook();
