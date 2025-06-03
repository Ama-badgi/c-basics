#include "persons.h"

#include <stdio.h>

#ifndef LOAD_H_
#define LOAD_H_

enum exit_codes load_currency_table(struct currency_table *table, FILE *input);
enum exit_codes load_persons(struct persons *persons, FILE *input);
enum exit_codes load_payments(struct persons *persons, struct currency_table *table, FILE *input);

#endif
