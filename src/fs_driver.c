#include "../include/fs_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static FILE* disk = NULL;
static Superblock sb;
static uint8_t bitmap[MAX_BLOCKS];
static Inode inodes[MAX_FILES];

// Inicializa o sistema de arquivos abrindo o disco
int fs_init(const char* disk_path) {
    disk = fopen(disk_path, "r+b");
    if (!disk) {
        disk = fopen(disk_path, "w+b");
        if (!disk) {
            perror("Erro ao abrir disco");
            return -1;
        }
    }
    
    // Lê o superbloco
    fseek(disk, 0, SEEK_SET);
    if (fread(&sb, sizeof(Superblock), 1, disk) != 1) {
        // Disco não formatado
        memset(&sb, 0, sizeof(Superblock));
        return 0;
    }
    
    // Verifica magic number
    if (sb.magic != MAGIC_NUMBER) {
        // Disco não formatado
        memset(&sb, 0, sizeof(Superblock));
        return 0;
    }
    
    // Lê bitmap
    fseek(disk, BLOCK_SIZE, SEEK_SET);
    fread(bitmap, 1, MAX_BLOCKS, disk);
    
    // Lê inodes
    fseek(disk, BLOCK_SIZE * 2, SEEK_SET);
    fread(inodes, sizeof(Inode), MAX_FILES, disk);
    
    return 0;
}

// Formata o disco com um sistema de arquivos limpo
int fs_format() {
    if (!disk) return -1;
    
    // Cria o superbloco
    sb.magic = MAGIC_NUMBER;
    sb.total_blocks = MAX_BLOCKS;
    sb.block_size = BLOCK_SIZE;
    sb.num_inodes = MAX_FILES;
    sb.inode_blocks = (MAX_FILES * sizeof(Inode)) / BLOCK_SIZE + 1;
    sb.data_start = 2 + sb.inode_blocks;
    
    // Limpa bitmap
    memset(bitmap, 0, MAX_BLOCKS);
    // Marca blocos do sistema como usados
    bitmap[0] = 1; // Superbloco
    bitmap[1] = 1; // Bitmap
    for (int i = 2; i < sb.data_start; i++) {
        bitmap[i] = 1; // Inodes
    }
    
    // Limpa inodes
    memset(inodes, 0, sizeof(inodes));
    
    // Escreve o superbloco
    fseek(disk, 0, SEEK_SET);
    fwrite(&sb, sizeof(Superblock), 1, disk);
    
    // Escreve bitmap
    fseek(disk, BLOCK_SIZE, SEEK_SET);
    fwrite(bitmap, 1, MAX_BLOCKS, disk);
    
    // Escreve inodes
    fseek(disk, BLOCK_SIZE * 2, SEEK_SET);
    fwrite(inodes, sizeof(Inode), MAX_FILES, disk);
    
    // Cria arquivo /etc/passwd padrão
    const char* default_passwd = "admin:senha123\nuser:user123\n";
    fs_create("passwd");
    fs_write("passwd", default_passwd, strlen(default_passwd));
    
    fflush(disk);
    printf("Disco formatado com sucesso!\n");
    printf("Sistema de arquivos SO-G inicializado.\n");
    printf("Blocos totais: %d\n", sb.total_blocks);
    printf("Tamanho do bloco: %d bytes\n", sb.block_size);
    printf("Inodes disponíveis: %d\n", sb.num_inodes);
    
    return 0;
}

// Encontra um inode pelo nome do arquivo
int fs_find_inode(const char* filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (inodes[i].used && strcmp(inodes[i].name, filename) == 0) {
            return i;
        }
    }
    return -1;
}

// Encontra um inode livre
int fs_find_free_inode() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!inodes[i].used) {
            return i;
        }
    }
    return -1;
}

// Encontra um bloco livre
int fs_find_free_block() {
    for (int i = sb.data_start; i < MAX_BLOCKS; i++) {
        if (!bitmap[i]) {
            return i;
        }
    }
    return -1;
}

// Cria um novo arquivo
int fs_create(const char* filename) {
    if (!disk) return -1;
    
    // Verifica se já existe
    if (fs_find_inode(filename) != -1) {
        fprintf(stderr, "Arquivo '%s' já existe\n", filename);
        return -1;
    }
    
    // Encontra inode livre
    int inode_idx = fs_find_free_inode();
    if (inode_idx == -1) {
        fprintf(stderr, "Sem inodes disponíveis\n");
        return -1;
    }
    
    // Inicializa o inode
    strncpy(inodes[inode_idx].name, filename, MAX_FILENAME - 1);
    inodes[inode_idx].name[MAX_FILENAME - 1] = '\0';
    inodes[inode_idx].size = 0;
    inodes[inode_idx].used = 1;
    memset(inodes[inode_idx].blocks, 0, sizeof(inodes[inode_idx].blocks));
    
    // Salva inodes
    fseek(disk, BLOCK_SIZE * 2, SEEK_SET);
    fwrite(inodes, sizeof(Inode), MAX_FILES, disk);
    fflush(disk);
    
    return 0;
}

