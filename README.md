# Máquina de estados — Ciclo de vida de um pedido de delivery

Implementa uma máquina de estados simples em C e a valida com testes 100% automatizados
de estados, transições e caminhos.

## Estrutura

```
src/pedido.h, src/pedido.c      # Estados, eventos e a função de transição
tests/test_estados.c            # Testes de estados
tests/test_transicoes.c         # Testes de transições (8 válidas + 34 inválidas)
tests/test_caminhos.c           # Testes dos 4 caminhos independentes
tests/helpers.*, testlib.*      # Oráculo da tabela de transições e mini biblioteca de testes
docs/diagrama.dot               # Fonte do diagrama de estados (Graphviz)
Makefile
```

## Como executar (requer gcc e make)

```bash
make test   # compila e roda todos os testes (código de saída != 0 se algum falhar)
make cov    # roda os testes com cobertura (gcov): linhas e ramos de src/pedido.c
```

Resultado esperado: 98 casos aprovados; 100% das linhas e 100% dos ramos.
