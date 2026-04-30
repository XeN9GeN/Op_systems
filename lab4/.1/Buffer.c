#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INTEGER_OF_UNIVERCE 100
#define SUCCESS 0

void create_buffer() {
    int size = INTEGER_OF_UNIVERCE;
    const char* phrase = "hello world";

  
    char* buff1 = (char*)malloc(size);
    if (buff1 == NULL) {
        perror("Error of allocation memory: buff1\n");
        return;
    }
    strcpy(buff1, phrase);

    printf("Buffer1 phrase before free: %s\n", buff1);
    free(buff1);
    printf("Buffer1 phrase after free: %s\n", buff1);


    
    char* buff2 = (char*)malloc(size);
    if (buff2 == NULL) {
        perror("Error of allocation memory: buff2\n");
        return;
    }
    strcpy(buff2, phrase);

    printf("Buffer2 phrase before free: %s\n", buff2);

    char* cursor_buff2 = buff2 + (size / 2);
    free(cursor_buff2);

    printf("Buffer2 phrase after free: %s\n", buff2);
}

int main() {
    create_buffer();
    return SUCCESS;
}
