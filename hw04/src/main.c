#include "currency.h"
#include "errors.h"
#include "load.h"
#include "persons.h"
#include "structures.h"

#include <string.h>

void free_memory(struct currency_table *table, struct persons *persons)
{
    for (size_t i = 0; i < table->size; i++) {
        free(table->currencies[i].name);
    }

    for (size_t i = 0; i < persons->size; i++) {
        free(persons->persons[i].id);
        free(persons->persons[i].name);
    }

    free(table->currencies);
    free(persons->persons);
}

struct person *find_extreme(struct persons *persons, int sign)
{
    if (persons->size == 0)
        return NULL;
    struct person *extreme;
    double amount = 0;
    for (size_t i = 0; i < persons->size; i++) {
        if (persons->persons[i].amount * sign > amount) {
            extreme = &(persons->persons[i]);
            amount = extreme->amount * sign;
        }
    }
    if (-0.0001 <= amount && amount <= 0.0001) {
        return NULL;
    }
    return extreme;
}

void settle_debts(struct persons *persons, char **main_currency)
{
    struct person *debtor = find_extreme(persons, -1);
    struct person *creditor = find_extreme(persons, 1);

    while ((debtor != NULL) && (creditor != NULL) && (debtor != creditor)) {
        double amount = -debtor->amount;
        if (amount > creditor->amount)
            amount = creditor->amount;

        debtor->amount += amount;
        creditor->amount -= amount;

        printf("%s (%s) -> %s (%s): ", debtor->name, debtor->id, creditor->name, creditor->id);
        if ((int) amount == amount) {
            printf("%.0f %s\n", amount, *main_currency);
        } else {
            printf("%.2f %s\n", amount, *main_currency);
        }
        debtor = find_extreme(persons, -1);
        creditor = find_extreme(persons, 1);
    }
}

int main(int argc, char **argv)
{
    enum exit_codes exit_code;
    struct currency_table currency_table;
    struct persons persons;
    FILE *person_file;
    FILE *currency_file;
    FILE *payment_file;
    memset(&currency_table, 0, sizeof(currency_table));
    memset(&persons, 0, sizeof(persons));

    if (argc != 4) {
        return error_happened(INVALID_ARGUMENTS);
    }

    // Initialising structures
    exit_code = init_currency_table(&currency_table);
    if (exit_code != SUCCESS) {
        return error_happened(exit_code);
    }
    exit_code = init_persons(&persons);
    if (exit_code != SUCCESS) {
        free(currency_table.currencies);
        return error_happened(exit_code);
    }

    // Loading persons
    person_file = fopen(argv[1], "r");
    if (person_file == NULL) {
        free_memory(&currency_table, &persons);
        return error_happened(INVALID_FILE);
    }
    exit_code = load_persons(&persons, person_file);
    fclose(person_file);
    if (exit_code != SUCCESS) {
        free_memory(&currency_table, &persons);
        return error_happened(exit_code);
    }

    // Loading currencies
    currency_file = fopen(argv[2], "r");
    if (currency_file == NULL) {
        free_memory(&currency_table, &persons);
        return error_happened(INVALID_FILE);
    }
    exit_code = load_currency_table(&currency_table, currency_file);
    fclose(currency_file);
    if (exit_code != SUCCESS) {
        free_memory(&currency_table, &persons);
        return error_happened(exit_code);
    }

    // Loading payments
    payment_file = fopen(argv[3], "r");
    if (payment_file == NULL) {
        free_memory(&currency_table, &persons);
        return error_happened(INVALID_FILE);
    }
    exit_code = load_payments(&persons, &currency_table, payment_file);
    fclose(payment_file);
    if (exit_code != SUCCESS) {
        free_memory(&currency_table, &persons);
        return error_happened(exit_code);
    }

    settle_debts(&persons, &currency_table.main_currency);
    free_memory(&currency_table, &persons);

    return 0;
}
