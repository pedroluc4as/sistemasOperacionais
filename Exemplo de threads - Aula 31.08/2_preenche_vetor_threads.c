#include <stdio.h>   // Para printf
#include <pthread.h> // Para threads
#include <stdlib.h>  // Para exit()

#define TAMANHO 800
#define NUM_THREADS 400 // Define o número de threads

int vet[TAMANHO];

/**
 * @brief Função que cada thread executará para preencher sua parte do vetor.
 *
 * Preenche a fatia do vetor com o ID da própria thread.
 */
void *preenche (void *arg) {
    // 1. Converte o argumento void* para o ID (0, 1, 2 ou 3)
    int id = *(int *)(arg);

    // 2. Calcula a fatia de trabalho de cada thread
    int items_por_thread = TAMANHO / NUM_THREADS;

    // 3. Calcula o índice inicial e final para ESTA thread
    int inicio = id * items_por_thread;
    int fim = inicio + items_por_thread;

    // 4. Correção para a última thread (caso TAMANHO não seja divisível)
    if (id == NUM_THREADS - 1) {
        fim = TAMANHO; 
    }

    printf ("Thread %d preenchendo de %d até %d com o valor %d.\n", id, inicio, fim - 1, id);

    // 5. Loop de preenchimento
    for (int i = inicio; i < fim; i++) {
        // Preencher com o ID da thread
        vet[i] = id; 
    }
    
    pthread_exit(NULL);
}

int main () {
	 // Inicializa o vetor com valor 8
    for (int i = 0; i < TAMANHO; i++) {
        vet[i] = -1; 
    }
		
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    int status;

    // --- 1. Loop de Criação de Threads ---
    for (int i = 0; i < NUM_THREADS; i++) {		
        thread_ids[i] = i; 
        printf("Main: criando thread %d\n", i);
        status = pthread_create(&threads[i], NULL, preenche, (void *)(&thread_ids[i]));
        if (status != 0) {
            printf("Erro ao criar thread. Codigo: %d\n", status);
            return -1;
        }
        
        /*printf ("Estado atual do vetor:\n");
        for (int i = 0; i < TAMANHO; i++) {
        printf ("%d ", vet[i]);
        }
        printf ("\n************\n");*/
        
    }

    // --- 2. Loop de Junção (Join) ---
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Main: Todas as threads terminaram. Imprimindo o vetor:\n");

    // --- 3. Impressão do Resultado ---
    for (int i = 0; i < TAMANHO; i++) {
        printf ("%d ", vet[i]);
    }
    printf("\n");

    return 0;   
}
