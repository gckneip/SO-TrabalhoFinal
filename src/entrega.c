#include "entrega.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "config.h"

void* realizarEntrega(void* arg) {
  EntregaArgs* data = (EntregaArgs*)arg;
  if (data->isNovato) {
    pthread_mutex_lock(data->pedido);
    printf("[Novato %d] Peguei o lanche do Restaurante %d\n", data->entregador,
           data->restaurante);
    sleep(TEMPO_ESPERA_LANCHE);  // Simula o tempo para pegar o lanche

    printf("[Novato %d] Aguardando moto do Restaurante %d\n", data->entregador,
           data->restaurante);
    if (pthread_mutex_trylock(data->moto) != 0) {
      fprintf(stderr,
              "[Novato %d] Outro entregador está com a moto do Restaurante %d. "
              "Devolvendo o lanche.\n",
              data->entregador, data->restaurante);
      pthread_mutex_unlock(data->pedido);
      pthread_exit(NULL);
    }

    printf("[Novato %d] Peguei a moto do Restaurante %d\n", data->entregador,
           data->restaurante);
    printf("[Novato %d] Realizando entrega... %d\n", data->entregador,
           data->restaurante);
    pthread_mutex_unlock(data->moto);
    pthread_mutex_unlock(data->pedido);
  } else {
    pthread_mutex_lock(data->moto);
    printf("[Veterano %d] Peguei a moto do Restaurante %d\n", data->entregador,
           data->restaurante);
    sleep(TEMPO_ESPERA_MOTO);  // Simula o tempo para pegar a moto
    printf("[Veterano %d] Aguardando lanche do Restaurante %d\n",
           data->entregador, data->restaurante);
    pthread_mutex_lock(data->pedido);
    printf("[Veterano %d] Peguei o lanche do Restaurante %d\n",
           data->entregador, data->restaurante);
    printf("[Veterano %d] Realizando entrega... %d\n", data->entregador,
           data->restaurante);
    pthread_mutex_unlock(data->pedido);
    pthread_mutex_unlock(data->moto);
  }
  pthread_exit(NULL);
}
