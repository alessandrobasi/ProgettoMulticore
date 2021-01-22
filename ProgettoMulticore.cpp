
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
    https://docs.microsoft.com/it-it/cpp/parallel/openmp/openmp-in-visual-cpp?view=msvc-160

*/

#include <iostream>
#include <stdio.h>
#include <omp.h>

/////////////////////////////
bool _check_row(int* sudoku, int trynum, int riga) {
    bool test = false;
    #pragma omp parallel for num_threads(9) firstprivate(sudoku)
    for (int incr = 0; incr < 9; incr++) {
        if (!test && trynum == sudoku[riga * 9 + incr]) {
            test = true;
        }
    }
    return test;
}

bool _check_col(int* sudoku, int trynum, int col) {
    bool test = false;
    #pragma omp parallel for num_threads(9) firstprivate(sudoku)
    for (int incr = 0; incr < 9; incr++) {
        if (!test && trynum == sudoku[col + 9 * incr]) {
            test = true;
        }
    }
    return test;
}

bool _check_square(int* sudoku, int trynum, int riga, int col) {
    riga /= 3;
    col /= 3;
    bool test = false;
    #pragma omp parallel for num_threads(9) firstprivate(sudoku)
    for (int add_riga = 0; add_riga < 3; add_riga++) {
        for (int add_col = 0; add_col < 3; add_col++) {
            if (!test && trynum == sudoku[9 * (3 * riga + add_riga) + (3 * col + add_col)]) {
                test = true;
            }
        }
    }
    return test;
}
/////////////////////////////

/* prendo il sudoku, il numero da testare e riga e colonna "virtuali"
ritorno un booleano per indicare se trynum va bene (non viola le regole del gioco)*/
bool check_space(int* sudoku, int trynum, int row, int col) {

    bool Brow, Bcol, Bsquare;

    // creo una verifica parallela delle 3 funzioni di verifica
    #pragma omp parallel num_threads(3) firstprivate(sudoku)
    {
        int t = omp_get_thread_num();
        switch (t) {
        case 0:
            Brow = _check_row(sudoku, trynum, row);
            break;
        case 1:
            Bcol = _check_col(sudoku, trynum, col);
            break;
        case 2:
            Bsquare = _check_square(sudoku, trynum, row, col);
            break;
        default:
            break;
        }
    }
    // le funzioni ritornano se il numero è presente nella riga|colonna|quadrato
    // di conseguenza se è presente il posto non va bene
    return !Brow && !Bcol && !Bsquare;
}

/* prendo il sudoku e una variabile vuota pos
modifico pos con la prima posizione vuota*/
void next_pos_vuoto(int* sudoku, int& pos, int num_threads) {
    pos = -1; // inizializzo la variabile a un vaore "NULLO"
    
    // uso il parallelismo per eseguire i passi del loop
    #pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < 81; i++) {
        if (sudoku[i] == 0 && pos < 0) {
            #pragma omp critical
            {
                if (pos < 0) {
                    pos = i;
                }
            }
        }
    }
}

bool backtracking(int* sudoku, int num_threads) {
    int pos;
    // Cerco la prima posizione vuota nello schema
    next_pos_vuoto(sudoku, pos, num_threads);
    
    // Controllo se esiste almeno una posizione vuota
    if (pos < 0) {
        return true;
    }
    // mi calcolo la colonna e la riga "virtuale" del vettore
    int col = pos % 9;
    int row = pos / 9;

    // tento di cercare il numero giusto
    // !! Nessun parallelismo poichè ricorsivo
    // !! Su Windows non è presente la direttiva Task
    for (int trynum = 1; trynum < 10; trynum++) {

        // calcolo se il numero del for può essere inserito
        if (check_space(sudoku, trynum, row, col)) {
            sudoku[pos] = trynum;

            // eseguo la ricorsione avendo bloccato la prima posisione vuota
            if (backtracking(sudoku, num_threads))
                return true;

            // se tutte le ricorsioni falliscono azzera la posizione
            sudoku[pos] = 0;
        }

    }
    return false;
}

////////// debug
void _pprint(int* sudoku) {
    for (int i = 0; i < 81; i++) {

        if (i % 9 == 0) {
            std::cout << std::endl;
        }

        std::cout << sudoku[i] << " ";

    }
    std::cout << "\n\n" << std::endl;
}

int main(int argc, char* argv[]) {
    int thread_count;
    if (argc <= 1) {
        std::cout << "Inserire come argomento al programma il numero di thread\n" << argv[0] << " <Num thread>\n\nIl programma continua con 1 thread (dove possibile)\n" << std::endl;
        thread_count = 1;
    }
    else {
        thread_count = atoi(argv[1]);
    }
    

    // costruzione del sudoku
    int sudoku[81] = {
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

    // inizio algoritmo di backtracking
    backtracking(sudoku, thread_count);

    _pprint(sudoku);
    return 0;
}