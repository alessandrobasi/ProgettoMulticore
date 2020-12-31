
/*

Progetto n° 7 Sudoku
Approccio multicore usato: MPI, OpenMP  
Approccio algoritmo usato: Backtracking, Algoritmo di Crook?
Studenti:
    Daniele Fardella        fardella.1642573@studenti.uniroma1.it
    Alessandro Basilici     basilici.1835392@studenti.uniroma1.it

mpiexec -n 2 .\ProgettoMulticore.exe

Sorgenti:
    https://en.wikipedia.org/wiki/Sudoku_solving_algorithms
    http://www.ams.org/notices/200904/tx090400460p.pdf
    https://ichi.pro/it/risolvi-il-sudoku-in-modo-piu-elegante-con-l-algoritmo-di-crook-in-python-105010293053459
    https://github.com/wyfok/Solve_Sudoku_with_Crook_algorithm
    https://towardsdatascience.com/solve-sudoku-more-elegantly-with-crooks-algorithm-in-python-5f819d371813

.
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <array> 
#include <stdio.h>
//#include <omp.h>  // Impostazioni di Visual Studio | Proprietà -> C/c++ -> Linguaggio -> Supporto per OpenMP
#include "mpi.h"  // https://www.microsoft.com/en-us/download/details.aspx?id=100593

#define DEBUG false
#define MAX_BUFF 81
#define ROOT 0

typedef std::array<int, 81> sudo;
typedef std::vector<std::vector<std::vector<int>>> sudonote;


bool check_row(sudo sudoku, int trynum, int riga) {

    for (int incr = 0; incr < 9; incr++) {
        if (trynum == sudoku[riga*9 + incr]) {
            return true;
        }
    }
    return false;
}

bool check_col(sudo sudoku, int trynum, int col) {
    for (int incr = 0; incr < 9; incr++) {
        if (trynum == sudoku[col + 9 * incr]) {
            return true;
        }
    }
    return false;
}

bool check_square(sudo sudoku, int trynum, int riga, int col) {
    riga /= 3;
    col /= 3;

    for (int add_riga = 0; add_riga < 3; add_riga++) {
        for (int add_col = 0; add_col < 3; add_col++) {
            if (trynum == sudoku[ 9*(3*riga + add_riga) + (3*col + add_col) ]) {
                return true;
            }
        }
    }
    return false;
}

/*
* questa funzione cerca singoli elementi nelle note
* e li inserisce nel sudoku
*/
void solve_singleton(sudo &sudoku, sudonote &note) {

    for (int riga = 0; riga < 9; riga++) {

        for (int colonna = 0; colonna < 9; colonna++) {

            if (note[riga][colonna].size() == 1) {
                sudoku[9*riga + colonna] = note[riga][colonna][0];
                note[riga][colonna].clear();
            }

        }

    }

}

/*
* questa funzione riempie la griglia delle note
* con i possibili valori dopo aver controllato
* riga, colonna e quadrato
*/
void markup(sudo sudoku, int casella, sudonote &note) {

    int riga = casella / 9;
    int colonna = casella % 9;

    for (int trynum = 1; trynum <= 9; trynum++) {

        if (!check_row(sudoku, trynum, riga) && 
            !check_col(sudoku, trynum, colonna) && 
            !check_square(sudoku, trynum, riga, colonna)) {

            note[riga][colonna].push_back(trynum);
            
        }// if
    }// for
}// markup

////////// debug
void _pprint(sudo sudoku) {
    for (int i = 0; i < 81; i++) {

        if (i % 9 == 0) {
            std::cout << std::endl;
        }

        std::cout << sudoku[i] << " ";

    }
    std::cout << std::endl;
}

void _pprint(sudonote note) {
    for (int i = 0; i < 9; i++) {

        for (int j = 0; j < 9; j++) {
            std::cout << "[";
            for (int num : note[i][j] ) {
                std::cout << num << ",";
            }
            std::cout <<"], ";
        }
        std::cout << std::endl;
        
    }
}
//////////

