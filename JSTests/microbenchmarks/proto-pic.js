let o1 = {foo: 0};
let o2 = {bar: 0, foo: 0};

let b1 = {__proto__: o1};
let b2 = {__proto__: o2};

function foo(o) {
    return o.foo;
}

for (let i = 0; i < 1000000; i++) {
  foo(b1);
  foo(b2);
}

