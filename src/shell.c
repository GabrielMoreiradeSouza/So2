#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

// Divide a linha em argumentos
int parse_command(char* line, char** args) {
    int count = 0;
    char* token = strtok(line, " \t\n");
    
    while (token != NULL && count < MAX_ARGS - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t\n");
    }
    
    args[count] = NULL;
    return count;
}

// Executa um comando
int execute_command(char** args) {
    if (args[0] == NULL) {
        return 1;
    }
    
    // Comando interno: exit
    if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "sair") == 0) {
        printf("Encerrando shell...\n");
        exit(0);
    }
    
    // Comando interno: help
    if (strcmp(args[0], "help") == 0 || strcmp(args[0], "ajuda") == 0) {
        printf("\n=== SO-G Shell - Comandos Disponíveis ===\n");
        printf("  ls          - Lista arquivos no sistema virtual\n");
        printf("  cat <arq>   - Exibe conteúdo de um arquivo\n");
        printf("  chat        - Inicia o chat IPC\n");
        printf("  formatar    - Formata o disco virtual\n");
        printf("  help/ajuda  - Exibe esta ajuda\n");
        printf("  exit/sair   - Sai do shell\n\n");
        return 1;
    }
    
    // Para comandos do SO, adiciona o prefixo ./bin/
    char cmd_path[256];
    if (strcmp(args[0], "ls") == 0 ||
        strcmp(args[0], "cat") == 0 ||
        strcmp(args[0], "chat") == 0 ||
        strcmp(args[0], "formatar") == 0) {
        snprintf(cmd_path, sizeof(cmd_path), "./bin/%s", args[0]);
        args[0] = cmd_path;
    }
    
    // Fork e execução
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Erro no fork");
        return 1;
    } else if (pid == 0) {
        // Processo filho
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "Erro: comando '%s' não encontrado\n", args[0]);
            exit(1);
        }
    } else {
        // Processo pai
        int status;
        waitpid(pid, &status, 0);
    }
    
    return 1;
}

int main() {
    char input[MAX_INPUT];
    char* args[MAX_ARGS];
    
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║       Bem-vindo ao SO-G Shell!         ║\n");
    printf("║  Sistema Operacional Simulado em C     ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\nDigite 'help' para ver os comandos disponíveis\n\n");
    
    while (1) {
        printf("SO-G> ");
        fflush(stdout);
        
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            break;
        }
        
        // Remove o newline
        input[strcspn(input, "\n")] = 0;
        
        // Ignora linhas vazias
        if (strlen(input) == 0) {
            continue;
        }
        
        // Parse e executa
        parse_command(input, args);
        execute_command(args);
    }
    
    return 0;
}
