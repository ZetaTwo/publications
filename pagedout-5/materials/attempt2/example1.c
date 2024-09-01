#include <stdio.h>
__attribute__((section (".section2")))
int function2(void) {
    puts("Function 2");
}
__attribute__((section (".section1")))
int function1(void) {
    puts("Function 1");
}
int main() {
    function1();
}
