#include "currency.h"
#include "errors.h"
#include "persons.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int semicolon(int letter)
{
    return letter == ';';
}

size_t count_arguments(char *line)
{
    size_t arguments = 0;

    for (size_t i = 0; i < strlen(line); i++) {
        while ((i < strlen(line)) && isspace(line[i])) {
            i++;
        }
        if (i >= strlen(line)) {
            break;
        }
        arguments++;
        while ((i < strlen(line) && !isspace(line[i]))) {
            i++;
        }
    }
    return arguments;
}

int check_alnum(char *id) {
    for (size_t i = 0; i < strlen(id); i++) {
        if (!isalnum(id[i])) {
            return 0;
        }
    }
    return 1;
}

int is_number(char *num, int is_rating)
{
    int decimal_point_found = 0;
    size_t start_idx = 0;
    if (!is_rating && (num[start_idx] == '-')) {
        start_idx++;
    }

    for (size_t i = start_idx; i < strlen(num); i++) {
        if ((!decimal_point_found) && (num[i] == '.')) {
            decimal_point_found = 1;
            continue;
        }
        if (!isdigit(num[i])) {
            return 0;
        }
    }

    return 1;
}

enum exit_codes load_currency_table(struct currency_table *table, FILE *input)
{
    char *line = NULL;
    enum exit_codes code;

    while (!feof(input)) {
        line = read_line(input);
        if ((line == NULL) && (!feof(input))) {
            free(line);
            return ALLOCATION_FAILED;
        }
        if (empty_string(line) == 0) {
            free(line);
            continue;
        }

        size_t arguments = count_arguments(line);
        if (arguments != 2) {
            free(line);
            return INVALID_FILE_FORMAT;
        }

        char *end;

        char *name = trim_string(line, NULL);
        end = words_end(name);
        *end = '\0';
        if (!check_alnum(name)) {
            free(line);
            return INVALID_CHARACTERS_ID;
        }

        char *rating = trim_string(end + 1, NULL);
        end = words_end(rating);
        *end = '\0';
        if (!is_number(rating, 1)) {
            free(line);
            return INVALID_NUMBER;
        }

        code = add_currency(table, name, strtod(rating, NULL));
        free(line);
        if (code != SUCCESS) {
            return code;
        }
    }
    if (table->main_currency == NULL) {
        return MISSING_MAIN_CURRENCY;
    }
    return SUCCESS;
}

enum exit_codes load_persons(struct persons *persons, FILE *input)
{
    char *line = NULL;
    enum exit_codes code;
    size_t persons_count = 0;

    while (!feof(input)) {
        line = read_line(input);
        if ((line == NULL) && (!feof(input))) {
            return ALLOCATION_FAILED;
        }
        if (empty_string(line) == 0) {
            free(line);
            continue;
        }

        size_t arguments = count_arguments(line);
        if (arguments < 2) {
            free(line);
            return INVALID_FILE_FORMAT;
        }

        char *end;

        char *id = trim_string(line, NULL);
        end = words_end(id);
        *end = '\0';

        if (!check_alnum(id)) {
            free(line);
            return INVALID_CHARACTERS_ID;
        }

        char *name = trim_string(end + 1, &end);
        *end = '\0';

        code = add_person(persons, id, name);
        free(line);
        if (code != SUCCESS) {
            return code;
        }
        persons_count++;
    }
    if (persons_count < 2) {
        return NOT_ENOUGH_PEOPLE;
    }
    return SUCCESS;
}

enum exit_codes load_payments(struct persons *persons, struct currency_table *table, FILE *input)
{
    char *line = NULL;

    while (!feof(input)) {
        line = read_line(input);

        if ((line == NULL) && (!feof(input))) {
            return ALLOCATION_FAILED;
        }
        if (empty_string(line) == 0) {
            free(line);
            continue;
        }

        size_t arguments = count_arguments(line);
        if (arguments != 4) {
            free(line);
            return INVALID_FILE_FORMAT;
        }

        char *end;

        char *from = trim_string(line, NULL);
        end = words_end(from);
        *end = '\0';

        char *to = trim_string(end + 1, NULL);
        end = words_end(to);
        *end = '\0';

        char *amount = trim_string(end + 1, NULL);
        end = words_end(amount);
        *end = '\0';
        if (!is_number(amount, 0)) {
            free(line);
            return INVALID_NUMBER;
        }

        char *currency = trim_string(end + 1, &end);
        *end = '\0';

        if (!find_currency(table, currency)) {
            free(line);
            return CURRENCY_NOT_FOUND;
        }

        double num_amount = strtod(amount, NULL);
        double value = convert_currency(table, num_amount, currency);
        int from_count = char_count(from, ';') + 1;
        int to_count = char_count(to, ';') + 1;

        char last_char;
        do {
            end = get_token(from, semicolon);
            last_char = *end;

            *end = '\0';

            struct person *p;
            p = find_person(persons, from);
            if (p == NULL) {
                free(line);
                return PERSON_NOT_FOUND;
            }
            p->amount += value / from_count;

            from = end;
        } while (last_char && ++from);

        do {
            end = get_token(to, semicolon);
            last_char = *end;

            *end = '\0';

            struct person *p;
            p = find_person(persons, to);
            if (p == NULL) {
                free(line);
                return PERSON_NOT_FOUND;
            }
            p->amount -= value / to_count;

            to = end;
        } while (last_char && ++to);
        free(line);
    }
    return SUCCESS;
}
