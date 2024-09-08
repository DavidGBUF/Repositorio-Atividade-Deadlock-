#include <stdio.h>      // Inclui a biblioteca padrão de entrada e saída
#include <pthread.h>   // Inclui a biblioteca para suporte a threads
#include <unistd.h>    // Inclui a biblioteca para chamadas de sistema como sleep
#include <errno.h>     // Inclui a biblioteca para gerenciamento de erros

pthread_mutex_t lock1, lock2;  // Declara dois mutexes (lock1 e lock2) para sincronização
int deadlock_detectado = 0;    // Variável para sinalizar se um deadlock foi detectado

// Função executada pela thread do processo A
void* processA(void* arg) {
    // Tenta adquirir o lock1 sem bloqueio (não espera se não puder adquirir)
    if (pthread_mutex_trylock(&lock1) == 0) {
        printf("Processo A pegou lock1\n");
        sleep(1); // Simula algum processamento
        
        // Tenta adquirir o lock2 sem bloqueio
        if (pthread_mutex_trylock(&lock2) == 0) {
            printf("Processo A pegou lock2\n");
            pthread_mutex_unlock(&lock2); // Libera lock2
            printf("O recurso 2 foi liberado do processo A\n");
        } else {
            printf("Processo A não conseguiu pegar lock2\n");
        }
        
        pthread_mutex_unlock(&lock1); // Libera lock1
        printf("O recurso 1 foi liberado do processo A\n");
    } else {
        printf("Processo A não conseguiu pegar lock1\n");
    }
    return NULL; // Retorna NULL para indicar que a função terminou
}

// Função executada pela thread do processo B
void* processB(void* arg) {
    // Tenta adquirir o lock2 sem bloqueio
    if (pthread_mutex_trylock(&lock2) == 0) {
        printf("Processo B pegou lock2\n");
        sleep(1); // Simula algum processamento

        // Tenta adquirir o lock1 sem bloqueio
        if (pthread_mutex_trylock(&lock1) == 0) {
            printf("Processo B pegou lock1\n");
            pthread_mutex_unlock(&lock1); // Libera lock1
            printf("O recurso 1 foi liberado do processo B\n");
        } else {
            printf("Processo B não conseguiu pegar lock1\n");
        }

        pthread_mutex_unlock(&lock2); // Libera lock2
        printf("O recurso 2 do processo B foi usado e liberado\n");
    } else {
        printf("Processo B não conseguiu pegar lock2\n");
    }
    return NULL; // Retorna NULL para indicar que a função terminou
}

// Função executada pela thread de detecção de deadlock
void* detectar_deadlock(void* arg) {
    while (!deadlock_detectado) { // Continua verificando até que um deadlock seja detectado
        sleep(1); // Monitora a cada 1 segundo

        // Tenta adquirir ambos os mutexes sem bloqueio
        int lock1_status = pthread_mutex_trylock(&lock1);
        int lock2_status = pthread_mutex_trylock(&lock2);

        // Verifica se ambos os mutexes estão bloqueados
        if (lock1_status != 0 && lock2_status != 0) {
            printf("Pontecial Deadlock detectado!\n");
            deadlock_detectado = 1; // Define a variável para indicar que o deadlock foi detectado
        } else {
            // Se algum mutex foi adquirido, libera-o
            if (lock1_status == 0) pthread_mutex_unlock(&lock1);
            if (lock2_status == 0) pthread_mutex_unlock(&lock2);
        }
    }
    return NULL; // Retorna NULL para indicar que a função terminou
}

int main() {
    pthread_t t1, t2, t3; // Declara três threads

    // Inicializa os mutexes
    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&lock2, NULL);

    // Cria as threads para processA, processB e detectar_deadlock
    pthread_create(&t1, NULL, processA, NULL);
    pthread_create(&t2, NULL, processB, NULL);
    pthread_create(&t3, NULL, detectar_deadlock, NULL);

    // Aguarda a conclusão das threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    // Destrói os mutexes
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
    
    printf("Programa finalizou.\n"); // Mensagem final indicando que o programa terminou

    return 0; // Retorna 0 para indicar que o programa terminou com sucesso
}
