#include <ctype.h>
#include "csv_parsing.h"

int parse_party_names(char* party_names_text, Party*** parties, int* parties_count)
{
    // alokowanie pamieci na tablice ze wskaznikami na structy partii
    int new_allocated_size;
    int allocated_size = 1;
    (*parties) = (Party**)malloc(allocated_size * sizeof(Party*));
    if (*parties == NULL) return 0;

    // wczytywanie nazw partii do tablicy
    Party** temp;
    char* token = strtok(party_names_text, ";");
    token = strtok(NULL, ";");
    token = strtok(NULL, ";");
    while(token != NULL)
    {
        // podwajanie zalokowanej pamieci
        if ((*parties_count) >= allocated_size)
        {
            new_allocated_size = allocated_size * 2;
            temp = realloc((*parties), new_allocated_size * sizeof(Party*));
            if (temp == NULL) 
            {
                for (int i = 0; i < allocated_size; i++)
                {
                    free((*parties)[i] -> name);
                    free((*parties)[i]);
                }
                free(*(parties));
                return 0;
            }
            (*parties) = temp;
            allocated_size = new_allocated_size;
        }

        // alokowanie pamieci na nowa partie
        (*parties)[*parties_count] = malloc(sizeof(Party));
        if ((*parties)[*parties_count] == NULL)
        {
            for (int i = 0; i < *parties_count; i++)
            {
                free((*parties)[i] -> name);
                free((*parties)[i]);
            }
            free(*parties);
            return 0;
        }

        // zapisywanie pointera do nazwy partii i inicjowanie zmiennych
        token = trim_whitespace(token);
        (*parties)[*parties_count] -> name = malloc(strlen(token) + 1);
        if ((*parties)[*parties_count] -> name == NULL) 
        {
            for (int i = 0; i < *parties_count; i++) 
            {
                free((*parties)[i] -> name);
                free((*parties)[i]);
            }
            free((*parties)[*parties_count]);
            free(*parties);
            return 0;
        }
        strcpy((*parties)[*parties_count] -> name, token);
        (*parties)[(*parties_count)] -> is_minority = false;
        (*parties)[(*parties_count)] -> is_in_coalition = false;
        (*parties)[(*parties_count)] -> coalition = NULL;
        (*parties)[(*parties_count)] -> mandates_sum = 0;

        // wczytywanie nowej nazwy partii
        token = strtok(NULL, ";");
        (*parties_count)++;
    }
    
    // weryfikowanie wyniku
    printf("Wczytane partie:\n");
    for (int i = 0; i < (*parties_count); i++) printf("%s ", (*parties)[i] -> name);
    printf("\n\n");
    return 1;
}  

char* trim_whitespace(char* str)
{
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
    return str;
}

int parse_voting_results(Party*** parties, int** mandates_per_district, int parties_count, int* districts_count, int* all_votes_sum)
{   
    // alokowanie pamieci na glosy z kazdego okregu
    int* temp;
    int allocated_size = 10;
    for (int i = 0; i < parties_count; i++)
    {
        (*parties)[i] -> votes_per_district = (int*)calloc(allocated_size, sizeof(int));
        if ((*parties)[i] -> votes_per_district == NULL) 
        {
            for (int j = 0; j < parties_count; j++)
            {
                if (j < i) free((*parties)[j] -> votes_per_district);
                free((*parties)[j]);
            }
            free((*parties));
            return 0;
        }
    }
    
    // alokowanie pamieci na liczbe mandatow na okreg
    (*mandates_per_district) = (int*)calloc(allocated_size, sizeof(int));
    if ((*mandates_per_district) == NULL)
    {       
        free_parties_structs(parties, parties_count);
        return 0;
    }

    // wczytywanie danych
    char delim = ';';
    char *next_delim, *curr, *line = NULL;
    size_t line_size = 0;
    int curr_votes_count;
    while (getline(&line, &line_size, stdin) != -1)
    {
        // zwiekszanie pamieci
        if ((*districts_count) >= allocated_size)
        {
            allocated_size *= 2;
            for (int i = 0; i < parties_count; i++)
            {
                temp = realloc((*parties)[i] -> votes_per_district, allocated_size * sizeof(int));
                if (temp == NULL) 
                {
                    free_parties_structs(parties, parties_count);
                    return 0;
                }
                (*parties)[i] -> votes_per_district = temp;
            }
        
            temp = realloc((*mandates_per_district), allocated_size * sizeof(int));
            if (temp == NULL)
            {
                free_parties_structs(parties, parties_count);
                return 0;
            }
            (*mandates_per_district) = temp;
        }

        // usuwanie znaku nowej lini na koncu linijki
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

        // pomiecie nazwy okregu
        next_delim = strchr(line, delim);
        if (next_delim == NULL)
        {
            free_parties_structs(parties, parties_count);
            return 0;
        } 
        (*next_delim) = '\0';
        curr = next_delim + 1;

        // wczytanie liczby mandatow w okregow
        next_delim = strchr(curr, delim);
        if (next_delim == NULL)
        {
            free_parties_structs(parties, parties_count);
            return 0;
        }
        (*next_delim) = '\0';
        (*mandates_per_district)[*districts_count] = atoi(curr);
        curr = next_delim + 1;
        
        // wczytanie glosow
        for (int i = 0; i < parties_count; i++)
        {
            next_delim = strchr(curr, delim);
            if (next_delim != NULL)
            {
                (*next_delim) = '\0';
                if (*curr == '\0') curr_votes_count = 0;
                else curr_votes_count = atoi(curr);
                curr = next_delim + 1;
            } else
            {
                if (*curr == '\0') curr_votes_count = 0;
                else curr_votes_count = atoi(curr);
                curr = NULL;
            }
            (*parties)[i] -> votes_per_district[*districts_count] = curr_votes_count;
            (*parties)[i] -> votes_sum += curr_votes_count;
            (*all_votes_sum) += curr_votes_count;
        }
        (*districts_count)++;
    }
    free(line);
    return 1;
} 

