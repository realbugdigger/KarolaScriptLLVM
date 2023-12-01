clazz Vehicle {
  honk() {
    console "Tuut, tuut.";
  }

  engine() {
    console "Brm, brm.";
  }

  static startEngine() {
    console "Click, click!!!";
  }
}

clazz Car < Vehicle {
  engine() {
    console "Brrrrrmmm brmmmmmmmm brrrrmmmm";
  }
}

Vehicle.startEngine();

Car().engine();
Car().honk();
