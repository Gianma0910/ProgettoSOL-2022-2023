/**
 * File that contains some useful method for strings
 * @author Gianmarco Petrocchi
 */

#include "../mystring.h"

char* str_create(char* s){
    if(s == NULL || strlen(s) == 0){
        char* empty_s = malloc(sizeof(char));
        if(empty_s == NULL){
            fprintf(stderr, "str_create() malloc error\n");
            exit(EXIT_FAILURE);
        }
        strncpy(empty_s, "", 1);
        empty_s[0] = 0;
        return empty_s;
    }

    return strdup(s);
}

char* str_concat(char* s1, char* s2){
    if(s1 == NULL && s2 != NULL)
        return str_create(s2);
    else if(s1 != NULL && s2 == NULL)
        return str_create(s1);

    char* concatenated_string = calloc(strlen(s1) + strlen(s2) + 1, sizeof(char));
    if(concatenated_string == NULL){
        return NULL;
    }
    strncpy(concatenated_string, s1, strlen(s1));
    concatenated_string[strlen(s1)] = '\0';

    return strncat(concatenated_string, s2, strlen(s2));
}

char* str_concatn(char* s1, ...){
    char* result = str_create(s1);
    va_list ap;
    va_start(ap, s1);

    char *s = NULL;

    while((s = va_arg(ap, char*)) != NULL){
        char* temp = str_concat(result, s);
        if(temp == NULL){
            free(result);
            return NULL;
        }
        free(result);
        result = temp;
    }

    va_end(ap);
    return result;
}
