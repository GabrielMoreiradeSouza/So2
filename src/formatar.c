#include <stdio.h>
#include <stdlib.h>
#include "../include/fs_driver.h"

int main(int argc, char* argv[]) {
    const char* disk_path = "disco.img";
    
    if (argc > 1) {
        disk_path = argv[1];
    }
    
    printf("=== Formatador SO-G ===\n");
    printf("Formatando disco: %s\n\n", disk_path);
    
    if (fs_init(disk_path) != 0) {
        fprintf(stderr, "Erro ao inicializar disco\n");
        return 1;
    }
    
    if (fs_format() != 0) {
        fprintf(stderr, "Erro ao formatar disco\n");
        fs_close();
        return 1;
    }
    
    printf("\nFormatação concluída!\n");
    printf("Arquivo /etc/passwd criado com usuários padrão:\n");
    printf("  - admin:senha123\n");
    printf("  - user:user123\n");
    
    fs_close();
    return 0;
}