int main(int argc, char* argv[]) {
    
    // Inizializzo le 2 variabili di controllo
    /*
    * com_size = numero totale di processi (-n X => com_size = X+1)
    * rank     = numero univoco del processo (0 = ROOT, ... )
    */
    int com_size;
    int rank;

    // Inizializzo mpi
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &com_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (com_size % 3 != 0 && DEBUG) {
        MPI_Finalize();
        std::cout << "il numero di thread non e' un divisore o multiplo di 3" << std::endl;
        return 0;
    }


    /*
    Possiamo vedere la griglia del sudoku come una serie continua di celle
        0, ... , 8 (prima riga), 9, ... , 17, ...

    Stabiliamo prima di tutto quante celle un thread deve lavorare:

    com_size sarà il numero totale di thread a nostra disposizione
    facendo una semplice divisione e prendendo la parte intera otteniamo il lavoro per ogni singolo thread,
    (numero totale di celle)/(numero totale di processi):
        81 : se abbiamo  1 thread
        9  : se abbiamo  9 thread
        1  : se abbiamo 81 thread

    per ogni thread ci interessa la cella di partenza (inizio) e la cella di fine (fine). 
    !! NB: la cella di fine deve essere esclusa poichè è la cella di inizio del successivo thread

    */
    // array vuoto del sudoku
    sudo sudoku;
    sudo recv_sudoku;

    // griglia vuota di note
    sudonote note;

    // definisco la grandezza del numero di righe e colonne
    note.resize(9);
    for (int i = 0; i < 9; i++) {
        note[i].resize(9);
    }

    //TODO: numero di thread non divisibili per 3 lasciano le ultime caselle del sudoku invariate
    int lavoro_per_thread = 81 / com_size; // quante celle deve controllare ogni thread
    int inizio = rank * lavoro_per_thread; // cella del array di inzio
    int fine = inizio + lavoro_per_thread; // cella di fine
    if (rank == com_size - 1 && 81 % com_size != 0) {
        fine += 81 % com_size;
        lavoro_per_thread += 81 % com_size;
    }

    // per una giusta dimostrazione creo il sudoku solo nel thread master
    if (rank == ROOT) {
        // private sudoku 
        // dati disponibili solo al thread master
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
    }
    
    // allineo ogni thread su questa barriera, al fine di essere sicuro 
    // che il primo thread (che supera la bariera) riesce a trovare il sudoku già riempito
    MPI_Barrier(MPI_COMM_WORLD);


    MPI_Bcast(&sudoku, MAX_BUFF, MPI_INT, 0, MPI_COMM_WORLD);

    // a questo punto ogni thread lavora per se
    std::cout << "rank: " << rank << "/" << com_size << " inizio: " << inizio << " fine: " << fine << "(escluso)" << std::endl;

    // il thread inizia ad iterare sul numero di inizio fino al numero di fine (fine esclusa)
    for (int casella = inizio; casella < fine; casella++) {
        // controllo se il numero della casella in cui sono arrivato è uno zero
        if (sudoku[casella] != 0) {
            continue;
        }

        // sudoku[casella] == 0
        // segno nelle note (alla casella designata) i possibili valori
        markup(sudoku, casella, note);

    }

    // dopo aver riempito le note, ogni thread cerca i singoli elementi
    // e riempie il sudoku
    solve_singleton(sudoku, note);

    // Faccio la slice del grande array (sudoku)
    // Faccio il taglio dall'inizio del lavoro del thread alla fine
    std::copy(sudoku.begin() + inizio, sudoku.begin() + fine, sudoku.begin());


    // A questo punto i thread devono scambiarsi le proprie tabelle con solo le caselle lavorate
    //MPI_Allgather(&sudoku, fine - inizio, MPI_INT, &recv_sudoku, fine - inizio, MPI_INT, MPI_COMM_WORLD);
    MPI_Gather(&sudoku, fine - inizio, MPI_INT, &recv_sudoku, fine - inizio + (81 % com_size), MPI_INT, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT) {
        _pprint(recv_sudoku);
        sudoku = recv_sudoku;
    }


    MPI_Barrier(MPI_COMM_WORLD);
    
    // PASSO 3 di Crook


    MPI_Finalize();
    return 0;
}