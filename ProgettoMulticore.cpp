
/*

Progetto n� 7 Sudoku
Approccio multicore usato: MPI, OpenMP  
Approccio algoritmo usato: Backtracking, Algoritmo di Crook?
Studenti:
    Daniele Fardella        fardella.1642573@studenti.uniroma1.it
    Alessandro Basilici     basilici.1835392@studenti.uniroma1.it

Sorgenti:
    https://en.wikipedia.org/wiki/Sudoku_solving_algorithms
    http://www.ams.org/notices/200904/tx090400460p.pdf
    https://ichi.pro/it/risolvi-il-sudoku-in-modo-piu-elegante-con-l-algoritmo-di-crook-in-python-105010293053459

*/

#include <iostream>
#include <stdio.h>
//#include <omp.h>  // Impostazioni di Visual Studio | Propriet� -> C/c++ -> Linguaggio -> Supporto per OpenMP
#include "mpi.h"  // https://www.microsoft.com/en-us/download/details.aspx?id=100593
#include <vector>

#define MAX_BUFF 256

typedef std::vector<std::vector<std::pair<std::vector<int>, int>>> sudo;


bool check_row(sudo sudoku, int num, int i) {
    for (int item = 0; item < 9; item++) {
        if (num == sudoku[i][item].second) {
            return true;
        }
    }
    return false;
}

bool check_col(sudo sudoku, int num, int j) {
    for (int item = 0; item < 9; item++) {
        if (num == sudoku[item][j].second) {
            return true;
        }
    }
    return false;
}

bool check_square(sudo sudoku, int num, int i, int j) {
    i /= 3;
    j /= 3;

    for (int add_i = 0; add_i < 3; add_i++) {
        for (int add_j = 0; add_j < 3; add_j++) {
            if (num == sudoku[(3 * i) + add_i][(3 * j) + add_j].second) {
                return true;
            }
        }
    }
    return false;
}


int main(int argc, char* argv[]) {
    char buff[MAX_BUFF] = {0};
    int com_size;
    int rank;

    // inizializzo mpi
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &com_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /*
    Possiamo vedere la griglia del sudoku come una serie continua di celle
        0, ... , 8 (prima riga), 9, ... , 17, ...


    Stabiliamo prima di tutto quante celle un thread deve lavorare:

    com_size sar� il numero totale di thread a nostra disposizione
    facendo una semplice divisione e prendendo la parte intera otteniamo il lavoro per ogni singolo thread
        81 : se abbiamo  1 thread
        9  : se abbiamo  9 thread
        1  : se abbiamo 81 thread

    per ogni thread ci interessa la cella di partenza (inizio)
    e la cella di fine (fine), NB la cella di fine deve essere esclusa 


    */


    int lavoro_per_thread = 81.0 / com_size;
    int inizio = rank * lavoro_per_thread; // cella del vettore
    int fine = inizio + lavoro_per_thread;

    if (rank == 0) {

        sudo sudoku = {
            { { {}, 0 }, { {}, 0 }, { {}, 0 }, { {}, 1 }, { {}, 8 }, { {}, 0 }, { {}, 3 }, { {}, 6 }, { {}, 0 } }, //  0 ...  8
            { { {}, 0 }, { {}, 7 }, { {}, 0 }, { {}, 2 }, { {}, 0 }, { {}, 4 }, { {}, 0 }, { {}, 0 }, { {}, 8 } }, //  9 ... 17
            { { {}, 0 }, { {}, 0 }, { {}, 0 }, { {}, 0 }, { {}, 5 }, { {}, 7 }, { {}, 2 }, { {}, 0 }, { {}, 9 } }, // 18 ... 26
            { { {}, 1 }, { {}, 6 }, { {}, 0 }, { {}, 0 }, { {}, 7 }, { {}, 0 }, { {}, 9 }, { {}, 8 }, { {}, 0 } }, // 27 ... 35
            { { {}, 8 }, { {}, 0 }, { {}, 3 }, { {}, 5 }, { {}, 0 }, { {}, 1 }, { {}, 6 }, { {}, 0 }, { {}, 2 } }, // 36 ... 44
            { { {}, 0 }, { {}, 5 }, { {}, 7 }, { {}, 0 }, { {}, 9 }, { {}, 0 }, { {}, 0 }, { {}, 3 }, { {}, 4 } }, // 45 ... 53
            { { {}, 5 }, { {}, 0 }, { {}, 2 }, { {}, 7 }, { {}, 6 }, { {}, 0 }, { {}, 0 }, { {}, 0 }, { {}, 0 } }, // 54 ... 62
            { { {}, 9 }, { {}, 0 }, { {}, 0 }, { {}, 4 }, { {}, 0 }, { {}, 8 }, { {}, 0 }, { {}, 5 }, { {}, 0 } }, // 63 ... 71
            { { {}, 0 }, { {}, 8 }, { {}, 6 }, { {}, 0 }, { {}, 1 }, { {}, 5 }, { {}, 0 }, { {}, 0 }, { {}, 0 } }, // 72 ... 80
        };
        


    }
    else {
        


    }




    MPI_Finalize();
    return 0;
}