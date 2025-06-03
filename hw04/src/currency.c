#include "currency.h"
#include "errors.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

int compare_by_name(const void* a, const void* b)
{
    struct currency *currency1 = (struct currency*) a;
    struct currency *currency2 = (struct currency*) b;

    return strcmp(currency1->name, currency2->name);
}

static int search_currency_cmp(const void *_name, const void *_currency)
{
    const char *name = (const char *) _name;
    const struct currency *currency = (const struct currency *) _currency;
    return strcmp(name, currency->name);
}

struct currency *find_currency(struct currency_table *table, const char *name)
{
    return (struct currency *) bsearch(name, table->currencies, table->size, sizeof(struct currency),
                                       search_currency_cmp);
}

int init_currency_table(struct currency_table *table)
{
    table->capacity = 16;
    table->main_currency = NULL;
    table->size = 0;
    table->currencies = (struct currency *) malloc(sizeof(table->currencies[0]) * table->capacity);
    if (table->currencies == NULL) {
        return ALLOCATION_FAILED;
    }
    return SUCCESS;
}

int add_currency(struct currency_table *table, const char *name, double rating)
{
    if (-0.0001 <= rating && rating <= 0.0001 && (table->main_currency != NULL))
        return CURRENCY_DUPLICATE_MAIN;

    if (find_currency(table, name) != NULL) {
        return CURRENCY_ALREADY_PRESENT;
    }

    if (table->size == table->capacity) {
        table->capacity *= 2;
        struct currency *currencies;
        currencies = (struct currency *) realloc(table->currencies, table->capacity * sizeof(struct currency));
        if (currencies == NULL) {
            return ALLOCATION_FAILED;
        }
        table->currencies = currencies;
    }

    table->currencies[table->size].name = copy_string(name);
    table->currencies[table->size].rating = rating;

    if (-0.0001 <= rating && rating <= 0.0001) {
        table->main_currency = table->currencies[table->size].name;
    }
    table->size++;

    qsort(table->currencies, table->size, sizeof(struct currency), compare_by_name);
    return SUCCESS;
}

double convert_currency(struct currency_table *table, double amount, const char *currency)
{
    struct currency *found;
    found = find_currency(table, currency);
    if (!found->rating)
        return amount;
    return amount * found->rating;
}
