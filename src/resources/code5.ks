//          funct thrice(fn) {
//                for (let i = 1; i <= 3; i = i + 1) {
//                    fn(i);
//                }
//            }
//
//    thrice(funct (a) {
//        konsole a;
//    });


let i = 0;

funct thrice(fn) {
  for ( i = 1; i <= 3; i = i + 1) {
    fn(i);
  }
}

thrice(funct (a) {
  konsole a;
});
