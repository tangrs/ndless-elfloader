#include <os.h>

int main();

void _start() {	
	main();
}

void foo() {
    printf("FOO!\n");
}

char *globvar = "Hello from global!\n";

int main() {
    static void (*var)() = foo;
	printf("Hello World!\n");
	printf("Address of globvar is %p. Points to string at address %p\n", &globvar, globvar);
	printf("Static address of function foo %p, real address of foo %p\n", var, foo);
	
	return 0;
}
