# Makefile para SO-G - Sistema Operacional Simulado

CC = gcc
CFLAGS = -Wall -Wextra -I./include -pthread
LDFLAGS = -pthread -lrt

# Diretórios
SRC_DIR = src
BIN_DIR = bin
INC_DIR = include

# Arquivos fonte
FS_DRIVER = $(SRC_DIR)/fs_driver.c
SOURCES = $(SRC_DIR)/meu_so.c $(SRC_DIR)/login.c $(SRC_DIR)/shell.c \
          $(SRC_DIR)/formatar.c $(SRC_DIR)/ls_so.c $(SRC_DIR)/cat_so.c \
          $(SRC_DIR)/chat.c

# Executáveis
EXECUTABLES = $(BIN_DIR)/meu_so $(BIN_DIR)/login $(BIN_DIR)/shell \
              $(BIN_DIR)/formatar $(BIN_DIR)/ls $(BIN_DIR)/cat $(BIN_DIR)/chat

.PHONY: all clean run format test help

all: $(BIN_DIR) $(EXECUTABLES)
	@echo ""
	@echo "╔════════════════════════════════════════╗"
	@echo "║  SO-G compilado com sucesso!           ║"
	@echo "╚════════════════════════════════════════╝"
	@echo ""
	@echo "Próximos passos:"
	@echo "  1. make format  - Formata o disco virtual"
	@echo "  2. make run     - Inicia o sistema"
	@echo ""

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Sistema principal
$(BIN_DIR)/meu_so: $(SRC_DIR)/meu_so.c
	$(CC) $(CFLAGS) -o $@ $<

# Login
$(BIN_DIR)/login: $(SRC_DIR)/login.c $(FS_DRIVER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Shell
$(BIN_DIR)/shell: $(SRC_DIR)/shell.c
	$(CC) $(CFLAGS) -o $@ $<

# Formatador
$(BIN_DIR)/formatar: $(SRC_DIR)/formatar.c $(FS_DRIVER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Comando ls
$(BIN_DIR)/ls: $(SRC_DIR)/ls_so.c $(FS_DRIVER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Comando cat
$(BIN_DIR)/cat: $(SRC_DIR)/cat_so.c $(FS_DRIVER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Chat IPC
$(BIN_DIR)/chat: $(SRC_DIR)/chat.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Formata o disco virtual
format: $(BIN_DIR)/formatar
	@echo "Formatando disco virtual..."
	./$(BIN_DIR)/formatar
	@echo ""

# Executa o sistema
run: all
	@echo "Iniciando SO-G..."
	@echo ""
	./$(BIN_DIR)/meu_so

# Testa os componentes
test: format
	@echo ""
	@echo "=== Testando Sistema de Arquivos ==="
	@echo ""
	@echo "Listando arquivos:"
	./$(BIN_DIR)/ls
	@echo ""
	@echo "Lendo arquivo passwd:"
	./$(BIN_DIR)/cat passwd
	@echo ""
	@echo "Testes concluídos!"
	@echo ""

# Limpeza
clean:
	rm -rf $(BIN_DIR)
	rm -f disco.img
	@echo "Arquivos limpos!"

# Ajuda
help:
	@echo "SO-G - Sistema Operacional Simulado"
	@echo ""
	@echo "Comandos disponíveis:"
	@echo "  make          - Compila todo o sistema"
	@echo "  make format   - Formata o disco virtual"
	@echo "  make run      - Executa o SO-G"
	@echo "  make test     - Testa os componentes"
	@echo "  make clean    - Remove arquivos compilados"
	@echo "  make help     - Mostra esta ajuda"
	@echo ""
