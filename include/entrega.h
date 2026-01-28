#ifndef ENTREGA_H
#define ENTREGA_H

#include <pthread.h>

typedef struct {
  pthread_mutex_t* pedido;
  pthread_mutex_t* moto;
  int entregador;
  int isNovato;
  int restaurante;
} EntregaArgs;

void* realizarEntrega(void* arg);

#endif  // ENTREGA_H
