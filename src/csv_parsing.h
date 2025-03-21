
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Coalition Coalition;
typedef struct Party
{
    char* name;
    bool is_minority;
    bool is_above_threshold;
    bool is_in_coalition;

    Coalition* coalition;
    int* votes_per_district;
    int votes_sum;
    int mandates_sum;
} Party;

struct Coalition
{
    Party** members;
    int members_count;
    int* votes_per_district;
    int votes_sum;
    int mandates_sum;
    bool is_above_threshold;
};

int parse_party_names(char* party_names_text, Party*** parties, int* parties_count);

char* trim_whitespace(char* str);

int parse_voting_results(Party*** parties, int** mandates_per_district, int parties_count, 
                         int* districts_count, int* all_votes_sum);

int find_party_index(char* party_name, Party** parties, int parties_count);

int create_coalition(Coalition*** coalitions, int* coalitions_count, char* members, 
                     Party*** parties, int parties_count, int districts_count);

void free_parties_structs(Party*** parties, int parties_count);

void free_coalition_structs(Coalition*** coalitions, int coalitions_count);