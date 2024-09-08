#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock1, lock2; //Cria dois mutexes

// Cria um processo A que PODE retornar um ponteiro
// para qualquer tipo, e também PODE receber um ponteiro para qualquer tipo
void *processA(void *arg) { 
    pthread_mutex_lock(&lock1); // Adquire o recurso 1, representado pelo lock1
    printf("Processo A adquiriu o lock 1\n");
    sleep(1); // Simula algum processamento
    pthread_mutex_lock(&lock2);
    printf("Process A adquiriu o lock 2\n");

    // Libera os recursos|mutexes
    
    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);
    printf("Processo A liberou os recursos\n");
}

void *processB(void *arg) {
    pthread_mutex_lock(&lock1);
    printf("Process B adquiriu o lock 1\n");
    sleep(1); // Simula algum processamento
    pthread_mutex_lock(&lock2);
    printf("Process B adquiriu o lock 2\n");

    // Libera os recursos
    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
    printf("Processo A liberou os recursos\n");
}

int main() {
    pthread_t t1, t2;// Define as threads
    
    //Inicializa os mutexes com atributos padrão
    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&lock2, NULL);
    
    //Cria as threads com atributos padrão, que executam as funções definidas sem
    // argumentos
    pthread_create(&t1, NULL, processA, NULL);
    pthread_create(&t2, NULL, processB, NULL);

    // Faz a thread principal (Main) esperar pela finalização das threads e não 
    // deseja coletar os possíveis retornos das threads (expressos pelo NULL)
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Libera os recursos|mutexes para não sobrecarregar o sistema
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);

    return 0;
}
