CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -g -O0
SRC     = src/pedido.c
TESTS   = tests/main.c tests/testlib.c tests/helpers.c \
          tests/test_estados.c tests/test_transicoes.c tests/test_caminhos.c

.PHONY: test cov clean

# Compila e executa todos os testes automatizados (retorna != 0 se algum falhar)
test:
	@mkdir -p build
	$(CC) $(CFLAGS) $(SRC) $(TESTS) -o build/testes
	./build/testes

# Mesmos testes, com relatório de cobertura (linhas e ramos) do módulo src/pedido.c
cov:
	@mkdir -p build/cov
	$(CC) $(CFLAGS) --coverage $(SRC) $(TESTS) -o build/cov/testes
	cd build/cov && ./testes | tail -3
	gcov -b -c -o build/cov build/cov/testes-pedido.gcda | sed -n '1,5p'
	@mv -f pedido.c.gcov build/cov/ 2>/dev/null || true

clean:
	rm -rf build
