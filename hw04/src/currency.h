#include "errors.h"
#include "utils.h"

#ifndef CURRENCY_H
#define CURRENCY_H

struct currency
{
    char *name;
    double rating;
};

struct currency_table
{
    struct currency *currencies;
    char *main_currency;
    size_t size;
    size_t capacity;
};

int init_currency_table(struct currency_table *table);
int add_currency(struct currency_table *table, const char *name, double rating);
double convert_currency(struct currency_table *table, double amount, const char *currency);
struct currency *find_currency(struct currency_table *table, const char *name);

#endif
