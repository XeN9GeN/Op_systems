#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define ERROR_RET_VAL -1
#define OVERWRITE 1

int env() {
    const char* var_name = "ENVIR_VAR";

 
    char* val = getenv(var_name);
    if (val == NULL) {
        fprintf(stderr, "Environment var %s not set\n", var_name);
        return ERROR_RET_VAL;
    }
    printf("Initial env value: %s\n", val);

    int setenv_result = setenv(var_name, "ChangedByProgram", OVERWRITE);
    if (setenv_result == ERROR_RET_VAL) {
        perror("setenv failed");
        return ERROR_RET_VAL;
    }

    val = getenv(var_name);
    if (val == NULL) {
        fprintf(stderr, "Error: Environment var %s disappeared after setting\n", var_name);
        return ERROR_RET_VAL;
    }

    printf("New env value: %s\n", val);
    return SUCCESS;

}

int main() {
    int env_func_return_val = env();
    if (env_func_return_val == ERROR_RET_VAL) {
        fprintf(stderr, "Error in func env()\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
