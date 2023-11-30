funct thrice(fn) {
  for (let i = 1; i <= 3; i = i + 1) {
    fn(i);
  }
}

thrice(funct (a) {
  console a;
  console "Bug Digger is the best hacker!!!";
});