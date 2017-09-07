#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h> //libreria para la funcion keypress

#define ELEMENTS 1

const char file[12] = "dataDogs.dat"; //archivo .dat para insercion y borrado de registros

struct Node* hashArray[2000]; //tabla hash

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

//funcion para escribir una estructura dogType en un archivo con el nombre 'file'
void writeFile(const char file[], void *data, size_t size, int elements) {
    FILE *f;
    int d, r;

    f = fopen(file, "r+"); //se abre el archivo para leer/escribir
    if(f == NULL) {
        perror("error fopen1 writeFile");
        exit(-1);
    }

    fseek(f, 0, SEEK_SET); //ponemos el indicador de pos. al inicio del archivo
    r = fread(&d, sizeof(int), ELEMENTS, f); //se verifica cuantos registros hay en el archivo hasta ahora

    
    if(r != ELEMENTS) {  //la sentencia se ejecuta si el archivo no tiene ningun registro
      perror("error fread1 writeFile");
    } else {  //se ejecuta si hay al menos un registro
      d += 1; //se incrementa el num de registros en 1
      fseek(f, 0, SEEK_SET);  //ponemos el indicador de pos. al inicio del archivo
      fwrite(&d, sizeof(int), ELEMENTS, f); //se sobreescribe el num de registros con el valor nuevo
    }

    r = fclose(f); //se cierra el archivo
    if(r != 0) {
        perror("error fclose1 writeFile");
        exit(-1);
    }

    f = fopen(file, "a"); //se abre el archivo en modo de escritura append
    if(f == NULL) {
        perror("error fopen2 writeFile");
        exit(-1);
    }

    r = fwrite(data, size, elements, f); //ahora se puede escribir al final del archivo la estructura
    if(r != elements) {
      perror("error fwrite2 writeFile");
      exit(-1);
    }

    r = fclose(f); //cerramos el archivo
    if (r != 0) {
      perror("error fclose2 writeFile");
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

//funcion para continuar al presionar cualquier tecla Fuente : StackOverFlow
int keypress(unsigned char echo) {
  struct termios savedState, newState;
  int c;
  if (-1 == tcgetattr(STDIN_FILENO, &savedState)){
    return EOF;     /* error on tcgetattr */
  }
  newState = savedState;
  if ((echo = !echo)){ /* yes i'm doing an assignment in an if clause */
  echo = ECHO;    /* echo bit to disable echo */
  }
  /* disable canonical input and disable echo.  set minimal input to 1. */
  newState.c_lflag &= ~(echo | ICANON);
  newState.c_cc[VMIN] = 1;
  if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &newState)){
    return EOF;     /* error on tcsetattr */
  }
  c = getchar();      /* block (withot spinning) until we get a keypress */
  /* restore the saved state */
  if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &savedState)){
    return EOF;     /* error on tcsetattr */
  }
  return c;
}

//funcion para convertir de minusculas a mayusculas
void minToMay(char string[]) {
  int i, desp = 'a'-'A';
  for(i = 0; string[i] != '\0'; ++i) {
    if(string[i] >= 'a' && string[i] <= 'z') {
      string[i] = string[i] - desp;
    }
  }
}