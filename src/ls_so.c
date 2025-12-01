#include <stdio.h>
#include <stdlib.h>
#include "../include/fs_driver.h"

int main(int argc, char* argv[]) {
    const char* disk_path = "disco.img";
    
    if (fs_init(disk_path) != 0) {
        fprintf(stderr, "Erro ao inicializar disco\n");
        fprintf(stderr, "Execute './bin/formatar' primeiro\n");
        return 1;
    }
    
    char* filenames[MAX_FILES];
    int count = fs_list(filenames, MAX_FILES);
    
    if (count == 0) {
        printf("Nenhum arquivo encontrado\n");
    } else {
        printf("Arquivos em disco.img:\n");
        for (int i = 0; i < count; i++) {
            printf("  %s\n", filenames[i]);
            free(filenames[i]);
        }
    }
    
    fs_close();
    return 0;
}
