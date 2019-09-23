
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
pthread_mutex_t m;
pthread_cond_t c;
int t = 0, itemsOnBuffer = 0;


typedef struct buff node;
struct buff {
 int content;
 node *next;
};
typedef struct c cons;
struct c {
 node *B;
};
typedef struct p prod;
struct p {
 node *B;
 int V;
};

void insertOnBuffer(void *buffer){
node *newNode = malloc(sizeof(node));
newNode->content = rand();
newNode->next = buffer->next;
buffer->next = newNode;
itemsOnBuffer++;
}
int removeFromBuffer(void *buffer){
node *tail, *aux;
tail = buffer;
while(tail->next != NULL){
  aux = tail;
  tail = tail->next;
}
aux->next = NULL;
int item = tail->content;
free(tail);
itemsOnBuffer--;
return item;
}
void producer(void *B, int V){
pthread_mutex_lock(&m);
for(; V>0 ; V--){
  if(itemsOnBuffer <= 30){
    while(itemsOnBuffer == t)
      pthread_cond_wait( &m, &c );
    pthread_mutex_lock(&m);
    insertOnBuffer(&B);
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
  }
}
}
void consumer(void *B){
for(;;){
  pthread_mutex_lock(&m);
  while(itemsOnBuffer<1)
    pthread_cond_wait(&m, &c);
  item = removeFromBuffer(&B);
  pthread_mutex_unlock(&m);
  if(item == -1){
    pthread_mutex_unlock(&m);
    return ;
  }
  if(primo(item)){
    printf("[%d:%d]\n", pthread_self(), item);
  }
}
}
bool primo(int item){
int i = 0;
for(i=2; i<abs(item); i++){
  if(item % i == 0) return false;
}
return true;
}
void main(void) {
int v = 0, p = 0, c = 0;
printf("Enter the \"v p c t\" values");
scanf("%d %d %d %d", &v, &p, &c, &t);
node *buffer;
pthread_mutex_init(&m, NULL);
pthread_t producers[5], consumers[5];
int i = 0;
prod *args_prod;
cons *args_cons;
args_prod = (prod *) malloc(sizeof(prod));
args_prod->B = buffer;
args_prod->V = v;
args_cons = (cons *) malloc(sizeof(cons));
args_cons->B = buffer;
for(i=0; i<p; i++){
  pthread_create(&producers[i], NULL, consumer, args_prod);
}
for(i=0; i<c; i++){
  pthread_create(&consumers[i], NULL, consumer, args_cons);
}

for(i=0; i<c; i++){
  pthread_join(consumers[i], NULL);
}
for(i=0; i<p; i++){
  pthread_join(producers[i], NULL);
}

}
