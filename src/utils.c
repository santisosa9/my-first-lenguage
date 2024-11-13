#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char* current_file_path = NULL;

char* repeat_char(char chr, int times){
    char* result = (char*)malloc(sizeof(char) * times + 1);
    for(int i = 0; i < times; i++){
        result[i] = chr;
    }
    result[times] = '\0';
    return result;
}

char* repeat_str(char* str, int times){
    char* result = (char*)malloc(sizeof(char) * times * strlen(str) + 1);
    result[0] = '\0';
    for(int i = 0; i < times; i++){
        strncat(result, str, strlen(str));
    }
    return result;
}

char* itos(int n){
    char* result = (char*)malloc(sizeof(char) * 12);
    sprintf(result, "%d", n);
    return result;
}


// Returns a heap allocated string that is a duplicate of the string src.
char* strdup(const char *src) {
    size_t len = strlen(src) + 1;       // Space for length plus null
    char *dst = (char*)malloc(len);     // Space for length plus null
    if (dst == NULL) return NULL;       // No memory
    strncpy(dst, src, len);             // Copy the characters
    return dst;                         // Return the new string
}

char* get_file_path(){
    return current_file_path == NULL ? strdup("") : current_file_path;
}


void assert_pre(bool condition, const char* error_msg, const char* precondition_fault_msg){
    if(!condition){
        fprintf(stderr, "Error: %s\n", error_msg);
        fprintf(stderr, "Precondition fault: %s\n", precondition_fault_msg);
        exit(EXIT_FAILURE);
    }
}
