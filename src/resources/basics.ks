let a = 5;
let b = 2;

console "Hello World" + (a + b);

// console (10 / 0);

let name = "bug_digger";
name = toUpper(name);
console name;
name = toLower(name);
console name;

let suffix = "_hacker";
console name + suffix;


let truthy = false;
console (truthy ? a : (a + b));


console "== Loops section ==\n\n";

for (let i = 1; i < 10; i = i + 1) {
    console i;
}

let i = 0;
while (i <= 100) {
    console i;
    i = i + 10;
}