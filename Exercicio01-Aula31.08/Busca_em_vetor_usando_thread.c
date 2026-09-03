#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define TAMANHO 1000
#define NUM_THREADS 4

int vet[TAMANHO];
int maiores_parciais[NUM_THREADS]; //Vetor global secundário

void *busca_maior_valor(void *arg) 
{
    int id = *(int *)(arg);

    //Calcula a divisão de trampo para cada threads
    int items_por_thread = TAMANHO / NUM_THREADS;
    int inicio = id * items_por_thread;
    int fim = inicio + items_por_thread;

    //Correção para a última thread
    if (id == NUM_THREADS - 1) 
    {
        fim = TAMANHO; 
    }

    //Lógica de busca
    int maior_local = vet[inicio];
    for (int i = inicio + 1; i < fim; i++) 
    {
        if (vet[i] > maior_local) {
            maior_local = vet[i];//Coloca o primeiro item como o maior, assumindo que seja o maior
        }
    }
    
    // Salva exclusivamente na posição correspondente
    maiores_parciais[id] = maior_local; 
    printf("Thread %d vasculhou de %d ate %d e achou o maior parcial: %d\n", id, inicio, fim - 1, maior_local);
    
    pthread_exit(NULL);
}

int main() 
{
    // Semente para gerar números verdadeiramente aleatórios
    srand(time(NULL));
    
    //Preenche o vetor principal com valores aleatórios
    for (int i = 0; i < TAMANHO; i++) 
    {
        vet[i] = rand() % 10000; 
    }
    
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    int status;

    //1.Loop de Criação de Threads
    for (int i = 0; i < NUM_THREADS; i++) 
    {		
        thread_ids[i] = i; 
        status = pthread_create(&threads[i], NULL, busca_maior_valor, (void *)(&thread_ids[i]));
        if (status != 0) 
        {
            printf("Erro ao criar thread. Código: %d\n", status);
            return -1;
        }
    }

    //2.Loop de Junção (Join)
    for (int i = 0; i < NUM_THREADS; i++) 
    {
        pthread_join(threads[i], NULL);
    }

    //3.Busca do Maior Absoluto na main
    printf("\nmain: Todas as threads terminaram. Analisando o vetor de parciais...\n");
    int maior_absoluto = maiores_parciais[0];
    for (int i = 1; i < NUM_THREADS; i++) 
    {
        if (maiores_parciais[i] > maior_absoluto) 
        {
            maior_absoluto = maiores_parciais[i];
        }
    }

    printf("O MAIOR número de todo o vetor é: %d\n", maior_absoluto);

    return 0;   
}