int find_party_index(char* party_name, Party** parties, int parties_count)
{
    // zwraca indeks partii na podstawie jej nazwy
    for (int i = 0; i < parties_count; i++)
    {
        if (strcmp(parties[i] -> name, party_name) == 0)
        {
            return i;
        }
    }
    return -1;
}

int create_coalition(Coalition*** coalitions, int* coalitions_count, char* members, 
                     Party*** parties, int parties_count, int districts_count)
{
    // alokowanie pamieci
    Coalition** temp;
    (*coalitions_count)++;
    if (*coalitions_count == 1)
    {
        (*coalitions) = (Coalition**)malloc((*coalitions_count) * sizeof(Coalition*));
        if ((*coalitions) == NULL) 
        {
            return 0;
        }
    } 
    else
    {
        temp = realloc((*coalitions), (*coalitions_count) * sizeof(Coalition*));
        if (temp == NULL) return 0;
        (*coalitions) = temp;
    }

    // zliczanie liczby potencjalnych czlonkow
    int members_count = 1;  
    for (int i = 0; members[i]; i++) members_count += (members[i] == ';');

    // inicjalizowanie zmiennych
    int coal_idx = (*coalitions_count) - 1;
    (*coalitions)[coal_idx] = (Coalition*)malloc(sizeof(Coalition));
    if ((*coalitions)[coal_idx] == NULL) return 0;
    (*coalitions)[coal_idx] -> members = (Party**)malloc(members_count * sizeof(Party*));
    if ((*coalitions)[coal_idx] -> members == NULL) return 0;
    (*coalitions)[coal_idx] -> members_count = members_count;
    (*coalitions)[coal_idx] -> votes_per_district = (int*)calloc(districts_count, sizeof(int));
    if ((*coalitions)[coal_idx] -> votes_per_district == NULL) return 0;
    (*coalitions)[coal_idx] -> votes_sum = 0;

    // make a copy of the members string
    char* members_copy = strdup(members);
    if (members_copy == NULL) return 0;

    // wczytywanie partii do koalicji
    int party_index;
    int mem_idx = 0;
    char* name = strtok(members_copy, ";");
    while (name != NULL)
    {
        party_index = find_party_index(name, (*parties), parties_count);
        if (party_index == -1)
        {
            printf("podana w koalicji nazwa partii nie istnieje\n");
            free(members_copy);
            return 0;
        }

        // sumowanie glosow na wszystkich czlonkow koalicji
        (*parties)[party_index] -> is_in_coalition = true;
        (*parties)[party_index] -> coalition = (*coalitions)[coal_idx];
        (*coalitions)[coal_idx] -> members[mem_idx] = (*parties)[party_index];
        (*coalitions)[coal_idx] -> votes_sum += (*parties)[party_index] -> votes_sum;
        for (int i = 0; i < districts_count; i++)
        {
            (*coalitions)[coal_idx] -> votes_per_district[i] += (*parties)[party_index] -> votes_per_district[i];
        }
        mem_idx++;
        name = strtok(NULL, ";");
    }

    free(members_copy);
    return 1;
}

void free_parties_structs(Party*** parties, int parties_count)
{
    for (int i = 0; i < parties_count; i++)
    {
        free((*parties)[i]);
        free((*parties)[i] -> name);
        free((*parties)[i] -> votes_per_district);
    }
    free((*parties));
}  

void free_coalition_structs(Coalition*** coalitions, int coalitions_count)
{
    for (int i = 0; i < coalitions_count; i++)
    {
        free((*coalitions)[i]);
        free((*coalitions)[i] -> votes_per_district);
    }
    free((*coalitions));
}