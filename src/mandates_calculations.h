#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "csv_parsing.h"

typedef struct Quotient
{
    double quotient;
    int votes;
    Party* party;
    Coalition* coalition;
} Quotient;

void check_passing_threshold(Party** parties, int parties_count, int all_votes_sum);

int compare_quotients(const void *a, const void *b);

int calculate_mandates(Party** parties, Coalition** coalitions, int* mandates_per_district,
                       int parties_count, int coalitions_count, int districts_count, bool is_sainte_lague);