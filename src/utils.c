#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *strdup(const char *src) {
    char *dst = malloc(strlen (src) + 1);  // Space for length plus null
    if (dst == NULL) return NULL;          // No memory
    strcpy(dst, src);                      // Copy the characters
    return dst;                            // Return the new string
}
