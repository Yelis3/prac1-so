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
  fseek(f, 0, SEEK_END);
  number = ftell(f);
  return number;
}

//funcion que devuelve la cantidad de registros en el archivo file
int recordsQty() {
  FILE *f;
  f = fopen("dataDogs.datr+", "r");
  if(f == NULL) {
    perror("error fopen recordsQty");
    exit(-1);
  }

  int number = fileSize(f);

  int r = fclose(f);
  if (r != 0) {
    perror("error fclose");
    exit(-1);
  }

  return number/sizeof(struct dogType);
}
