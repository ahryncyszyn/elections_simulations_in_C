// Aleksandra Hryncyszyn, 353636

#include "mandates_calculations.h"

void free_and_exit_with_error(char* party_names_text, int** mandates_per_district, Party*** parties, 
                              int parties_count, Coalition*** coalitions, int coalitions_count);

int main(int argc, char* argv[])
{
    // wczytanie calej linijki z nazwami partii
    // getline dynamicznie alokuje pamiec na wczytywana linie
    char* party_names_text = NULL;
    size_t party_names_text_size = 0;    
    if (getline(&party_names_text, &party_names_text_size, stdin) == -1)
    {
        printf("Błąd wczytywania nazw partii\n");
        return 0;
    }

    // inicjalizowanie structow partii ich nazwami
    Party** parties = NULL;
    int parties_count = 0;
    if (!parse_party_names(party_names_text, &parties, &parties_count))
    {
        printf("Blad parsowania nazw partii\n");
        free(party_names_text);
        return 1;
    }

    // wczytywanie wynikow wyborow jako atrybutow do kazdej partii
    // przechowuje mandaty na kazdy okreg jako tablice oraz sume wszystkich glosow
    int* mandates_per_district = NULL;
    int districts_count = 0;
    int all_votes_sum = 0;
    if (!parse_voting_results(&parties, &mandates_per_district, 
        parties_count, &districts_count, &all_votes_sum))
    {
        // structy parties sa zwalniane wewnatrz funkcji parse_voting_results
        printf("Blad parsowania wynikow wyborow\n");
        free(mandates_per_district);
        free(party_names_text);
        return 1;
    }

    // tablica na structy koalicji
    int coalitions_count = 0;
    Coalition** coalitions = NULL;

    // weryfikowanie argumentow wywolania
    bool is_sainte_lague = 0;
    int party_index;
    int i = 1;
    while (i < argc)
    {
        if (strcmp(argv[i], "--minority") == 0)
        {
            i++;
            if (i >= argc)
            {
                printf("brak nazwy partii po --minority\n");
                free_and_exit_with_error(party_names_text, &mandates_per_district, &parties,
                                        parties_count, &coalitions, coalitions_count);
            }
            party_index = find_party_index(argv[i], parties, parties_count);
            if (party_index == -1)
            {
                printf("podana nazwa partii nie istnieje\n");
                free_and_exit_with_error(party_names_text, &mandates_per_district, &parties,
                                        parties_count, &coalitions, coalitions_count);
            }
            parties[party_index] -> is_minority = true;
        }
        else if (strcmp(argv[i], "--alliance") == 0)
        {
            i++;
            if (i >= argc)
            {
                printf("brak nazw partii po --aliance\n");
                free_and_exit_with_error(party_names_text, &mandates_per_district, &parties,
                                        parties_count, &coalitions, coalitions_count);
            }
            if (!create_coalition(&coalitions, &coalitions_count, argv[i], 
                                  &parties, parties_count, districts_count))
            {
                printf("błąd w tworzeniu koalicji\n");
                free_and_exit_with_error(party_names_text, &mandates_per_district, &parties,
                                        parties_count, &coalitions, coalitions_count);
            }
        }
        else if (strcmp(argv[i], "--sainte-lague") == 0)
        {
            if (is_sainte_lague)
            {
                printf("argument sainte_lague powinien byc podany tylko raz\n");
                free_and_exit_with_error(party_names_text, &mandates_per_district, &parties,
                                        parties_count, &coalitions, coalitions_count);
            }
            is_sainte_lague = 1;
        }
        else
        {
            printf("Niepoprawne argumenty wywołania\n");
            free_and_exit_with_error(party_names_text, &mandates_per_district, &parties,
                                        parties_count, &coalitions, coalitions_count);
        }
        i++;
    }

    
    // obliczanie ktore partie przekroczyly prog wyborczy
    check_passing_threshold(parties, parties_count, all_votes_sum);


    // obliczanie ile mandatow uzyskala kazda partia
    if (!calculate_mandates(parties, coalitions, mandates_per_district,
                            parties_count, coalitions_count, districts_count, is_sainte_lague))
    {
        printf("Błąd alokowania pamieci\n");
        free_and_exit_with_error(party_names_text, &mandates_per_district, &parties,
                                        parties_count, &coalitions, coalitions_count);
    }

    // wypisywanie wynikow kazdej partii
    for (int i = 0; i < parties_count; i++)
    {
       if (parties[i] -> mandates_sum > 0)
       {
            printf("Partia %s uzyskała: %d mandatow\n", parties[i] -> name, parties[i] -> mandates_sum);
       }
    }

    for (int i = 0; i < coalitions_count; i++)
    {
        if (coalitions[i] -> mandates_sum > 0)
        {
            printf("Koalicja");
            for (int j = 0; j < coalitions[i] -> members_count; j++)
            {
                printf(" %s", coalitions[i] -> members[j] -> name);
            }
            printf(" uzyskała: %d mandatow\n", coalitions[i] -> mandates_sum);
        }
    }

    // zwalnianie pamieci
    free(party_names_text);
    free(mandates_per_district);
    free_parties_structs(&parties, parties_count);
    free_coalition_structs(&coalitions, coalitions_count);
    return 0;  
}

void free_and_exit_with_error(char* party_names_text, int** mandates_per_district, Party*** parties, 
                              int parties_count, Coalition*** coalitions, int coalitions_count)
{
    free(party_names_text);
    free(*mandates_per_district);
    free_parties_structs(parties, parties_count);
    free_coalition_structs(coalitions, coalitions_count);
    exit(1);
}