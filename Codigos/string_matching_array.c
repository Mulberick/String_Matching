#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int MatchingArray(char *T, char *P){
    int n = strlen(T);
    int m = strlen(P);

    for(int i = 0; i <= n - m; i++) {
        int j;
        for(j = 0; j < m; j++) {
            if(T[i + j] != P[j]) {
                break;
            }
        }
        if(j == m) {
            printf("Coincidencia encontrada en la posición: %d\n", i);
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <cadena> <cadena a buscar>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // T = Cadena de texto, P = Cadena patron a buscar
    char *T = argv[1];
    char *P = argv[2];

    MatchingArray(T, P);
    
    return EXIT_SUCCESS;
}