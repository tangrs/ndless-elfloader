#include <os.h>

int main();

void _start() {
    main();
}

void foo() {
    printf("FOO! I didn't crash! ^_^\n");
}

char *globvar = "Hello from global!\n";

int main() {
    static void (*var)() = foo;
    printf("Hello World!\n");
    printf("Address of globvar is %p. Points to string at address %p\n", &globvar, globvar);
    printf("Static address of function foo %p, real address of foo %p\n", var, foo);

    printf("Calling foo from static variable. Hope it doesn't crash\n");
    var();

    printf("Printing contents of globvar: %s\n",globvar);

    printf("If the above reads \"Hello from global!\", this thing worked\n");
    printf("Goodbye world! \n");

    show_msgbox("Test","MSGBOX!");

    return 0;
}
