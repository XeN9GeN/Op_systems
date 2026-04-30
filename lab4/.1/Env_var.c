#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0

void env() {
    const char* var_name = "ENVIR_VAR";

 
    char* val = getenv(var_name);
    if (val == NULL) {
        printf("Enviroment var didnt set\n");
    }
    else {
        printf("Cur val of env: %s\n", val);
    }

    //setenv(var_name, "ChangedByProgram", 1); - linux
    _putenv_s(var_name, "ChangedByProgram");// - windows

    val = getenv(var_name);
    if (val == NULL) {
        printf("Enviroment var didnt set\n");
    }
    else {
        printf("Cur val of env: %s\n", val);
    }
}

int main() {
    env();
    return SUCCESS;
}
