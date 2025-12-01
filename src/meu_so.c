#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║            SO-G Simulator              ║\n");
    printf("║   Sistema Operacional em Espaço de     ║\n");
    printf("║          Usuário - Versão 1.0          ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");
    printf("Inicializando sistema...\n\n");
    
    // Executa o processo de login
    char* args[] = {"./bin/login", NULL};
    execvp(args[0], args);
    
    // Se chegou aqui, execvp falhou
    perror("Erro ao iniciar processo de login");
    printf("\nVerifique se o sistema foi compilado corretamente.\n");
    printf("Execute: make\n");
    
    return 1;
}
