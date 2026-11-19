#include <stdio.h>

#include "testlib.h"

int main(void) {
    int casos = 0;
    int falhas = 0;

    falhas += suite_estados(&casos);
    falhas += suite_transicoes(&casos);
    falhas += suite_caminhos(&casos);

    printf("\n--------------------------------------------\n");
    printf("Casos executados: %d | Aprovados: %d | Falhas: %d\n", casos, casos - falhas, falhas);
    return falhas == 0 ? 0 : 1;
}
