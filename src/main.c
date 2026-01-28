#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "entrega.h"

int main() {
  pthread_mutex_t motos[MAX_RESTAURANTES];
  pthread_mutex_t pedidos[MAX_RESTAURANTES];
  int entregadores_veteranos[NUM_THREADS / 2];
  int entregadores_novatos[NUM_THREADS / 2];

  size_t totalEntregas = NUM_THREADS;
  pthread_t* threads = calloc(totalEntregas, sizeof(pthread_t));
  EntregaArgs* entregas = calloc(totalEntregas, sizeof(EntregaArgs));

  if (!threads || !entregas) {
    fprintf(stderr, "Erro ao alocar memoria para entregas\n");
    free(threads);
    free(entregas);
    return 1;
  }

  for (int i = 0; i < MAX_RESTAURANTES; i++) {
    pthread_mutex_init(&motos[i], NULL);
    pthread_mutex_init(&pedidos[i], NULL);
  }

  for (int i = 0; i < NUM_THREADS / 2; i++) {
    entregadores_veteranos[i] = i + 1;
    entregadores_novatos[i] = i + 1;
  }

  // Create NUM_THREADS deliveries alternating novatos/veteranos
  for (int i = 0; i < NUM_THREADS; i++) {
    int restaurante_idx = i % MAX_RESTAURANTES;
    entregas[i].pedido = &pedidos[restaurante_idx];
    entregas[i].moto = &motos[restaurante_idx];
    entregas[i].isNovato = (i % 2 == 0);
    entregas[i].entregador = entregas[i].isNovato
                                 ? entregadores_novatos[i / 2]
                                 : entregadores_veteranos[i / 2];
    entregas[i].restaurante = i % MAX_RESTAURANTES + 1;
    int rc = pthread_create(&threads[i], NULL, realizarEntrega, &entregas[i]);
    if (rc != 0) {
      fprintf(stderr, "Falha ao criar thread %d (rc=%d)\n", i, rc);
    }
  }

  for (size_t t = 0; t < NUM_THREADS; t++) {
    pthread_join(threads[t], NULL);
  }

  free(threads);
  free(entregas);
  return 0;
};