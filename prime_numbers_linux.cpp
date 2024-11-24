#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include <cstring>
using namespace std;


bool estePrim(int numar) {
    if (numar < 2) return false;
    for (int i = 2; i * i <= numar; ++i) {
        if (numar % i == 0) return false;
    }
    return true;
}


void gasestePrime(int start, int end, int pipeScriere) {
    for (int i = start; i <= end; ++i) {
        if (estePrim(i)) {
            write(pipeScriere, &i, sizeof(i));
        }
    }
    int semnalFinal = -1;  
    write(pipeScriere, &semnalFinal, sizeof(semnalFinal));
    close(pipeScriere);
}

int main() {
    const int TOTAL_NUMERE = 10000;
    const int NUMAR_PROCESE = 10;
    const int DIMENSIUNE_INTERVAL = TOTAL_NUMERE / NUMAR_PROCESE;

    int pipeuri[NUMAR_PROCESE][2];
    pid_t procese[NUMAR_PROCESE];

    
    for (int i = 0; i < NUMAR_PROCESE; ++i) {
        if (pipe(pipeuri[i]) == -1) {
            cerr << "Eroare la crearea pipe-ului!" << endl;
            return 1;
        }

        procese[i] = fork();

        if (procese[i] == -1) {
            cerr << "Eroare la crearea procesului!" << endl;
            return 1;
        }

        if (procese[i] == 0) {  // Proces copil
            close(pipeuri[i][0]);  // Inchidem capatul de citire
            int start = i * DIMENSIUNE_INTERVAL + 1;
            int end = start + DIMENSIUNE_INTERVAL - 1;
            gasestePrime(start, end, pipeuri[i][1]);
            return 0;
        }
    }

    
    cout << "Numere prime gasite de fiecare proces:\n";
    vector<int> toatePrime;

    for (int i = 0; i < NUMAR_PROCESE; ++i) {
        close(pipeuri[i][1]);  
        int numarPrim;
        bool arePrime = false;

        cout << "Procesul " << i + 1 << " a gasit: ";
        while (read(pipeuri[i][0], &numarPrim, sizeof(numarPrim)) > 0) {
            if (numarPrim == -1) break;
            cout << numarPrim << " ";
            toatePrime.push_back(numarPrim);
            arePrime = true;
        }

        if (!arePrime) {
            cout << "niciun numar prim.";
        }

        cout << endl;
        close(pipeuri[i][0]);
        waitpid(procese[i], nullptr, 0);  
    }

    
    cout << "\nToate numerele prime gasite:\n";
    for (int prim : toatePrime) {
        cout << prim << " ";
    }
    cout << endl;

    return 0;
}
