#ifndef ENTREGA_H
#define ENTREGA_H

#include <pthread.h>

#include "config.h"
typedef struct {
  int novatos_cederam;
  int novato_aguardando_prioridade[NUM_RESTAURANTES];
  pthread_mutex_t lock;
} RestauranteControle;

typedef struct {
  int entregador;
  int restaurante;
  int isNovato;

  pthread_mutex_t* pedido;
  pthread_mutex_t* moto;

  RestauranteControle* controle;
} EntregaArgs;

void* realizarEntrega(void* arg);

#endif  // ENTREGA_H
