#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/fs_driver.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    const char* disk_path = "disco.img";
    
    if (fs_init(disk_path) != 0) {
        fprintf(stderr, "Erro ao inicializar disco\n");
        fprintf(stderr, "Execute './bin/formatar' primeiro\n");
        return 1;
    }
    
    char buffer[BLOCK_SIZE * MAX_FILE_BLOCKS];
    memset(buffer, 0, sizeof(buffer));
    
    int size = fs_read(filename, buffer, sizeof(buffer) - 1);
    
    if (size < 0) {
        fprintf(stderr, "Erro ao ler arquivo '%s'\n", filename);
        fs_close();
        return 1;
    }
    
    buffer[size] = '\0';
    printf("%s", buffer);
    
    fs_close();
    return 0;
}
