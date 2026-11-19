#include "testlib.h"

void suite_inicia(Suite *s, const char *nome) {
    s->nome = nome;
    s->total = 0;
    s->falhas = 0;
    printf("\n=== %s ===\n", nome);
}

void caso(Suite *s, const char *id, int condicao) {
    s->total++;
    if (!condicao) {
        s->falhas++;
    }
    printf("  [%s] %s\n", condicao ? "OK" : "FALHOU", id);
}
