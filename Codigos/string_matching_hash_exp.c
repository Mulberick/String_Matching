#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PRIMO 1000000007L   // Numero primo para el calculo del hash
#define BASE  256           // Cardinalidad del alfabeto (1 byte = 256 valores posibles)

#define MEDIA_EXPERIMENTOS 30

// Experimento 1 y 3: variar n, patron fijo
#define N_INICIO      50000
#define N_FIN         1000000
#define N_INCREMENTO  50000
#define M_FIJO        50

// Experimento 2: variar m, texto fijo
#define N_FIJO        1000000
#define M_INICIO      500
#define M_FIN         50000     // ver nota al final sobre este valor
#define M_INCREMENTO  500

#define RUTA_ALEATORIO    "../Datos/Textos/texto_aleatorio.txt"
#define RUTA_ADVERSARIAL  "../Datos/Textos/texto_adversarial.txt"

long long contador = 0;  // cuenta iteraciones del ciclo principal, igual que en tu ejemplo de Prim

int MatchingHash(char *T, long n, char *P, long m, int modo_debug) {
    if (m == 0 || m > n) {
        fprintf(stderr, "Patron invalido o mas largo que el texto\n");
        return -1;
    }

    long peso = 1;
    for (long i = 0; i < m - 1; i++) {
        peso = (peso * BASE) % PRIMO;
    }
    if (modo_debug) printf(" - Peso h (BASE^%ld mod %ld): %ld\n", m - 1, PRIMO, peso);

    long hashP = 0, hashT = 0;
    for (long i = 0; i < m; i++) {
        hashP = (BASE * hashP + P[i]) % PRIMO;
        hashT = (BASE * hashT + T[i]) % PRIMO;
    }
    if (modo_debug) printf(" - hashP = %ld | hashT inicial = %ld\n\n", hashP, hashT);

    int posicion = -1;
    for (long i = 0; i <= n - m; i++) {
        contador++;
        if (hashT == hashP) {
            if (strncmp(T + i, P, m) == 0) {
                posicion = (int)i;
                if (modo_debug) printf("Coincidencia en posicion: %ld\n", i);
            }
        }
        if (i < n - m) {
            hashT = (BASE * (hashT - T[i] * peso) + T[i + m]) % PRIMO;
            if (hashT < 0) hashT += PRIMO;
        }
    }
    return posicion;
}

// utilidades para los experimentos

char *leerArchivoCompleto(const char *ruta, long *tamano_leido) {
    FILE *f = fopen(ruta, "rb");
    if (!f) {
        fprintf(stderr, "No se pudo abrir %s\n", ruta);
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = (char *)malloc(tam + 1);
    fread(buffer, 1, tam, f);
    buffer[tam] = '\0';
    fclose(f);
    if (tamano_leido) *tamano_leido = tam;
    return buffer;
}

void imprimirCabecera(const char *nombreVariable) {
    printf("%s", nombreVariable);
    for (int i = 1; i <= MEDIA_EXPERIMENTOS; i++) printf(",Exp%d", i);
    printf(",Minimo,Maximo,Iteraciones,Promedio\n");
    fflush(stdout);
}

void correrPunto(long valorVariable, char *T, long n, char *P, long m) {
    double t[MEDIA_EXPERIMENTOS], suma_t = 0, min = 1e9, max = -1;
    
    for (int j = 0; j < MEDIA_EXPERIMENTOS; j++) {
        contador = 0; 
        double start = (double)clock() / CLOCKS_PER_SEC;
        MatchingHash(T, n, P, m, 0);
        double end = (double)clock() / CLOCKS_PER_SEC;
        double time = end - start;
        t[j] = time;
        suma_t += time;
        if (time < min) min = time;
        if (time > max) max = time;
    }
    double prom_t = (suma_t - min - max) / (MEDIA_EXPERIMENTOS - 2);

    printf("%ld", valorVariable);
    for (int j = 0; j < MEDIA_EXPERIMENTOS; j++) printf(",%.8f", t[j]);
    printf(",%.8f,%.8f,%lld,%.8f\n", min, max, contador, prom_t);
    fflush(stdout);
}

// los 3 experimentos

void experimento1_variar_n(void) {
    long tam_total;
    char *texto = leerArchivoCompleto(RUTA_ALEATORIO, &tam_total);
    char *patron = (char *)malloc(M_FIJO + 1);
    memcpy(patron, texto + (tam_total - M_FIJO), M_FIJO);  // patron tomado del final
    patron[M_FIJO] = '\0';

    imprimirCabecera("n");
    for (long n = N_INICIO; n <= N_FIN && n <= tam_total; n += N_INCREMENTO) {
        char *T = (char *)malloc(n + 1);
        memcpy(T, texto, n);
        T[n] = '\0';
        correrPunto(n, T, n, patron, M_FIJO);
        free(T);
    }
    free(texto);
    free(patron);
}

void experimento2_variar_m(void) {
    long tam_total;
    char *texto = leerArchivoCompleto(RUTA_ALEATORIO, &tam_total);
    long n = (N_FIJO < tam_total) ? N_FIJO : tam_total;

    imprimirCabecera("m");
    for (long m = M_INICIO; m <= M_FIN && m <= n; m += M_INCREMENTO) {
        char *patron = (char *)malloc(m + 1);
        memcpy(patron, texto + (n - m), m);  // patron = ultimos m caracteres 
        patron[m] = '\0';
        correrPunto(m, texto, n, patron, m);
        free(patron);
    }
    free(texto);
}

void experimento3_peor_caso(void) {
    long tam_total;
    char *texto = leerArchivoCompleto(RUTA_ADVERSARIAL, &tam_total);  // "aaaa...ab"
    char *patron = (char *)malloc(M_FIJO + 1);
    memcpy(patron, texto + (tam_total - M_FIJO), M_FIJO);  // "aaa...ab" de tamaño M_FIJO
    patron[M_FIJO] = '\0';

    imprimirCabecera("n");
    for (long n = N_INICIO; n <= N_FIN && n <= tam_total; n += N_INCREMENTO) {
        char *T = (char *)malloc(n + 1);
        memcpy(T, texto, n);
        T[n] = '\0';
        correrPunto(n, T, n, patron, M_FIJO);
        free(T);
    }
    free(texto);
    free(patron);
}

int main(int argc, char **argv) {
    if (argc == 2 && (argv[1][0] == '1' || argv[1][0] == '2' || argv[1][0] == '3')) {
        switch (argv[1][0]) {
            case '1': experimento1_variar_n(); break;   // caso promedio, variando n
            case '2': experimento2_variar_m(); break;   // caso promedio, variando m
            case '3': experimento3_peor_caso(); break;  // peor caso, variando n
        }
    } else if (argc == 3) {
        // Modo manual original, con detalle paso a paso
        MatchingHash(argv[1], strlen(argv[1]), argv[2], strlen(argv[2]), 1);
    } else {
        fprintf(stderr, "Uso:\n");
        fprintf(stderr, "  %s <1|2|3>          -> ejecutar experimento 1, 2 o 3\n", argv[0]);
        fprintf(stderr, "  %s <texto> <patron> -> modo manual con detalle paso a paso\n", argv[0]);
        return EXIT_FAILURE;
    }
    return 0;
}