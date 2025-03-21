#include "mandates_calculations.h"

void check_passing_threshold(Party** parties, int parties_count, int all_votes_sum)
{
    int threshold5 = 0.05 * all_votes_sum;
    int threshold8 = 0.08 * all_votes_sum;

    Party* party;
    bool did_pass;
    for (int i = 0; i < parties_count; i++)
    {
        party = parties[i];
        if (!(party -> is_minority) && !(party -> is_in_coalition) &&
             party -> votes_sum < threshold5)
        {
            party -> is_above_threshold = false;
        }
        else if (!(party -> is_minority) && party -> is_in_coalition)
        {
            did_pass = (party -> coalition -> votes_sum < threshold8) ? false : true;
            party -> coalition -> is_above_threshold = did_pass;
            party -> is_above_threshold = did_pass;
        }
        else party -> is_above_threshold = true;
    }
}

int compare_quotients(const void *a, const void *b) 
{
    Quotient* q1 = (Quotient*) a;
    Quotient* q2 = (Quotient*) b;

    if (q2 -> quotient > q1 -> quotient) return 1;
    if (q2 -> quotient < q1 -> quotient) return -1;

    return q2 -> votes - q1 -> votes;
}

int calculate_mandates(Party** parties, Coalition** coalitions, int* mandates_per_district,
                       int parties_count, int coalitions_count, int districts_count, bool is_sainte_lague)
{
    // zliczenie koalicji lub partii ktore przekroczyly prog
    int valid_committees = 0;
    for (int i = 0; i < parties_count; i++)
    {
        if (parties[i] -> is_above_threshold && !(parties[i] -> is_in_coalition)) valid_committees++;
    }
    for (int i = 0; i < coalitions_count; i++)
    {
        if (coalitions[i] -> is_above_threshold) valid_committees++;
    }

    // alokowanie pamieci na structy ilorazow
    Quotient* quotients = (Quotient*)malloc(valid_committees * sizeof(Quotient));
    if (quotients == NULL) return 0;

    // rodzielanie mandatow w kazdym okregu
    Quotient* temp;
    int all_mandates, divisor, all_quotients, q_index = 0;
    for (int dist = 0; dist < districts_count; dist++)
    {
        q_index = 0;
        all_mandates = mandates_per_district[dist];
        all_quotients = all_mandates * valid_committees;

        // dostosowanie pamieci do ilosci mandatow w okregu
        temp = (Quotient*)realloc(quotients, all_quotients * sizeof(Quotient));
        if (temp == NULL) 
        {
            free(quotients);
            return 0;
        }
        quotients = temp;

        // obliczanie ilorazow dla partii
        for (int p = 0; p < parties_count; p++)
        {
            if (!(parties[p] -> is_above_threshold) || parties[p] -> is_in_coalition) continue;

            for (int m = 1; m <= all_mandates; m++) 
            {
                divisor = is_sainte_lague ? (2 * m - 1) : m;

                quotients[q_index].quotient = (double) (parties[p] -> votes_per_district[dist]) / divisor;
                quotients[q_index].votes = parties[p] -> votes_per_district[dist];
                quotients[q_index].coalition = NULL;
                quotients[q_index].party = parties[p];
                q_index++;
            }
        }

        // obliczanie ilorazow dla koalicji
        for (int c = 0; c < coalitions_count; c++) 
        {
            if (!coalitions[c] -> is_above_threshold) continue;

            for (int m = 1; m <= all_mandates; m++) 
            {
                int divisor = is_sainte_lague ? (2 * m - 1) : m;

                quotients[q_index].quotient = (double) (coalitions[c] -> votes_per_district[dist]) / divisor;
                quotients[q_index].votes = coalitions[c] -> votes_per_district[dist];
                quotients[q_index].coalition = coalitions[c];
                quotients[q_index].party = NULL;
                q_index++;
            }
        }

        // sortowanie ilorazow
        qsort(quotients, q_index, sizeof(Quotient), compare_quotients);

        // przydzielanie mandatow
        for (int m = 0; m < all_mandates; m++) 
        {
            if (quotients[m].party != NULL) 
            {
                quotients[m].party -> mandates_sum++;
            } else if (quotients[m].coalition != NULL) 
            {
                quotients[m].coalition -> mandates_sum++;
            }
        }
    }

    // zwalnianie pamieci
    free(quotients);
    return 1;
}
