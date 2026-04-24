var global = (Function("return this"))();
global.count++;

export function foo() {
  // Called to trigger deferred module execution
}
