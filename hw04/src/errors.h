#include <setjmp.h>

#ifndef ERRORS_H
#define ERRORS_H

enum exit_codes
{
    SUCCESS = 0,

    INVALID_NUMBER,
    DUPLICATE_INITIALIZATION,
    ALLOCATION_FAILED,
    CURRENCY_ALREADY_PRESENT,
    CURRENCY_DUPLICATE_MAIN,
    MISSING_MAIN_CURRENCY,
    CURRENCY_NOT_FOUND,
    PERSON_ALREADY_PRESENT,
    PERSON_NOT_FOUND,

    INVALID_ARGUMENTS,
    INVALID_FILE,
    INVALID_FILE_FORMAT,
    NOT_ENOUGH_PEOPLE,
    INVALID_CHARACTERS_ID
};

int error_happened(enum exit_codes code);

#endif
