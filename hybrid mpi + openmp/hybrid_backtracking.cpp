/*

Progetto n� 7 Sudoku
Approccio multicore usato: MPI + OpenMP (Hybrid)
Approccio algoritmo usato: Backtracking
Studenti:
    Daniele Fardella        fardella.1642573@studenti.uniroma1.it
    Alessandro Basilici     basilici.1835392@studenti.uniroma1.it

Sorgenti:
    https://en.wikipedia.org/wiki/Sudoku_solving_algorithms
    http://www.ams.org/notices/200904/tx090400460p.pdf
    https://docs.microsoft.com/it-it/cpp/parallel/openmp/openmp-in-visual-cpp?view=msvc-160
    https://software.intel.com/content/www/us/en/develop/documentation/mpi-developer-guide-linux/top/compiling-and-linking/compiling-an-mpi-program.html
*/

#include <iostream>
#include <stdio.h>
#include <omp.h>
#include "mpi.h"  // https://www.microsoft.com/en-us/download/details.aspx?id=100593

//To compile a hybrid MPI/OpenMP* program using the Intel compiler, use the -qopenmp option. For example: $ mpiicc - qopenmp test.c - o testc

/*
Progetto Backtracking Multi-thread hybrid MPI/OpenMP
*/

/////////////////////////////
/*
questo gruppo di funzioni controllano nel sudoku passato se rispettivamente,
in riga, colonna o quadratro esiste o meno il numero passato,
se esiste ritorna true, altrimentri false
*/
#define MAX_BUFF 81
#define ROOT 0

typedef std::array<int, 81> sudo;
typedef std::vector<std::vector<std::vector<int>>> sudonote;

bool _check_row(int sudoku[9][9], int trynum, int riga) {
    bool test = false;
    int incr;
#pragma omp parallel for private(incr) firstprivate(riga) schedule(static)
    for (incr = 0; incr < 9; incr++) {
        if (!test && trynum == sudoku[riga][incr]) {
            test = true;
        }
    }
    return test;
}

bool _check_col(int sudoku[9][9], int trynum, int col) {
    bool test = false;
    int incr;
#pragma omp parallel for private(incr) firstprivate(col) schedule(static)
    for (incr = 0; incr < 9; incr++) {
        if (!test && trynum == sudoku[incr][col]) {
            test = true;
        }
    }
    return test;
}

bool _check_square(int sudoku[9][9], int trynum, int riga, int col) {
    riga -= riga % 3;
    col -= col % 3;
    bool test = false;
    int add_riga, add_col;
#pragma omp parallel for private(add_riga, add_col) firstprivate(riga, col) schedule(static)
    for (add_riga = 0; add_riga < 3; add_riga++) {
        for (add_col = 0; add_col < 3; add_col++) {
            if (!test && trynum == sudoku[riga + add_riga][col + add_col]) {
                test = true;
            }
        }
    }
    return test;
}
/////////////////////////////

/*
questa funzione raggruppa i risultati del gruppo di verifica del numero
ritorna false se almeno una funzione ritorna true
*/
bool check_space(int sudoku[9][9], int trynum, int row, int col) {

    bool Brow, Bcol, Bsquare;

    Brow = _check_row(sudoku, trynum, row);
    Bcol = _check_col(sudoku, trynum, col);
    Bsquare = _check_square(sudoku, trynum, row, col);

    // le funzioni ritornano se il numero � presente nella riga|colonna|quadrato
    // di conseguenza se � presente il posto non va bene
    return !Brow && !Bcol && !Bsquare;
}

/*
dato il sudoku, ritorno il numero di riga e colonna della prima
posizione libera presente
*/
void next_pos_vuoto(int sudoku[9][9], int& row, int& col, int num_threads) {
    int i, j;
    // uso il parallelismo per eseguire i passi del loop
#pragma omp parallel for schedule(static, num_threads) private(i, j)
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            if (sudoku[i][j] == 0) {
                if (row < 0 && col < 0) {
                    row = i;
                    col = j;
                }// if row col
            }// if sudoku
        }// for j
    }// for i
}// next_pos_vuoto

/*
funzione ricorsiva, chiama la funzione 'next_pos_vuoto' per ottenere una posizione vuota
passo base: controlla se c'è almeno una posizione vuota, se non c'è torna indietro (soluzione trovata)
passo induttivo: controlla se è possibile assegnare un valore alla posizione,
    se è possibile, segna la posizione ed entra in ricorsione
    se non è possibile trovare un numero corretto, un numero precedente è errato, ternima quel ciclo di ricorsione
*/
bool backtracking(int sudoku[9][9], int num_threads) {
    int row = -1, col = -1;
    // Cerco la prima posizione vuota nello schema
    next_pos_vuoto(sudoku, row, col, num_threads);

    // Controllo se esiste almeno una posizione vuota
    if (row < 0 && col < 0) {
        return true;
    }

    // tento di cercare il numero giusto
    // !! Nessun parallelismo poich� ricorsivo
    // !! Su Windows non � presente la direttiva Task
    int trynum;
    for (trynum = 1; trynum < 10; trynum++) {

        // calcolo se il numero del for pu� essere inserito
        if (check_space(sudoku, trynum, row, col)) {
            sudoku[row][col] = trynum;

            // eseguo la ricorsione avendo bloccato la prima posisione vuota
            if (backtracking(sudoku, num_threads)) {
                return true;
            }
            else {
                sudoku[row][col] = 0;
            }
            // se tutte le ricorsioni falliscono azzera la posizione

        }

    }


    return false;
}

