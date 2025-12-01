#ifndef FS_DRIVER_H
#define FS_DRIVER_H

#include <stdint.h>
#include <stdio.h>

// Constantes do sistema de arquivos
#define BLOCK_SIZE 512
#define MAX_FILES 64
#define MAX_FILENAME 32
#define MAX_BLOCKS 1024
#define MAGIC_NUMBER 0x534F4746  // "SOFG"
#define MAX_FILE_BLOCKS 16

// Estrutura do Superbloco (Block 0)
typedef struct {
    uint32_t magic;           // Número mágico para identificar o FS
    uint32_t total_blocks;    // Total de blocos no disco
    uint32_t block_size;      // Tamanho de cada bloco
    uint32_t num_inodes;      // Número de inodes
    uint32_t inode_blocks;    // Blocos usados para inodes
    uint32_t data_start;      // Primeiro bloco de dados
} Superblock;

// Estrutura do Inode
typedef struct {
    char name[MAX_FILENAME];  // Nome do arquivo
    uint32_t size;            // Tamanho em bytes
    uint32_t used;            // 1 se usado, 0 se livre
    uint32_t blocks[MAX_FILE_BLOCKS]; // Blocos de dados
} Inode;

// Funções do driver
int fs_init(const char* disk_path);
int fs_format();
int fs_create(const char* filename);
int fs_write(const char* filename, const char* data, uint32_t size);
int fs_read(const char* filename, char* buffer, uint32_t max_size);
int fs_list(char** filenames, int max_count);
int fs_delete(const char* filename);
void fs_close();

// Funções auxiliares
int fs_find_inode(const char* filename);
int fs_find_free_inode();
int fs_find_free_block();

#endif // FS_DRIVER_H
