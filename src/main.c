#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "entrega.h"

int main() {
  printf("==== TESTE DEADLOCK + STARVATION ====\n");
  printf("1 Restaurante | Alta Contencao | Aging ativo\n\n");

  pthread_mutex_t motos[NUM_RESTAURANTES];
  pthread_mutex_t pedidos[NUM_RESTAURANTES];

  RestauranteControle controle;
  controle.novatos_cederam = 0;
  pthread_mutex_init(&controle.lock, NULL);

  for (int i = 0; i < NUM_RESTAURANTES; i++) {
    pthread_mutex_init(&motos[i], NULL);
    pthread_mutex_init(&pedidos[i], NULL);
  }

  pthread_t threads[NUM_THREADS];
  EntregaArgs entregas[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    int restaurante;

    int faixa = NUM_THREADS / NUM_RESTAURANTES;
    restaurante = i / faixa;
    if (restaurante >= NUM_RESTAURANTES) {
      restaurante = NUM_RESTAURANTES - 1;
    }

    entregas[i].pedido = &pedidos[restaurante];
    entregas[i].moto = &motos[restaurante];
    entregas[i].controle = &controle;

    entregas[i].isNovato = (i % 2 == 0);
    entregas[i].entregador = i + 1;
    entregas[i].restaurante = restaurante + 1;

    int rc = pthread_create(&threads[i], NULL, realizarEntrega, &entregas[i]);
    if (rc != 0) {
      fprintf(stderr, "Erro ao criar thread %d\n", i);
    }
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  printf("\n==== FIM DO TESTE ====\n");

  for (int i = 0; i < NUM_RESTAURANTES; i++) {
    pthread_mutex_destroy(&motos[i]);
    pthread_mutex_destroy(&pedidos[i]);
  }
  pthread_mutex_destroy(&controle.lock);

  return 0;
}