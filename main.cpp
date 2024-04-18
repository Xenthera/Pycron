#include "pocketpy.h"

int main() {

    pkpy::VM* vm = new pkpy::VM();

    vm->exec("print('hello world')");

    return 0;
}