////////// debug ////////////
void _pprint(int sudoku[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            std::cout << sudoku[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n" << std::endl;
}
/////////////////////////////

int main(int argc, char* argv[]) {  

    // Inizializzo le 2 variabili di controllo
    /*
    * com_size = numero totale di processi (-n X => com_size = X+1)
    * rank     = numero univoco del processo (0 = ROOT, ... )
    */
    int com_size;
    int rank;   
    int errs = 0;
    int provided, flag, claimed;

    // Inizializzo mpi
    //con required = MPI_THREAD_FUNNELED solo il master thread (cioè quello che ha chiamato MPI_INIT_THREAD) fa le chiamate MPI 
    MPI_Init_thread(0, 0, MPI_THREAD_FUNNELED, &provided);

    MPI_Is_thread_main(&flag);
    if (!flag) {
        errs++;
        printf("This thread called init_thread but Is_thread_main gave false\n"); fflush(stdout);
    }

    MPI_Query_thread(&claimed);
    if (claimed != provided) {
        errs++;
        printf("Query thread gave thread level %d but Init_thread gave %d\n", claimed, provided); fflush(stdout);
    }
    
    MPI_Comm_size(MPI_COMM_WORLD, &com_size);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (com_size % 3 != 0) {
        MPI_Finalize();
        std::cout << "il numero di thread non e' un divisore o multiplo di 3" << std::endl;
        return 0;
    }

    if (provided < required) {

        // Controllo se il livello di permesso

        if (rank == 0) {
            cout << "Warning:  This MPI implementation provides insufficient"
                << " threading support." << endl;
        }
        //Per openmp
        int thread_count;
        if (argc <= 1) {
            std::cout << "Inserire come argomento al programma il numero di thread\n" << argv[0] << " <Num thread>\n\nIl programma continua con 1 thread (dove possibile)\n" << std::endl;
            thread_count = 1;
        }
        else {
            thread_count = atoi(argv[1]);
        }
        //Per openmp
    }


    /*
    Possiamo vedere la griglia del sudoku come una serie continua di celle
        0, ... , 8 (prima riga), 9, ... , 17, ...

    Stabiliamo prima di tutto quante celle un thread deve lavorare:

    com_size sar� il numero totale di thread a nostra disposizione
    facendo una semplice divisione e prendendo la parte intera otteniamo il lavoro per ogni singolo thread,
    (numero totale di celle)/(numero totale di processi):
        81 : se abbiamo  1 thread
        9  : se abbiamo  9 thread
        1  : se abbiamo 81 thread

    per ogni thread ci interessa la cella di partenza (inizio) e la cella di fine (fine).
    !! NB: la cella di fine deve essere esclusa poich� � la cella di inizio del successivo thread

    */
    // array vuoto del sudoku
    sudo sudoku;
    sudo recv_sudoku; // Si può rimuovere?

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
    // che il primo thread (che supera la barriera) riesce a trovare il sudoku gi� riempito
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&sudoku, MAX_BUFF, MPI_INT, 0, MPI_COMM_WORLD);

    // a questo punto ogni thread lavora per se
    std::cout << "rank: " << rank << "/" << com_size << " inizio: " << inizio << " fine: " << fine << "(escluso)" << std::endl;

    // il thread inizia ad iterare sul numero di inizio fino al numero di fine (fine esclusa)
    for (int casella = inizio; casella < fine; casella++) {
        // controllo se il numero della casella in cui sono arrivato � uno zero
        if (sudoku[casella] != 0) {
            continue;
        }

        // sudoku[casella] == 0, quindi
        // inizio algoritmo di backtracking
        backtracking(sudoku, thread_count);

    }

    // Faccio la slice del grande array (sudoku)
    // Faccio il taglio dall'inizio del lavoro del thread alla fine
    std::copy(sudoku.begin() + inizio, sudoku.begin() + fine, sudoku.begin());

    // A questo punto i thread devono scambiarsi le proprie tabelle con solo le caselle lavorate
    //MPI_Allgather(&sudoku, fine - inizio, MPI_INT, &recv_sudoku, fine - inizio, MPI_INT, MPI_COMM_WORLD);
    MPI_Gather(&sudoku, fine - inizio, MPI_INT, &recv_sudoku, fine - inizio + (81 % com_size), MPI_INT, ROOT, MPI_COMM_WORLD);
    // Da adattare senza &recv_sudoku?

    if (rank == ROOT) {
        _pprint(recv_sudoku);
        sudoku = recv_sudoku;
    }


    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    return errs;

}