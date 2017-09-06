#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mainMethods.c"

// estructura con toda la información del registro
// struct dogType {
//   char name[32];
//   char type[32];
//   int age;          //años
//   char breed[16];
//   int height;       //en cm
//   float weight;     //en kg
//   char sex;         //H o M
// };

const int SIZE = 1800;

struct Node {
  int data;
  char* key;
  struct Node *PN;
  struct Node *NN;
};

struct Node* hashArray[1800];


char* getKeyFromId(int data) {
  char* key = (char*)malloc(sizeof(char)*32);
  //open records file
  FILE *f;
  f = fopen(file, "r");
  if(f == NULL) {
    perror("error fopen");
    exit(-1);
  }

  fseek(f, data * sizeof(struct dogType) + 4, SEEK_SET);
  fgets(key, 32, f);

  //close records file
  int r = fclose(f);
  if (r != 0) {
    perror("error fclose");
    exit(-1);
  }

  return key;
}

// Function that assigns a unique hash key to a specific name value
int hashCode(char* key) {
  int hash = 0;
  for (int i = 0; i < strlen(key); i++) {
    hash += key[i] * (i * 2 + 1);
  }
  hash = hash % SIZE;

  while((hashArray[hash] != NULL) && hashArray[hash]->key != key) {
    hash = (hash + 7) % SIZE;
  }

  // printf("--- hash final %d\n", hash);
  return hash;
}

void insert(char* key, int data) {
  struct Node *item = (struct Node*) malloc(sizeof(struct Node));
  struct Node *aux = (struct Node*) malloc(sizeof(struct Node));
  item->data = data;
  item->key = key;
  //  item->NN = NULL;
  //  item->PN = NULL;

  //get the hash
  int hashIndex = hashCode(key);
  aux = hashArray[hashIndex];

  if(aux == NULL) {
    hashArray[hashIndex] = item;
    // printf("---The name %s has been inserted for the first time\n", item->key);
  } else if(aux->data == -1 && item->key == aux->key) {
    hashArray[hashIndex] = item;
    // printf("---The name %s has been deleted before\n", item->key);
  } else {
    while(aux->NN != NULL) {
      aux = aux->NN;
    }
    aux->NN = item;
    item->PN = aux;
    // printf("---The name %s has been inserted before\n", item->key);
  }

  // printf("---Name %s in the hash %d inserted\n\n", hashArray[hashIndex]->key, hashIndex);
}

// Function that searches a node from a name
struct Node *search(char* key) {
  //get the hash
  int hashIndex = hashCode(key);
  struct Node *item = (struct Node*) malloc(sizeof(struct Node));

  item = hashArray[hashIndex];

  while(item != NULL) {
    if(item->key == key && item->data != -1)
    return item;

    item = item->NN;
  }

  return NULL;
}

void delete(int data) {
  char* key = getKeyFromId(data);
  int hashIndex = hashCode(key);
  struct Node *item = hashArray[hashIndex];
  bool success = false;


  //move in array until an empty
  while(item != NULL) {
    if(item->data == data) {
      if(item->PN == NULL && item->NN != NULL) {
        printf("Es el primero\n");
        struct Node *next = item->NN;
        hashArray[hashIndex] = next;
        next->PN = NULL;
        item = NULL;
      }
      else if(item->NN != NULL && item->PN != NULL) {
        printf("Es de en medio\n");
        struct Node *prev = item->PN;
        struct Node *next = item->NN;
        prev->NN = item->NN;
        next->PN = item->PN;
        item = NULL;
      }
      else if(item->PN != NULL && item->NN == NULL)  {
        printf("Es el ultimo\n");
        struct Node *prev = item->PN;
        prev->NN = NULL;
        item = NULL;
      }
      else {
        printf("Es el unico\n");
        item->data = -1;
      }
      success = true;
    } else {
      item = item->NN;
    }
  }

  if(success) {
    printf("---The name %s with id %d has been deleted\n", key, data);
  }
}

void showTable() {
  struct Node *item;
  int count = 0;
  printf("\n---- HASH TABLE ----\n");
  for (int i = 0; i < SIZE; i++) {
    item = hashArray[i];
    while(item != NULL) {
      if(item->data != -1) {
        count ++;
        printf("Nombre: %s, ID: %d\n", item->key, item->data);
      }
      item = item->NN;
    }
  }
  if(count == 0) {
    printf("There are no pets in this hash\n");
  }
}

int main() {

  int qty = recordsQty();
  char* key = (char*)malloc(sizeof(char)*32);;

  //open records file
  FILE *f;
  f = fopen("dataDogs.datr+", "r");
  if(f == NULL) {
    perror("error fopen hashTable main");
    exit(-1);
  }

  for (int i = 0; i < qty; i++) {
    fseek(f, i * sizeof(struct dogType) + 4, SEEK_SET);
    fgets(key, 32, f);
    printf("%d\n", i + 1);

    //generating the hash from the dataDogs.datr+ file
    insert(key, i + 1);
  }

  //close records file
  int r = fclose(f);
  if (r != 0) {
    perror("error fclose");
    exit(-1);
  }

  return 0;
}
