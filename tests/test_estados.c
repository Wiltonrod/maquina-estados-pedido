/* Testes de ESTADOS: cada um dos 7 estados é alcançável e se comporta como definido. */

#include <stdio.h>
#include <string.h>

#include "helpers.h"
#include "testlib.h"

static const char *MENSAGENS[NUM_ESTADOS] = {
    "Aguardando pagamento",
    "Pagamento confirmado",
    "Em preparo na cozinha",
    "Pronto, aguardando o entregador",
    "Saiu para entrega",
    "Pedido entregue",
    "Pedido cancelado",
};

/* Quantos eventos são aceitos (RESULT_OK) a partir do estado s. */
static int saidas_validas(Estado s) {
    int n = 0;
    for (int e = 0; e < NUM_EVENTOS; e++) {
        Pedido p = pedido_em(s);
        if (pedido_processa_evento(&p, (Evento)e) == RESULT_OK) {
            n++;
        }
    }
    return n;
}

int suite_estados(int *casos) {
    Suite s;
    char id[96];
    suite_inicia(&s, "Testes de ESTADOS");

    /* Estado inicial */
    Pedido novo;
    pedido_inicializa(&novo);
    caso(&s, "estado_inicial_e_CRIADO",
         novo.estado == CRIADO && novo.tam_historico == 1 && novo.historico[0] == CRIADO &&
             !pedido_finalizado(&novo));

    /* Cobertura de estados: os 7 são alcançados a partir do inicial */
    for (int i = 0; i < NUM_ESTADOS; i++) {
        Pedido p = pedido_em((Estado)i);
        snprintf(id, sizeof id, "estado_alcancavel_%s", estado_nome((Estado)i));
        caso(&s, id, p.estado == (Estado)i && p.historico[p.tam_historico - 1] == (Estado)i);
    }

    /* Flag "finalizado" só em estados finais */
    for (int i = 0; i < NUM_ESTADOS; i++) {
        Pedido p = pedido_em((Estado)i);
        int final = (i == ENTREGUE || i == CANCELADO);
        snprintf(id, sizeof id, "finalizado_%s_e_%s", estado_nome((Estado)i),
                 final ? "verdadeiro" : "falso");
        caso(&s, id, pedido_finalizado(&p) == final);
    }

    /* Estados finais não têm saída */
    caso(&s, "ENTREGUE_nao_possui_saida", saidas_validas(ENTREGUE) == 0);
    caso(&s, "CANCELADO_nao_possui_saida", saidas_validas(CANCELADO) == 0);

    /* Estados não finais têm ao menos uma saída */
    for (int i = 0; i < NUM_ESTADOS; i++) {
        if (i == ENTREGUE || i == CANCELADO) {
            continue;
        }
        snprintf(id, sizeof id, "%s_possui_ao_menos_uma_saida", estado_nome((Estado)i));
        caso(&s, id, saidas_validas((Estado)i) >= 1);
    }

    /* Somente CRIADO não é destino de nenhuma transição */
    int tem_entrada[NUM_ESTADOS] = {0};
    for (int i = 0; i < NUM_ESTADOS; i++) {
        for (int e = 0; e < NUM_EVENTOS; e++) {
            Pedido p = pedido_em((Estado)i);
            if (pedido_processa_evento(&p, (Evento)e) == RESULT_OK) {
                tem_entrada[p.estado] = 1;
            }
        }
    }
    int sem_entrada = 0;
    for (int i = 0; i < NUM_ESTADOS; i++) {
        if (!tem_entrada[i] && i != CRIADO) {
            sem_entrada++;
        }
    }
    caso(&s, "somente_CRIADO_nao_tem_entrada", !tem_entrada[CRIADO] && sem_entrada == 0);

    /* Pedidos não compartilham estado */
    Pedido a, b;
    pedido_inicializa(&a);
    pedido_inicializa(&b);
    pedido_processa_evento(&a, EV_PAGAR);
    caso(&s, "pedidos_sao_independentes", a.estado == PAGO && b.estado == CRIADO);

    /* Saída estilo Moore: mensagem depende só do estado */
    for (int i = 0; i < NUM_ESTADOS; i++) {
        snprintf(id, sizeof id, "mensagem_moore_%s", estado_nome((Estado)i));
        caso(&s, id, strcmp(estado_mensagem_cliente((Estado)i), MENSAGENS[i]) == 0);
    }
    caso(&s, "mensagem_moore_estado_invalido",
         strcmp(estado_mensagem_cliente(NUM_ESTADOS), "Estado desconhecido") == 0);

    /* Nomes legíveis (inclui valores fora do intervalo) */
    caso(&s, "estado_nome_valido_e_invalido",
         strcmp(estado_nome(PRONTO), "PRONTO") == 0 && strcmp(estado_nome(NUM_ESTADOS), "?") == 0 &&
             strcmp(estado_nome((Estado)-1), "?") == 0);
    caso(&s, "evento_nome_valido_e_invalido",
         strcmp(evento_nome(EV_CANCELAR), "cancelar") == 0 &&
             strcmp(evento_nome(NUM_EVENTOS), "?") == 0 && strcmp(evento_nome((Evento)-1), "?") == 0);

    *casos += s.total;
    return s.falhas;
}
