# Simulação - Impasse do Delivery - LaranjalFoods

**Disciplina:** Sistemas Operacionais  
**Professor:** Rafael Burlamaqui Amaral  
**Universidade Federal de Pelotas**

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