// Escreve dados em um arquivo
int fs_write(const char* filename, const char* data, uint32_t size) {
    if (!disk) return -1;
    
    int inode_idx = fs_find_inode(filename);
    if (inode_idx == -1) {
        fprintf(stderr, "Arquivo '%s' não encontrado\n", filename);
        return -1;
    }
    
    // Calcula quantos blocos são necessários
    int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (blocks_needed > MAX_FILE_BLOCKS) {
        fprintf(stderr, "Arquivo muito grande\n");
        return -1;
    }
    
    // Aloca blocos
    for (int i = 0; i < blocks_needed; i++) {
        if (i < MAX_FILE_BLOCKS && inodes[inode_idx].blocks[i] == 0) {
            int block = fs_find_free_block();
            if (block == -1) {
                fprintf(stderr, "Sem blocos disponíveis\n");
                return -1;
            }
            inodes[inode_idx].blocks[i] = block;
            bitmap[block] = 1;
        }
    }
    
    // Escreve os dados
    for (int i = 0; i < blocks_needed; i++) {
        int block = inodes[inode_idx].blocks[i];
        int offset = i * BLOCK_SIZE;
        int write_size = (size - offset > BLOCK_SIZE) ? BLOCK_SIZE : (size - offset);
        
        fseek(disk, block * BLOCK_SIZE, SEEK_SET);
        fwrite(data + offset, 1, write_size, disk);
    }
    
    inodes[inode_idx].size = size;
    
    // Salva bitmap e inodes
    fseek(disk, BLOCK_SIZE, SEEK_SET);
    fwrite(bitmap, 1, MAX_BLOCKS, disk);
    
    fseek(disk, BLOCK_SIZE * 2, SEEK_SET);
    fwrite(inodes, sizeof(Inode), MAX_FILES, disk);
    
    fflush(disk);
    return 0;
}

// Lê dados de um arquivo
int fs_read(const char* filename, char* buffer, uint32_t max_size) {
    if (!disk) return -1;
    
    int inode_idx = fs_find_inode(filename);
    if (inode_idx == -1) {
        fprintf(stderr, "Arquivo '%s' não encontrado\n", filename);
        return -1;
    }
    
    uint32_t size = inodes[inode_idx].size;
    if (size > max_size) size = max_size;
    
    int blocks_to_read = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    for (int i = 0; i < blocks_to_read; i++) {
        int block = inodes[inode_idx].blocks[i];
        int offset = i * BLOCK_SIZE;
        int read_size = (size - offset > BLOCK_SIZE) ? BLOCK_SIZE : (size - offset);
        
        fseek(disk, block * BLOCK_SIZE, SEEK_SET);
        fread(buffer + offset, 1, read_size, disk);
    }
    
    return size;
}

// Lista todos os arquivos
int fs_list(char** filenames, int max_count) {
    int count = 0;
    for (int i = 0; i < MAX_FILES && count < max_count; i++) {
        if (inodes[i].used) {
            filenames[count] = strdup(inodes[i].name);
            count++;
        }
    }
    return count;
}

// Remove um arquivo
int fs_delete(const char* filename) {
    if (!disk) return -1;
    
    int inode_idx = fs_find_inode(filename);
    if (inode_idx == -1) {
        fprintf(stderr, "Arquivo '%s' não encontrado\n", filename);
        return -1;
    }
    
    // Libera blocos
    for (int i = 0; i < MAX_FILE_BLOCKS; i++) {
        if (inodes[inode_idx].blocks[i] != 0) {
            bitmap[inodes[inode_idx].blocks[i]] = 0;
        }
    }
    
    // Marca inode como livre
    memset(&inodes[inode_idx], 0, sizeof(Inode));
    
    // Salva bitmap e inodes
    fseek(disk, BLOCK_SIZE, SEEK_SET);
    fwrite(bitmap, 1, MAX_BLOCKS, disk);
    
    fseek(disk, BLOCK_SIZE * 2, SEEK_SET);
    fwrite(inodes, sizeof(Inode), MAX_FILES, disk);
    
    fflush(disk);
    return 0;
}

// Fecha o sistema de arquivos
void fs_close() {
    if (disk) {
        fclose(disk);
        disk = NULL;
    }
}
