#include <stdio.h>

int main() {
    puts("Hello World!");
    return 0;
}


void backdoor() {
    puts("Backdoor!");
}

/*
__attribute__((section (".init2"))) int init2(void) {
    asm(
        "call _init\n"
        "call backdoor\n"
    );
}
*/
