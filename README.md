# Simulação - Impasse do Delivery - LaranjalFoods

**Disciplina:** Sistemas Operacionais  
**Professor:** Rafael Burlamaqui Amaral  
**Universidade Federal de Pelotas**

🔗 **Repositório:** [https://github.com/gckneip/SO-TrabalhoFinal](https://github.com/gckneip/SO-TrabalhoFinal)

## 📋 Visão Geral do Projeto

Este projeto simula um problema clássico de deadlock em sistemas concorrentes através de um cenário baseado no aplicativo de entregas "Laranjal Foods" em Pelotas.

### Contexto do Problema

Em Pelotas, o aplicativo "Laranjal Foods" opera com uma regra rígida: cada restaurante possui sua própria moto personalizada (adesivada com a marca da lanchonete). Para realizar uma entrega do Restaurante X, o entregador precisa necessariamente:
- Pegar a Comida do Restaurante X
- Pegar a Moto do Restaurante X

A frota de entregadores é mista, composta por:
- **Veteranos**: muito experientes
- **Novatos**: recém-chegados

## 🔴 O Problema: Deadlock

O sistema de despacho do aplicativo tem um bug: ele ocasionalmente aloca dois entregadores simultaneamente para o mesmo restaurante quando a demanda está alta.

### Comportamento das Threads

#### Tipo 1: Entregador Veterano

O veterano prioriza a logística e não quer pegar o peso da comida sem garantir que tem transporte.

1. Escolhe (ou recebe) um restaurante alvo i
2. **Bloqueia a Moto i**
3. Simula o tempo de caminhada até o balcão (sleep)
4. Tenta pegar o Pedido i
5. Se conseguir ambos, faz a entrega, libera os recursos e volta para a fila

#### Tipo 2: Entregador Novato

O novato é ansioso para garantir a comissão e quer pegar o pacote logo para marcar no app que "coletou".

1. Escolhe (ou recebe) um restaurante alvo i
2. **Bloqueia o Pedido i**
3. Simula o tempo de caminhada até o estacionamento (sleep)
4. Tenta pegar a Moto i
5. Se conseguir ambos, faz a entrega, libera os recursos e volta para a fila

### Situação de Deadlock

O deadlock ocorrerá especificamente quando um Veterano e um Novato decidirem atender o mesmo restaurante quase ao mesmo tempo:

1. **Veterano** pega a chave da moto do McDonald's e vai buscar o lanche
2. **Novato** pega o lanche do McDonald's e vai buscar a moto
3. **Resultado - Travamento Eterno:**
   - Veterano chega no balcão: "Cadê o lanche? O Novato pegou. Vou esperar ele soltar." (Mas o novato não vai soltar, pois ele precisa da moto para entregar)
   - Novato chega na moto: "Cadê a chave? O Veterano pegou. Vou esperar ele soltar."
   - **Ambos ficam travados eternamente segurando recursos vitais um do outro**

## 🛠️ Especificações Técnicas

### Requisitos Obrigatórios

1. **Linguagem:** C com biblioteca `pthread`
   - Alternativa: Java com semáforos (não com `synchronized`)

2. **Convenção de Código:** 
   - ⚠️ **Em todo laço de repetição, utilize a variável `l` em vez de `i`**

3. **Estrutura de Dados:**
   - Array de mutexes para os pedidos
   - Array de mutexes para as motos
   - Número configurável de restaurantes (ex: 5 ou 10)

4. **Threads:**
   - Threads de Entregadores Veteranos
   - Threads de Entregadores Novatos
   - **Dica:** Para forçar o deadlock, faça com que as threads escolham o restaurante aleatoriamente, mas garanta que o número de threads seja **maior que o número de restaurantes**

### Logging (Saída Esperada)

O programa deve narrar o que está acontecendo para que o deadlock seja visível:

```
[Veterano 1]: Peguei a chave da moto do Restaurante 0.
[Novato 2]: Peguei o lanche do Restaurante 0.
[Veterano 1]: Aguardando lanche do Restaurante 0...
[Novato 2]: Aguardando moto do Restaurante 0...
(Após isso, nenhuma mensagem sobre o Restaurante 0 aparece mais -> Deadlock confirmado)
```

## ✅ Solução Implementada

### 1. Prevenção de Deadlock

A solução implementada utiliza **pthread_mutex_trylock()** para evitar bloqueio indefinido, combinado com um mecanismo de **backoff** (recuo) quando recursos não estão disponíveis.

#### Estratégia para Novatos:
- Usam `pthread_mutex_trylock()` para tentar pegar a moto
- Se a moto estiver ocupada, **devolvem o lanche** e desistem da entrega
- Isso evita que fiquem bloqueados esperando indefinidamente

#### Estratégia para Veteranos:
- Também usam `pthread_mutex_trylock()` para verificar disponibilidade
- Quando detectam que um novato tem prioridade (aging), usam trylock no pedido
- Se o pedido estiver ocupado, **devolvem a moto** e desistem

**Como previne deadlock:**
- Nenhum entregador fica bloqueado esperando eternamente
- O uso de `trylock` permite que threads "desistam" e liberem recursos
- Isso quebra o ciclo de espera circular necessário para deadlock

### 2. Prevenção de Starvation (Inanição)

A solução implementa um mecanismo de **Aging** para garantir que novatos não sejam perpetuamente impedidos de fazer entregas.

#### Como funciona:

1. **Contador de Retornos:**
   - Cada vez que um novato precisa devolver o lanche, incrementa `novatos_cederam`
   - Este contador é compartilhado e protegido por mutex

2. **Limite de Aging (`LIMITE_AGING`):**
   - Quando `novatos_cederam >= LIMITE_AGING` (padrão: 3)
   - O próximo novato que tentar fazer entrega recebe **prioridade máxima**

3. **Comportamento com Prioridade Ativa:**
   - O novato prioritário **bloqueia com `pthread_mutex_lock()`** na moto em vez de trylock
   - Seta flag `novato_aguardando_prioridade[restaurante] = 1`
   - Veteranos detectam essa flag e passam a usar `trylock` no pedido
   - Se o pedido estiver ocupado, o veterano **desiste e devolve a moto**

4. **Limite de Espera (`LIMITE_ESPERA`):**
   - Mesmo com prioridade, o novato tem um limite de 10 tentativas
   - Isso evita que fique preso indefinidamente caso haja algum problema

#### Parâmetros Configuráveis (config.h):

```c
#define LIMITE_AGING 3      // Quantas vezes novatos devem ceder antes de ganhar prioridade
#define LIMITE_ESPERA 10    // Máximo de tentativas para novato prioritário
```

**Como previne starvation:**
- Garante que novatos eventualmente conseguem fazer entregas
- Após 3 desistências seguidas, um novato ganha prioridade absoluta
- Veteranos cedem o espaço quando detectam a prioridade ativa
- O contador é resetado após entrega bem-sucedida

### Estrutura de Controle

```c
typedef struct {
  int novatos_cederam;                          // Contador de aging
  int novato_aguardando_prioridade[NUM_RESTAURANTES]; // Flag de prioridade por restaurante
  pthread_mutex_t lock;                         // Protege acesso aos contadores
} RestauranteControle;
```

## 🚀 Compilação e Execução

```bash
make
./bin/LaranjalFoods
```

## 📊 Estrutura do Projeto

```
SO-TrabalhoFinal/
├── Makefile                 # Compilação
├── README.md                # Este arquivo
├── bin/
│   └── LaranjalFoods        # Executável
├── include/
│   ├── config.h             # Configurações
│   └── entrega.h            # Declarações de funções
├── obj/                     # Arquivos objeto compilados
└── src/
    ├── main.c               # Função principal
    └── entrega.c            # Lógica de simulação
```