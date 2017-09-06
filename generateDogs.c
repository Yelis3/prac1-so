#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mainMethods.c"

//funcion para generar un nombre aleatorio
char* generateName(FILE *f) {
  char* name = (char*)malloc(sizeof(char)*32);
  int maxRan = fileSize(f);

  int ran = rand() % maxRan;
  fseek(f, ran, SEEK_SET);
  if(fgets(name, 32, f)) {
    fgets(name, 32, f);
  }

  return name;
}

char* generateType() {
  int ran = rand() % 4;
  char *types[5] = {"perro", "gato", "hamster", "conejo", "pez"};
  return types[ran];
}

char* generateBreed(char animal[]) {
  int ran = rand() % 4;
  char *dogB[5] = {"husky", "labrador", "beagle", "pug", "bulldog"};
  char *catB[5] = {"siames", "persa", "angora", "abisinio", "ragdoll"};
  char *hamsterB[5] = {"ruso", "enano", "dorado", "de angora", "albino"};
  char *rabbitB[5] = {"mini lop", "californiano", "holandes", "jersey wooly", "arlequin"};
  char *fishB[5] = {"dorado", "pez gato", "pez payaso", "bailarina", "guppy"};

  if (animal == "perro") {
    return dogB[ran];
  }
  else if (animal == "gato") {
    return catB[ran];
  }
  else if (animal == "hamster") {
    return hamsterB[ran];
  }
  else if (animal == "conejo") {
    return rabbitB[ran];
  }
  else if (animal == "pez") {
    return fishB[ran];
  }
}

char generateSex() {
  int ran = rand() % 1;
  if(ran)
    return 'H';
  else
    return 'M';
}

int main() {
  struct dogType p;
  //open records file
  FILE *f;
  f = fopen(file, "a");
  if(f == NULL) {
    perror("error fopen");
    exit(-1);
  }
  //open names file
  FILE *n;
  n = fopen("petNames.txt", "r");
  if(f == NULL) {
    perror("error fopen");
    exit(-1);
  }

  int totalRecords = 10000000;
  writeFile(file, &totalRecords, sizeof(int), 1);

  for(int i = 0; i<1e07; i++) {
    char * name = generateName(n);
      name[strlen(name)-1] = 0; //remove the space that is not used by the string
    char * type = generateType();
    int age = rand() % 14;
    char * breed = generateBreed(type);
    int height = rand() % 120;
    float weight = (rand() % 60) + (rand() % 9)*0.1;
    char sex = generateSex();

    strcpy(p.name, name);
    strcpy(p.type, type);
    p.age = age;
    strcpy(p.breed, breed);
    p.height = height;
    p.weight = weight;
    p.sex = sex;

    int r = fwrite(&p, sizeof(struct dogType), ELEMENTS, f);
    if(r != ELEMENTS){
      perror("error fwrite");
      exit(-1);
    }
  }

  //close records file
  int r = fclose(f);
  if (r != 0) {
    perror("error fclose");
    exit(-1);
  }
  //close names file
  r = fclose(n);
  if (r != 0) {
    perror("error fclose");
    exit(-1);
  }

  system("mv dataDogs.data dataDogs.datr+");

  return 0;
}
