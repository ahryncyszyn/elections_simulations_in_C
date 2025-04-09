# Elections Simulation

This repository contains a program that simulates the distribution of parliamentary mandates based on the input CSV file containing voting data.

## Features

- **Electoral Threshold**: Parties that receive less than **5%** of the total national vote are excluded from mandate allocation (unless declared as a minority).
- **D'Hondt Method**: The default method for calculating mandates distribution, using division by consecutive natural numbers.
- **Sainte-Laguë Method**: An alternative mandates allocation method based on division by consecutive odd numbers (enabled via a command-line flag).
- **Alliances**: Supports forming electoral alliances between parties via command-line options. Alliances are subject to a higher electoral threshold of **8%**.
- **Minority Parties**: Allows declaring ethnic minority parties, which are exempt from the standard 5% threshold.

## Installation
The code can be cloned locally to your device using:
```
git clone git@github.com:ahryncyszyn/elections_simulations_in_C.git
```

## Compilation
Compilation can be achieved with one simple command:
```
make all
```
To run the simulation, you have to provide your own CSV file with data (see: [`next section`](##input-data-format)) or use [example ones](https://github.com/ahryncyszyn/elections_simulations_in_C/tree/main/input).
As the program reads data from the standard input (`stdin`), it is the most convenient to pass the input using redirection operator `<`.
```
./elections_simulation < input/wyniki* optional_flags
```
For example, to recreate results from Polish parliamentary elections in 2019, you should run:
```
./elections_simulation < input/wyniki2019.csv.lf --minority MN
```

## Input Data Format
The input is a semicolon-separated file (CSV). \ 
The first row contains the names of the first two columns (can be anything) and subsquently, names of all parties separated by semicolon. An example would be (in Polish):
```
okreg;mandatow;KO;AZER;KWIN;PSL;Prawica;PIS;Skuteczni;SLD;BIS;MN
```
The rest of the rows contain constituency name, numer of mandates, and votes per each party.
```
1;12;108191;;25319;31006;;183364;;71061;13495;
```
Missing values are treated as 0.

## Command Line Arguments
`--minority party_name` declares the specified party as representing an ethnic minority, exempting it from the 5% national electoral threshold.

`--alliance "party_name1;party_name2;..."` declares an alliance of listed parties, which means that their votes are summed and treated as one entity, but the electoral threshold is raised to 8%. 

`--sainte-lague` enables the **Sainte-Laguë method** for seat distribution, replacing the default D’Hondt method.
