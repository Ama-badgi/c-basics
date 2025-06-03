#include "errors.h"

#include <stdio.h>

static const char *resolve_message(enum exit_codes code)
{
    static char unknownError[128];
    switch (code) {
    case SUCCESS:
        return NULL;
    case INVALID_NUMBER:
        return "invalid number";
    case DUPLICATE_INITIALIZATION:
        return "duplicate initialization";
    case ALLOCATION_FAILED:
        return "allocation failed";
    case CURRENCY_ALREADY_PRESENT:
        return "currency is already present";
    case CURRENCY_DUPLICATE_MAIN:
        return "duplicated main currency";
    case MISSING_MAIN_CURRENCY:
        return "no main currency";
    case CURRENCY_NOT_FOUND:
        return "currency not found";
    case PERSON_ALREADY_PRESENT:
        return "person already present";
    case PERSON_NOT_FOUND:
        return "person not found";
    case INVALID_ARGUMENTS:
        return "invalid arguments; use <program> <person-file> <currency-file> <payments-file>";
    case INVALID_FILE:
        return "file on input does not exist";
    case INVALID_FILE_FORMAT:
        return "invalid format of input file";
    case NOT_ENOUGH_PEOPLE:
        return "too few people found in input file";
    case INVALID_CHARACTERS_ID:
        return "invalid characters in ID";
    default:
        sprintf(unknownError, "unknown error (%u)", code);
        return unknownError;
    }
}

void print_error_message(enum exit_codes code)
{
    fprintf(stderr, "Error happened: %s\n", resolve_message(code));
}

int error_happened(enum exit_codes code)
{
    print_error_message(code);
    return code;
}
