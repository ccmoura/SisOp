/*
Produtor / Consumidor em Pthreads
Autor: Christopher Conceição Moura
Disciplina: Sistemas Operacionais
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h> // para abs() em número primo de valor negativo

//variaveis de condicao
pthread_mutex_t m;
pthread_cond_t c;
int t = 0, itemsOnBuffer = 0;
int upper = 1000, lower = -1000; // faixa para numeros aleatorios

// structs para buffer e argumentos
typedef struct buff node;
struct buff {
 int content;
 node *next;
};
struct cons {
 node *B;
};
struct prod {
 node *B;
 int V;
};

// funções de inserção/remoção do buffer, primo e produtor/consumidor
void insertOnBuffer(node *buffer){
  node *newNode = malloc(sizeof(node));
  newNode->content = (rand() % (upper-lower+1)) + lower;
  newNode->next = buffer->next;
  buffer->next = newNode;
  itemsOnBuffer++;
} // insertOnBuffer
int removeFromBuffer(node *buffer){
  node *tail, *aux;
  tail = buffer;
  while(tail->next != NULL){
    aux = tail;
    tail = tail->next;
  } // while
  aux->next = NULL;
  int item = tail->content;
  free(tail);
  itemsOnBuffer--;
  return item;
}// removeFromBuffer
void producer(void *args){
  struct prod *arguments = args;
  int i = arguments->V;
  for(; i>0 ; i--){
    if(itemsOnBuffer <= t){
      while(itemsOnBuffer == t)
        pthread_cond_wait(&c, &m);
      pthread_mutex_lock(&m);
      insertOnBuffer(arguments->B);
      pthread_cond_signal(&c);
      pthread_mutex_unlock(&m);
    } // if
  } // for
} // producer
int primo(int item){
  int i;
  for(i=2; i<abs(item); i++){
    if(item % i == 0) return 0;
  } // for
  return 1;
} // primo
void consumer(void *args){
  int id_thread = pthread_self();
  struct cons *arguments = args;
  for(;;){
    pthread_mutex_lock(&m);
    while(itemsOnBuffer<1)
      pthread_cond_wait(&c, &m);
    int item = removeFromBuffer(arguments->B);
    pthread_mutex_unlock(&m);
    if(item == -1){
      printf("Consumer id[%d] found a -1, closing consumer.\n", id_thread);
      return ;
    } // if
    int p = primo(item);
    if(p){
      printf("[%d:%d]\n", id_thread, item);
    } // if
  } // for
} // consumer

// main
void main() {
  srand( (unsigned)time(NULL) );
  int v = 0, p = 0, c = 0, i = 0;
  printf("Enter the \"v p c t\" values: ");
  scanf("%d %d %d %d", &v, &p, &c, &t);

  node *buffer;
  buffer = malloc(sizeof(node));

  pthread_mutex_init(&m, NULL);
  pthread_t producers[p], consumers[c];

  // pegando argumentos que serão passados para as threads / buffer e V - número de interações
  struct prod *args_prod;
  struct cons *args_cons;
  args_prod = (struct prod *)malloc(sizeof(struct prod));
  args_prod->B = buffer;
  args_prod->V = v;
  args_cons = (struct cons*)malloc(sizeof(struct cons));
  args_cons->B = buffer;

  // lançando threads
  for(i=0; i<p; i++){
    pthread_create(&producers[i], NULL, (void *) producer, args_prod);
  }
  for(i=0; i<c; i++){
    pthread_create(&consumers[i], NULL, (void *) consumer, args_cons);
  }

  // joins
  for(i=0; i<p; i++){
    pthread_join(producers[i], NULL);
  }
  for(i=0; i<c; i++){
    pthread_join(consumers[i], NULL);
  }
} // main
