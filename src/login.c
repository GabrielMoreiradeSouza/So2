#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/fs_driver.h"

#define MAX_USERNAME 32
#define MAX_PASSWORD 32

int authenticate(const char* username, const char* password) {
    const char* disk_path = "disco.img";
    
    if (fs_init(disk_path) != 0) {
        fprintf(stderr, "Erro ao inicializar disco\n");
        fprintf(stderr, "Execute './bin/formatar' primeiro\n");
        return 0;
    }
    
    char buffer[BLOCK_SIZE * MAX_FILE_BLOCKS];
    memset(buffer, 0, sizeof(buffer));
    
    int size = fs_read("passwd", buffer, sizeof(buffer) - 1);
    
    if (size < 0) {
        fprintf(stderr, "Erro: arquivo /etc/passwd não encontrado\n");
        fs_close();
        return 0;
    }
    
    buffer[size] = '\0';
    
    // Parse do arquivo passwd (formato: usuario:senha)
    char* line = strtok(buffer, "\n");
    while (line != NULL) {
        char* user = strtok(line, ":");
        char* pass = strtok(NULL, ":");
        
        if (user && pass) {
            if (strcmp(user, username) == 0 && strcmp(pass, password) == 0) {
                fs_close();
                return 1;
            }
        }
        
        line = strtok(NULL, "\n");
    }
    
    fs_close();
    return 0;
}

int main() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int attempts = 0;
    
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║           SO-G Login System            ║\n");
    printf("║     Sistema Operacional Simulado       ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");
    
    while (attempts < 3) {
        printf("Usuário: ");
        fflush(stdout);
        
        if (fgets(username, MAX_USERNAME, stdin) == NULL) {
            break;
        }
        username[strcspn(username, "\n")] = 0;
        
        printf("Senha: ");
        fflush(stdout);
        
        if (fgets(password, MAX_PASSWORD, stdin) == NULL) {
            break;
        }
        password[strcspn(password, "\n")] = 0;
        
        if (authenticate(username, password)) {
            printf("\n✓ Login bem-sucedido! Bem-vindo, %s!\n", username);
            printf("Iniciando shell...\n\n");
            
            // Substitui este processo pelo shell
            char* args[] = {"./bin/shell", NULL};
            execvp(args[0], args);
            
            // Se chegou aqui, execvp falhou
            perror("Erro ao iniciar shell");
            return 1;
        } else {
            attempts++;
            printf("\n✗ Usuário ou senha incorretos. ");
            printf("Tentativas restantes: %d\n\n", 3 - attempts);
        }
    }
    
    printf("Número máximo de tentativas excedido.\n");
    return 1;
}
