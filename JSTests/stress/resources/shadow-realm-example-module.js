export function putInGlobal(key, val) {
  globalThis.key = val;
}

export function getFromGlobal(key) {
  return globalThis.key;
}

export var anObject = new Object();

export function getAnObject() {
  return anObject;
}

export var answer = 6;
