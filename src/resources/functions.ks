let pass = false;

funct a () {
    if (pass) {
        console "Help from function A again!";
        return;
    }
    console "Hello from function A!";
    pass = true;
}

funct b() {
    a();
    console "Hello from function B!";
}

console "Regular hello";

a();
b();


console "\n\n\nAnonymous functions from here!!!\n\n\n";


funct test(fn) {
  for (let i = 1; i <= 10; i = i + 1) {
    fn(i);
  }
}

test(funct (a) {
  console a;
  console "Bug Digger is the best hacker!!!";
});