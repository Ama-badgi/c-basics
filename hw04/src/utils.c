#include "utils.h"

#include "errors.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *copy_string(const char *str)
{
    char *copy;
    copy = (char *) malloc(strlen(str) + 1);
    if (copy == NULL) {
        return NULL;
    }
    strcpy(copy, str);
    return copy;
}

char *trim_string(char *begin, char **end)
{
    while (*begin && isspace(*begin))
        ++begin;
    if (end) {
        char *last_char = *end = begin;
        while (**end) {
            if (!isspace(**end))
                last_char = *end;
            ++(*end);
        }
        *end = last_char;
        if (**end && !isspace(**end))
            ++(*end);
    }
    return begin;
}

int empty_string(char *str)
{
    for (size_t idx = 0; idx < strlen(str); idx++) {
        if (!isspace(str[idx])) {
            return 1;
        }
    }
    return 0;
}

char *words_end(char *string)
{
    return get_token(string, isspace);
}

char *get_token(char *string, int (*condition)(int))
{
    while (*string && !condition(*string))
        ++string;
    return string;
}

int char_count(const char *string, char letter)
{
    int result = 0;
    for (; *string; string++) {
        if (*string == letter)
            result++;
    }
    return result;
}

char *read_line(FILE *input)
{
    int capacity = 16;
    int size = 0;
    char *buffer = (char *) malloc(capacity);
    if (buffer == NULL) {
        return NULL;
    }
    int c;

    while ((c = fgetc(input)) != EOF && c != '\n') {
        if (size >= capacity) {
            capacity *= 2;
            char *tmp = (char *) realloc(buffer, capacity);
            if (tmp == NULL) {
                free(buffer);
                return NULL;
            }
            buffer = tmp;
        }
        buffer[size] = c;
        size++;
    }
    buffer[size] = '\0';
    return buffer;
}
