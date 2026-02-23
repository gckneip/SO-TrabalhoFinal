#include "entrega.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "config.h"

void* realizarEntrega(void* arg) {
  EntregaArgs* data = (EntregaArgs*)arg;

  if (data->isNovato) {
    if (pthread_mutex_trylock(data->pedido) != 0) pthread_exit(NULL);

    printf("[Novato %d] Peguei o lanche do Restaurante %d\n", data->entregador,
           data->restaurante);

    sleep(TEMPO_ESPERA_LANCHE);

    printf("[Novato %d] Aguardando moto do Restaurante %d\n", data->entregador,
           data->restaurante);

    pthread_mutex_lock(&data->controle->lock);
    int novatos_cederam = data->controle->novatos_cederam;
    pthread_mutex_unlock(&data->controle->lock);
    int novatos_vao_ser_demitidos = (novatos_cederam >= LIMITE_AGING);

    if (!novatos_vao_ser_demitidos) {
      if (pthread_mutex_trylock(data->moto) != 0) {
        pthread_mutex_lock(&data->controle->lock);
        data->controle->novatos_cederam++;
        pthread_mutex_unlock(&data->controle->lock);
        printf(
            "[Novato %d] Moto ocupada. Devolvendo lanche. (Limite de retornos "
            "%d/%d)\n ",
            data->entregador, novatos_cederam + 1, LIMITE_AGING);

        pthread_mutex_unlock(data->pedido);
        pthread_exit(NULL);
      }
    } else {
      pthread_mutex_lock(&data->controle->lock);
      data->controle->novato_aguardando_prioridade[data->restaurante] = 1;
      data->controle->novatos_cederam = 0;
      pthread_mutex_unlock(&data->controle->lock);

      printf("[Novato %d] Atingiu limite de retornos (%d/%d)...\n",
             data->entregador, novatos_cederam, LIMITE_AGING);
      int tentativas = 0;

      while (pthread_mutex_trylock(data->moto) != 0) {
        tentativas++;
        sleep(1);

        if (tentativas >= LIMITE_ESPERA) {
          printf("[Novato %d] Esperou demais. Devolvendo lanche.\n",
                 data->entregador);

          pthread_mutex_unlock(data->pedido);

          pthread_mutex_lock(&data->controle->lock);
          data->controle->novato_aguardando_prioridade[data->restaurante] = 0;
          pthread_mutex_unlock(&data->controle->lock);

          pthread_exit(NULL);
        }
      }
    }

    printf("[Novato %d] Peguei a moto do Restaurante %d\n", data->entregador,
           data->restaurante);

    printf("[Novato %d] Realizando entrega...\n", data->entregador);

    pthread_mutex_lock(&data->controle->lock);
    data->controle->novatos_cederam = 0;
    data->controle->novato_aguardando_prioridade[data->restaurante] = 0;
    pthread_mutex_unlock(&data->controle->lock);

    pthread_mutex_unlock(data->moto);
    pthread_mutex_unlock(data->pedido);

  } else {
    if (pthread_mutex_trylock(data->moto) != 0) pthread_exit(NULL);
    printf("[Veterano %d] Peguei a moto do Restaurante %d\n", data->entregador,
           data->restaurante);

    sleep(TEMPO_ESPERA_MOTO);

    printf("[Veterano %d] Aguardando lanche do Restaurante %d\n",
           data->entregador, data->restaurante);

    pthread_mutex_lock(&data->controle->lock);
    int bloquear_veterano =
        data->controle->novato_aguardando_prioridade[data->restaurante] == 1;
    pthread_mutex_unlock(&data->controle->lock);

    if (bloquear_veterano) {
      if (pthread_mutex_trylock(data->pedido) != 0) {
        printf("[Veterano %d] Pedido ocupado. Devolvendo moto.\n",
               data->entregador);
        pthread_mutex_unlock(data->moto);
        pthread_exit(NULL);
      }

    } else {
      pthread_mutex_lock(data->pedido);
    }
    printf("[Veterano %d] Peguei o lanche do Restaurante %d\n",
           data->entregador, data->restaurante);

    printf("[Veterano %d] Realizando entrega...\n", data->entregador);

    pthread_mutex_unlock(data->pedido);
    pthread_mutex_unlock(data->moto);
  }

  pthread_exit(NULL);
}