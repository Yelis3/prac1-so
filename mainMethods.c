#include <stdio.h>
#include <stdlib.h>
#define ELEMENTS 1

const char file[12] = "dataDogs.dat";

// estructura con toda la información del registro
struct dogType {
  char name[32];
  char type[32];
  int age;          //años
  char breed[16];
  int height;       //en cm
  float weight;     //en kg
  char sex;         //H o M
  int id;
};

//funcion para escribir en un archivo con el nombre 'file'
void writeFile(const char file[], void *data, size_t size, int elements) {
  FILE *f;
  f = fopen(file, "a");
  if(f == NULL) {
    perror("error fopen");
    exit(-1);
  }

  int r = fwrite(data, size, elements, f);
  if(r != elements){
    perror("error fwrite");
    exit(-1);
  }
  r = fclose(f);
  if (r != 0) {
    perror("error fclose");
    exit(-1);
  }
}

//funcion para leer un archivo con el nombre 'file'
void readFile(char file[], void *data, size_t size, int elements) {
  FILE *f;
  f = fopen(file, "r");
  if(f == NULL) {
    perror("error fopen");
    exit(-1);
  }

  rewind(f);
  int  r = fread(data, size, elements, f);
  if(r != elements) {
    printf("r value: %i\n", (int)r);
    perror("error fread");
    exit(-1);
  }
}

int fileSize(FILE *f){
  int number = -1;
  fseek(f, 4, SEEK_END);
  number = ftell(f);
  return number;
}

//funcion que devuelve la cantidad de registros en el archivo file
int recordsQty(char file[], size_t size) {
  FILE *f;
  f = fopen(file, "r");
  if(f == NULL) {
    perror("error fopen");
    exit(-1);
  }

  int number = fileSize(f);

  int r = fclose(f);
  if (r != 0) {
    perror("error fclose");
    exit(-1);
  }
  return number/size;
}

//funcion para ingresar los datos de un registro
int enterRecord(void *x) {
  struct dogType *p = x;
  printf("\n\n------------------------------------------------\n\nNombre: ");
  fflush(stdout);
  scanf("%s", p->name);
  printf("Tipo: ");
  fflush(stdout);
  scanf("%s", p->type);
  printf("Edad: ");
  fflush(stdout);
  scanf("%i", &p->age);
  printf("Raza: ");
  fflush(stdout);
  scanf("%s", p->breed);
  printf("Estatura (cm): ");
  fflush(stdout);
  scanf("%i", &p->height);
  printf("Peso (Kg): ");
  fflush(stdout);
  scanf("%f", &p->weight);
  printf("Sexo: ");
  fflush(stdout);
  scanf(" %c", &p->sex); //se pone un espacio en blanco al comienzo del formato para que lea el char

  writeFile(file, p, sizeof(struct dogType), 1);
}

//funcion para mostrar un registro
void show(void *x) {
  struct dogType *p = x;
  printf("Nombre:  %s\n", p->name);
  printf("Tipo:  %s\n", p->type);
  printf("Edad:  %i\n", p->age);
  printf("Raza:  %s\n", p->breed);
  printf("Estatura:  %i\n", p->height);
  printf("Peso:  %f\n", p->weight);
  printf("Sexo:  %c\n", p->sex);
}
