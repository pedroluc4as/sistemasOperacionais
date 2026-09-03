#include <stdio.h>
#include <pthread.h> // Biblioteca de threads
#include <stdlib.h>  // Para exit()
#include <unistd.h>  // Para a função sleep()

// Variável global simples para simular os dados compartilhados na memória
char dados_compartilhados[100];
int dados_prontos = 0; // Flag indicadora (0 = não pronto, 1 = pronto)

// --- A MÁGICA DA SINCRONIZAÇÃO ACONTECE AQUI ---
// Mutex: Garante que apenas uma thread leia ou escreva na variável global por vez.
pthread_mutex_t mutex_dados = PTHREAD_MUTEX_INITIALIZER;
// Variável de Condição: Permite que uma thread durma até receber um "sinal" de outra.
pthread_cond_t cond_dados = PTHREAD_COND_INITIALIZER;

/**
 * @brief TAREFA A (Produtor): Simula uma busca lenta de dados (ex: API, BD).
 */
void *funcao_busca_dados(void *arg) {
    int id = *(int *)arg;
    printf("[Thread %d - Busca]: Iniciando busca de dados... (Isso vai levar 2s)\n", id);
    
    // Simula um trabalho demorado de hardware (ex: disco rígido lento ou rede)
    sleep(2); 
    
    // Antes de modificar os dados e a flag, precisamos bloquear o acesso (Exclusão Mútua)
    pthread_mutex_lock(&mutex_dados);
    
    // "Copia" os dados para a variável compartilhada
    sprintf(dados_compartilhados, "Dados foram buscados pela Thread %d!", id);
    dados_prontos = 1; // Sinaliza que os dados estão prontos
    
    // Emite o sinal de que a condição mudou para quem estiver esperando (acorda a Thread B)
    pthread_cond_signal(&cond_dados);
    
    // Libera o acesso (Mutex) para que outras threads possam ler a variável
    pthread_mutex_unlock(&mutex_dados);
    
    printf("[Thread %d - Busca]: DADOS PRONTOS e sinal enviado! Saindo.\n", id);
    pthread_exit(NULL);
}

/**
 * @brief TAREFA B (Consumidor): Simula o processamento dos dados.
 */
void *funcao_processa_dados(void *arg) {
    int id = *(int *)arg;
    printf("[Thread %d - Processa]: Iniciando... Aguardando os dados chegarem.\n", id);
    
    // Bloqueia o acesso para verificar a condição da flag com segurança
    pthread_mutex_lock(&mutex_dados);
    
    // O loop while é essencial para verificar a condição. 
    // Evita problemas com "spurious wakeups" (quando o SO acorda a thread por engano).
    while (dados_prontos == 0) {
        // A thread dorme aqui E libera o mutex ao mesmo tempo.
        // Só acorda quando receber o signal. Ao acordar, ela retoma o mutex automaticamente.
        // Isso economiza CPU, pois a thread não fica em "busy wait" (loop infinito gastando processamento).
        pthread_cond_wait(&cond_dados, &mutex_dados);
    }
    
    // Neste ponto, temos o mutex bloqueado para nós e sabemos que a flag mudou.
    // Simula um trabalho de processamento rápido
    printf("[Thread %d - Processa]: Processando os dados: '%s'\n", id, dados_compartilhados);
    
    // Libera o mutex após finalizar o uso da variável compartilhada
    pthread_mutex_unlock(&mutex_dados);
    
    printf("[Thread %d - Processa]: Processamento concluído. Saindo.\n", id);
    pthread_exit(NULL);
}

int main() {
    pthread_t t_busca, t_processa; // Handles para as duas threads
    int id_a = 1;
    int id_b = 2;
    int status;

    printf("[Main]: Iniciando o pipeline...\n");

    // As duas threads são criadas PRATICAMENTE ao mesmo tempo.
    // Ambas estarão vivas e rodando simultaneamente (Concorrência real!)
    // Note que a ordem de criação aqui não importa mais para o resultado do programa.
    
    printf("[Main]: Criando a Thread B (%d) para PROCESSAR dados.\n", id_b);
    status = pthread_create(&t_processa, NULL, funcao_processa_dados, (void *)&id_b);
    if (status != 0) { printf("Erro ao criar thread B.\n"); return -1; }
    
    printf("[Main]: Criando a Thread A (%d) para BUSCAR dados.\n", id_a);
    status = pthread_create(&t_busca, NULL, funcao_busca_dados, (void *)&id_a);
    if (status != 0) { printf("Erro ao criar thread A.\n"); return -1; }

    // --- PONTO DE SINCRONIZAÇÃO DO PROGRAMA PRINCIPAL ---
    // A main agora apenas espera que as DUAS threads terminem seu trabalho (Join).
    // A sincronização inteligente de QUEM faz O QUÊ primeiro está programada nas próprias threads.
    printf("[Main]: Esperando as Threads terminarem...\n");
    pthread_join(t_busca, NULL);
    pthread_join(t_processa, NULL);

    printf("[Main]: Pipeline concluído. Fim do programa.\n");
    return 0;
}
