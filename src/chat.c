#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

#define SHM_NAME "/meu_chat_shm"
#define SEM_NAME "/meu_chat_sem"
#define MAX_MESSAGES 10
#define MAX_MSG_LEN 256

typedef struct {
    char messages[MAX_MESSAGES][MAX_MSG_LEN];
    int count;
} ChatMemory;

static ChatMemory* chat_mem = NULL;
static sem_t* sem = NULL;
static int shm_fd = -1;
static volatile int running = 1;

void cleanup() {
    if (chat_mem != NULL) {
        munmap(chat_mem, sizeof(ChatMemory));
    }
    if (sem != NULL) {
        sem_close(sem);
    }
    if (shm_fd >= 0) {
        close(shm_fd);
    }
}

void signal_handler(int sig) {
    running = 0;
}

void* receiver_thread(void* arg) {
    int last_count = 0;
    
    while (running) {
        sem_wait(sem);
        
        if (chat_mem->count > last_count) {
            for (int i = last_count; i < chat_mem->count; i++) {
                printf("\n[Mensagem recebida]: %s\n", chat_mem->messages[i % MAX_MESSAGES]);
                printf("Você> ");
                fflush(stdout);
            }
            last_count = chat_mem->count;
        }
        
        sem_post(sem);
        
        usleep(100000); // 100ms
    }
    
    return NULL;
}

int main() {
    signal(SIGINT, signal_handler);
    
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║         SO-G Chat - IPC Demo           ║\n");
    printf("║   Usando Memória Compartilhada POSIX   ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");
    printf("Digite suas mensagens (Ctrl+C para sair)\n");
    printf("Abra outro terminal e execute './bin/chat' novamente\n");
    printf("para conversar entre processos!\n\n");
    
    // Cria/abre memória compartilhada
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Erro no shm_open");
        return 1;
    }
    
    // Define o tamanho
    if (ftruncate(shm_fd, sizeof(ChatMemory)) == -1) {
        perror("Erro no ftruncate");
        cleanup();
        return 1;
    }
    
    // Mapeia a memória
    chat_mem = mmap(NULL, sizeof(ChatMemory), PROT_READ | PROT_WRITE, 
                    MAP_SHARED, shm_fd, 0);
    if (chat_mem == MAP_FAILED) {
        perror("Erro no mmap");
        cleanup();
        return 1;
    }
    
    // Cria/abre semáforo
    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("Erro no sem_open");
        cleanup();
        return 1;
    }
    
    // Inicializa a memória se for o primeiro processo
    sem_wait(sem);
    if (chat_mem->count == 0) {
        memset(chat_mem, 0, sizeof(ChatMemory));
        printf("✓ Chat inicializado (primeiro processo)\n\n");
    } else {
        printf("✓ Conectado ao chat existente\n\n");
    }
    sem_post(sem);
    
    // Cria thread para receber mensagens
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receiver_thread, NULL);
    
    // Loop principal: envia mensagens
    char input[MAX_MSG_LEN];
    while (running) {
        printf("Você> ");
        fflush(stdout);
        
        if (fgets(input, MAX_MSG_LEN, stdin) == NULL) {
            break;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        if (strlen(input) == 0) {
            continue;
        }
        
        if (strcmp(input, "/sair") == 0 || strcmp(input, "/exit") == 0) {
            break;
        }
        
        // Escreve na memória compartilhada
        sem_wait(sem);
        
        int idx = chat_mem->count % MAX_MESSAGES;
        snprintf(chat_mem->messages[idx], MAX_MSG_LEN, "[PID %d]: %s", getpid(), input);
        chat_mem->count++;
        
        sem_post(sem);
    }
    
    printf("\nEncerrando chat...\n");
    running = 0;
    
    pthread_join(recv_thread, NULL);
    cleanup();
    
    return 0;
}
