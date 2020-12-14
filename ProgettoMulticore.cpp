
/*

Progetto n° 7 Sudoku
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
#include <vector>
#include <array> 
#include <stdio.h>
//#include <omp.h>  // Impostazioni di Visual Studio | Proprietà -> C/c++ -> Linguaggio -> Supporto per OpenMP
#include "mpi.h"  // https://www.microsoft.com/en-us/download/details.aspx?id=100593


#define MAX_BUFF 256

// sizeof(sudo) = 32 byte
//typedef std::vector<std::vector<int>> sudo;
//#define sudo std::array<int, 81>;
typedef std::array<int, 81> sudo;

/*
bool check_row(sudo sudoku, int num, int i) {
    for (int item = 0; item < 9; item++) {
        if (num == sudoku[i][item]) {
            return true;
        }
    }
    return false;
}

bool check_col(sudo sudoku, int num, int j) {
    for (int item = 0; item < 9; item++) {
        if (num == sudoku[item][j]) {
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
            if (num == sudoku[(3 * i) + add_i][(3 * j) + add_j]) {
                return true;
            }
        }
    }
    return false;
}
*/

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

    com_size sarà il numero totale di thread a nostra disposizione
    facendo una semplice divisione e prendendo la parte intera otteniamo il lavoro per ogni singolo thread
        81 : se abbiamo  1 thread
        9  : se abbiamo  9 thread
        1  : se abbiamo 81 thread

    per ogni thread ci interessa la cella di partenza (inizio) e la cella di fine (fine). 
    !! NB: la cella di fine deve essere esclusa poichè è la cella di inizio del successivo thread

    */
    sudo sudoku;
    int lavoro_per_thread = 81 / com_size;
    int inizio = rank * lavoro_per_thread; // cella del vettore di inzio
    int fine = inizio + lavoro_per_thread; // cella di fine 

    std::cout << "attivo rank: " << rank << std::endl;

    if (rank == 0) {
        sudoku = {
             0 , 0 , 0 , 1 , 8 , 0 , 3 , 6 , 0 , //  0 ...  8
             0 , 7 , 0 , 2 , 0 , 4 , 0 , 0 , 8 , //  9 ... 17
             0 , 0 , 0 , 0 , 5 , 7 , 2 , 0 , 9 , // 18 ... 26
             1 , 6 , 0 , 0 , 7 , 0 , 9 , 8 , 0 , // 27 ... 35
             8 , 0 , 3 , 5 , 0 , 1 , 6 , 0 , 2 , // 36 ... 44
             0 , 5 , 7 , 0 , 9 , 0 , 0 , 3 , 4 , // 45 ... 53
             5 , 0 , 2 , 7 , 6 , 0 , 0 , 0 , 0 , // 54 ... 62
             9 , 0 , 0 , 4 , 0 , 8 , 0 , 5 , 0 , // 63 ... 71
             0 , 8 , 6 , 0 , 1 , 5 , 0 , 0 , 0 , // 72 ... 80
        };
        
        //std::cout << sizeof(sudoku[0][0]) << " " << sizeof(sudoku[0]) << " " << sudoku.size() << " " << sizeof(sudo) << std::endl;
        //std::cout << "rank 0 inviando sudoku" << std::endl;
        //MPI_Bcast(&sudoku, 81, MPI_INT, 0, MPI_COMM_WORLD);
        //MPI_Send(&sudoku, 81, MPI_INT, 1, 0, MPI_COMM_WORLD);
        
        //std::cout << "rank 0 invitato sudoku!!" << std::endl;
        
    }
    else {
        //sudoku.resize(9);
        //MPI_Recv(&sudoku, 81, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        //std::vector<int>* test;
        //MPI_Barrier(MPI_COMM_WORLD);
        
        //std::cout << "ottenuto" << std::endl;
        //std::cout << sudoku[63] << std::endl;

    }
    //std::cout << "rank fin " << rank << std::endl;


    MPI_Bcast(&sudoku, 81, MPI_INT, 0, MPI_COMM_WORLD);
    std::cout << sudoku[63] << std::endl;

    MPI_Finalize();
    return 0;
}