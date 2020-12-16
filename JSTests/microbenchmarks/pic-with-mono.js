let o1 = {foo: 0};

let b1 = {__proto__: o1};

for (let i = 0; i < 10000000; i++) {
  let v = b1.foo;
}

