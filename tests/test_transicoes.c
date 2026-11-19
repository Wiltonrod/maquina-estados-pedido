/* Testes de TRANSIÇÕES: as 8 válidas e as 34 inválidas (7 estados x 6 eventos = 42). */

#include <stdio.h>
#include <string.h>

#include "helpers.h"
#include "testlib.h"

int suite_transicoes(int *casos) {
    Suite s;
    char id[128];
    suite_inicia(&s, "Testes de TRANSICOES");

    /* 8 + 34 = 42 combinações */
    int validas = 0, invalidas = 0;
    for (int i = 0; i < NUM_ESTADOS; i++) {
        for (int e = 0; e < NUM_EVENTOS; e++) {
            if (destino_esperado((Estado)i, (Evento)e) >= 0) {
                validas++;
            } else {
                invalidas++;
            }
        }
    }
    caso(&s, "tabela_tem_8_validas_e_34_invalidas",
         validas == 8 && invalidas == 34 && validas + invalidas == NUM_ESTADOS * NUM_EVENTOS);

    /* As 8 transições válidas (T1..T8) */
    for (int t = 0; t < NUM_VALIDAS; t++) {
        Pedido p = pedido_em(VALIDAS[t].origem);
        Resultado r = pedido_processa_evento(&p, VALIDAS[t].evento);
        snprintf(id, sizeof id, "valida_T%d_%s_%s_%s", t + 1, estado_nome(VALIDAS[t].origem),
                 evento_nome(VALIDAS[t].evento), estado_nome(VALIDAS[t].destino));
        caso(&s, id,
             r == RESULT_OK && p.estado == VALIDAS[t].destino && p.tam_historico >= 2 &&
                 p.historico[p.tam_historico - 2] == VALIDAS[t].origem &&
                 p.historico[p.tam_historico - 1] == VALIDAS[t].destino);
    }

    /* As 34 transições inválidas: devolvem erro e NÃO alteram o pedido */
    for (int i = 0; i < NUM_ESTADOS; i++) {
        for (int e = 0; e < NUM_EVENTOS; e++) {
            if (destino_esperado((Estado)i, (Evento)e) >= 0) {
                continue;
            }
            Pedido p = pedido_em((Estado)i);
            Pedido antes = p;
            Resultado r = pedido_processa_evento(&p, (Evento)e);
            snprintf(id, sizeof id, "invalida_%s_%s", estado_nome((Estado)i),
                     evento_nome((Evento)e));
            caso(&s, id, r == RESULT_TRANSICAO_INVALIDA && memcmp(&p, &antes, sizeof p) == 0);
        }
    }

    *casos += s.total;
    return s.falhas;
}
