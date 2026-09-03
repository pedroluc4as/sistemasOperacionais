// Inclui a biblioteca de Pthreads (threads POSIX)
#include <pthread.h>
// Inclui a biblioteca de entrada/saída padrão (para printf)
#include <stdio.h>
// Inclui a biblioteca padrão (para macros como NULL e funções como exit)
#include <stdlib.h>

// Define uma constante para o número de threads que queremos criar
#define NUMERO_DE_THREADS 10

// Esta é a função que cada thread executará
void *imprima_ola_mundo(void *tid) {
    // Converte o argumento void* de volta para um ponteiro de inteiro (int*)
    // Este ponteiro aponta para o ID *único* desta thread
    // Afirmamos para o compilador que dentro dessa caixa genérica tid existe o endereço de um número inteiro.
    // Transforme esse endereço de um número inteiro de volta em um ponteiro de inteiro (int *).
    // Depois, usamos *id para pegar o conteúdo real apontado por esse endereço na hora de imprimir.  
    int *id = (int *)(tid); 

    // Imprime a mensagem de "Olá Mundo" usando o ID da thread
    // Usamos *id para obter o valor para o qual o ponteiro aponta
    printf("Aula de SO cô Vlad slk º) %d\n", *id);

    // Termina a thread e não retorna nenhum valor específico (NULL)
    // Em vez disso, poderíamos apenas usar "return NULL;"
    pthread_exit(NULL);
}

// A função principal (main) é a primeira thread que inicia o programa
int main () {Ola mundo! Saudacoes da thread
    // Cria um array para armazenar os identificadores (handles) das threads
    pthread_t vet_threads[NUMERO_DE_THREADS];
    
    // Cria um array para armazenar os IDs de 0 a 9
    // Cada thread receberá um ponteiro para uma posição *diferente* deste array
    int thread_ids[NUMERO_DE_THREADS];
    
    // 'status' armazena o código de retorno de pthread_create para checar erros
    // 'i' é nossa variável de loop
    int status, i;

    // === 1. Loop de Criação de Threads ===
    for (i=0; i < NUMERO_DE_THREADS; i++) {
        printf("Funcao principal (main) aqui. Criando a thread %d\n", i);
        
        // Armazena o valor de 'i' na posição 'i' do nosso array de IDs
        thread_ids[i] = i;
        
        // status vai criar a nova thread. Os quatro parâmetros são:
        // 1. &vet_threads[i]: Endereço onde o ID da thread será armazenado. O Sistema Operacional precisa 
        // de um lugar para anotar o número de registro (handle) da thread que acabou de nascer para podermos controlá-la depois.

        // 2. NULL: Atributos padrão da thread. Permite alterar tamanho de pilha e prioridades.
        // Para 99% dos casos básicos, passamos nulo para usar o padrão do sistema. 

        // 3. imprima_ola_mundo: Função que a thread deve executar
        // O nome da função que a thread vai executar assim que inicializar.

        // 4. (void *)(&thread_ids[i]): Argumento a ser passado para a função (o ponteiro para o ID único).
        // Neste parâmetro passamos o que enviaremos para a thread usar lá dentro.  
       
        status = pthread_create(&vet_threads[i], NULL, imprima_ola_mundo, (void *)(&thread_ids[i]));

        // Verifica se a criação da thread falhou
        if (status != 0) {
            printf("Opa... pthread_create retornou o codigo de erro %d\n", status);
            // Retorna -1 para indicar um erro
            return -1;
        }
    }

    // === 2. Loop de Junção (Join) ===
    // A thread 'main' deve esperar por todas as threads filhas.
    // Este loop só começa DEPOIS que todas as threads foram criadas.
    // O conceito de que a função main também é uma thread (a thread principal) é vital.
    // Sem o join, o programa principal morre antes dos filhos trabalharem.
    for (i=0; i < NUMERO_DE_THREADS; i++) {
        // pthread_join bloqueia a 'main' e a faz esperar até que a thread 'vet_threads[i]' termine
        // O segundo argumento (NULL) é usado se quiséssemos capturar o valor de retorno da thread
        status = pthread_join(vet_threads[i], NULL);
        
        if (status != 0) {
            printf("Opa... pthread_join retornou o codigo de erro %d\n", status);
            return -1;
        }
    }

    printf("Funcao principal (main) aqui. Todas as threads terminaram. Saindo...\n");
    
    // Agora que todas as threads terminaram, a 'main' pode sair com segurança
    return 0;
}
