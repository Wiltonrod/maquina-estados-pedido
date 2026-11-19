/*
 * Testes de CAMINHOS: os 4 caminhos independentes do estado inicial até um estado final.
 *
 * Complexidade ciclomática do grafo (com nó de fim virtual ligado aos dois finais):
 *   V(G) = E - N + 2 = 10 - 8 + 2 = 4.
 * O grafo não tem ciclos, logo esses 4 caminhos são todos os caminhos possíveis.
 */

#include <stdio.h>
#include <string.h>

#include "helpers.h"
#include "testlib.h"

#define MAX_EVENTOS_CAMINHO 5

typedef struct {
    const char *nome;
    int n_eventos;
    Evento eventos[MAX_EVENTOS_CAMINHO];
    int n_estados;
    Estado estados[MAX_HISTORICO];
} CaminhoDoc;

static const CaminhoDoc CAMINHOS[4] = {
    {"CP1_feliz", 5,
     {EV_PAGAR, EV_INICIAR_PREPARO, EV_FINALIZAR_PREPARO, EV_DESPACHAR, EV_CONFIRMAR_ENTREGA},
     6, {CRIADO, PAGO, EM_PREPARO, PRONTO, EM_ENTREGA, ENTREGUE}},
    {"CP2_cancelado_sem_pagar", 1, {EV_CANCELAR}, 2, {CRIADO, CANCELADO}},
    {"CP3_cancelado_apos_pagar", 2, {EV_PAGAR, EV_CANCELAR}, 3, {CRIADO, PAGO, CANCELADO}},
    {"CP4_cancelado_em_preparo", 3, {EV_PAGAR, EV_INICIAR_PREPARO, EV_CANCELAR},
     4, {CRIADO, PAGO, EM_PREPARO, CANCELADO}},
};

/* ---- Enumeração de todos os caminhos por DFS com backtracking ---- */
#define MAX_ACHADOS 16
static Pedido achados[MAX_ACHADOS];
static int n_achados;
static int estourou_profundidade; /* 1 se algum caminho passar de MAX_HISTORICO (indício de ciclo) */

static void dfs(Pedido p) {
    if (pedido_finalizado(&p)) {
        if (n_achados < MAX_ACHADOS) {
            achados[n_achados] = p;
        }
        n_achados++;
        return;
    }
    if (p.tam_historico >= MAX_HISTORICO) {
        estourou_profundidade = 1;
        return;
    }
    for (int e = 0; e < NUM_EVENTOS; e++) {
        Pedido q = p; /* cópia = backtracking */
        if (pedido_processa_evento(&q, (Evento)e) == RESULT_OK) {
            dfs(q);
        }
    }
}

static int mesmo_caminho(const Pedido *p, const CaminhoDoc *c) {
    if (p->tam_historico != c->n_estados) {
        return 0;
    }
    for (int i = 0; i < c->n_estados; i++) {
        if (p->historico[i] != c->estados[i]) {
            return 0;
        }
    }
    return 1;
}

int suite_caminhos(int *casos) {
    Suite s;
    char id[128];
    suite_inicia(&s, "Testes de CAMINHOS");

    /* CP1..CP4: executa os eventos e compara a sequência de estados */
    int transicao_usada[NUM_ESTADOS][NUM_EVENTOS] = {{0}};
    for (int c = 0; c < 4; c++) {
        Pedido p;
        int ok = 1;
        pedido_inicializa(&p);
        for (int i = 0; i < CAMINHOS[c].n_eventos; i++) {
            Estado origem = p.estado;
            ok &= (pedido_processa_evento(&p, CAMINHOS[c].eventos[i]) == RESULT_OK);
            transicao_usada[origem][CAMINHOS[c].eventos[i]] = 1;
        }
        caso(&s, CAMINHOS[c].nome, ok && mesmo_caminho(&p, &CAMINHOS[c]) && pedido_finalizado(&p));
    }

    /* Juntos, os 4 caminhos usam as 8 transições da tabela */
    int usadas = 0, esperadas_ok = 1;
    for (int i = 0; i < NUM_ESTADOS; i++) {
        for (int e = 0; e < NUM_EVENTOS; e++) {
            usadas += transicao_usada[i][e];
            esperadas_ok &= (transicao_usada[i][e] == (destino_esperado((Estado)i, (Evento)e) >= 0));
        }
    }
    caso(&s, "caminhos_cobrem_as_8_transicoes", usadas == 8 && esperadas_ok);

    /* DFS: existem exatamente 4 caminhos e são os documentados */
    Pedido inicial;
    pedido_inicializa(&inicial);
    n_achados = 0;
    estourou_profundidade = 0;
    dfs(inicial);
    caso(&s, "dfs_encontra_exatamente_4_caminhos", n_achados == 4);

    int todos_documentados = 1;
    for (int c = 0; c < 4; c++) {
        int achou = 0;
        for (int a = 0; a < n_achados && a < MAX_ACHADOS; a++) {
            achou |= mesmo_caminho(&achados[a], &CAMINHOS[c]);
        }
        todos_documentados &= achou;
    }
    caso(&s, "dfs_caminhos_sao_os_documentados", todos_documentados);
    caso(&s, "grafo_e_aciclico", !estourou_profundidade);

    /* V(G) = E - N + 2, calculado sondando a implementação */
    int arestas = 0, finais = 0;
    for (int i = 0; i < NUM_ESTADOS; i++) {
        int saidas = 0;
        for (int e = 0; e < NUM_EVENTOS; e++) {
            Pedido p = pedido_em((Estado)i);
            saidas += (pedido_processa_evento(&p, (Evento)e) == RESULT_OK);
        }
        arestas += saidas;
        finais += (saidas == 0);
    }
    int E = arestas + finais;      /* + uma aresta de cada estado final ao nó de fim */
    int N = NUM_ESTADOS + 1;       /* + nó de fim virtual */
    snprintf(id, sizeof id, "complexidade_ciclomatica_E%d_N%d_igual_a_4", E, N);
    caso(&s, id, E - N + 2 == 4);

    /* Nenhum caminho continua depois de um estado final */
    for (int f = 0; f < 2; f++) {
        Estado final = (f == 0) ? ENTREGUE : CANCELADO;
        for (int e = 0; e < NUM_EVENTOS; e++) {
            Pedido p = pedido_em(final);
            Resultado r = pedido_processa_evento(&p, (Evento)e);
            snprintf(id, sizeof id, "final_%s_rejeita_%s", estado_nome(final), evento_nome((Evento)e));
            caso(&s, id, r == RESULT_TRANSICAO_INVALIDA && p.estado == final);
        }
    }

    *casos += s.total;
    return s.falhas;
}
