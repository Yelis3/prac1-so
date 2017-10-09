#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "pthread.h"
#include <termios.h> //libreria para la funcion keypress

#define ELEMENTS 1

const char file[12] = "dataDogs.dat"; //archivo .dat para insercion y borrado de registros

struct Node* hashArray[2000]; //tabla hash

void minToMay(char string[]);

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

//funcion para leer un archivo con el nombre 'file'
void readFile(char file[], void *data, size_t size, int elements)
{
    FILE *f;
    int r;

    f = fopen(file, "r");
    if(f == NULL) {
        perror("error fopen readFile");
        exit(-1);
    }

    rewind(f);
    r = fread(data, size, elements, f);
    if(r != elements) {
        perror("error fread readFile");
        exit(-1);
    }
}

//funcion para recibir los datos de la estructura
int enterRegistry(void *x) {
    struct dogType *p = x;
    printf("\n:::::::::::::::::::::::::::::::::::::::::::::INGRESO DE LOS DATOS DE LA MASCOTA::::::::::::::::::::::::::::::::::::::::::::::::::::\nNombre del animal: ");
    fflush(stdout);
    scanf(" %s", p->name);
    printf("Tipo: ");
    fflush(stdout);
    scanf(" %s", p->type);
    printf("Edad: ");
    fflush(stdout);
    scanf(" %i", &p->age);
    printf("Raza: ");
    fflush(stdout);
    scanf(" %s", p->breed);
    printf("Estatura (cm): ");
    fflush(stdout);
    scanf(" %i", &p->height);
    printf("Peso (kg): ");
    fflush(stdout);
    scanf(" %f", &p->weight);
    fflush(stdout);
    printf("Sexo (h/m): ");
    fflush(stdout);
    scanf("  %c", &p->sex); //se pone un espacio en blanco al comienzo del formato para que lea el char
    printf("\n::::::::::::::::::::::::::::::::::::::::::::::::::CONFIRMACION DEL SISTEMA:::::::::::::::::::::::::::::::::::::::::::::::::::::::::");

}

//funcion para escribir la estructura en el archivo .dat

void writeRegistry(void* x){

	struct dogType *p = x;

	writeFile(file, p, sizeof(struct dogType), 1); //se escribe el registro en el archivo .dat

}

//funcion para mostrar en pantalla el numero de registros disponibles hasta ahora
int calculateNumberRegistrys() {
    FILE *f = fopen(file,"r");
    int d, r;

    //verificar si se ya se ha ingresado AL MENOS un registro
    if(f == NULL) {
      perror("error fopen calculateNumberRegistrys");
      exit(-1);
    }

    r = fseek(f, 0, SEEK_SET); //se situa el indicador de pos. al inicio del archivo para leer el num de registros
    if(r != 0) {
      perror("fseek error calculateNumberRegistrys");
      exit(-1);
    }

    r = fread(&d, sizeof(int), ELEMENTS, f);
    if(r != ELEMENTS) {
      perror("fread error calculateNumberRegistrys");
      exit(-1);
    }

    return d;
}

//funcion para pasar al cliente el registro que desea ver
char* showRegistry(int x, void*p) {
    struct dogType *dog= p;

    FILE *f;
    int r;

    f = fopen(file, "r"); //vamos a leer el archivo! :3
    if(f == NULL) {
      perror("error fopen showRegistry");
      exit(-1);
    }

    fseek(f, (x-1) * (sizeof(struct dogType)) + 4, SEEK_SET); //lleva el indicador de pos al inicio del registro que buscamos

    r = fread(dog, sizeof(struct dogType), 1, f); //vamos a leerlo! ... otra vez ¬¬
    if(r != ELEMENTS) {
        perror("fread error showRegistry");
        exit(-1);
    }

    r = fclose(f);
    if(r != 0) {
        perror("fclose error showRegistry");
        exit(-1);
    }

    return dog->name;

    
}

//funcion para mostrar el registro en el cliente

void showRegs(void *x){


	struct dogType *dog= x;
	   
    printf("\n\nNombre del animal:  %s\n", dog->name); //se muestra el registro con todos sus atributos
    printf("Tipo:  %s\n", dog->type);
    printf("Edad:  %i\n", dog->age);
    printf("Raza:  %s\n", dog->breed);
    printf("Estatura (cm):  %i\n", dog->height);
    printf("Peso (kg):  %0.1f\n", dog->weight);
    printf("Sexo:  %c\n\n", dog->sex);

}




//funcion para validar la existencia de un registro
int validateRegistry(int number) {
    int maxNum = calculateNumberRegistrys();

    if(number <= 0 || number > maxNum) {
      printf("\n:::::::::::::::::::::::::::::::::::::::::::::MENSAJE DEL SISTEMA::::::::::::::::::::::::::::::::::::::::::::::::::::::"); 	
      printf("\n\n\nALERTA!: El numero ingresado no corresponde a algun registro del sistema!\nIngrese un numero entre 1 y %i\n\n\t",maxNum);
      return 0;
    }
    else {
      return 1;
    }
}

//funcion para borrar registros
void deleteRegistry(int x) {
    FILE* f;
    FILE* c;
    int r;

    f = fopen(file, "r");
    if(f == NULL) { // Validamos errores
      perror("fopen error deleteRegistry");
    }

    // Creamos un archivo copia para eliminar el animal y escribimos el número de registros
    c = fopen("dataDogs.temp", "w");
    if(c == NULL) { // Validamos errores
      perror("fopen error deleteRegistry");
    } else {
      struct dogType* dog = malloc(sizeof(struct dogType));
      int total_Reg = calculateNumberRegistrys(); // Leemos el número de registros existentes
      int i;

      total_Reg -= 1; //se resta a la cant de registros el animal que se va a eliminar

      fseek(c, 0, SEEK_SET);
      fwrite(&total_Reg, sizeof(int), 1, c); //se escribe en el temporal la cant de registros
      fseek(f, 4, SEEK_SET);

      total_Reg += 1; //Lo aumentamos de nuevo para hacer la copia correctamente

      // Recorremos todo el archivo haciendo copia de todos excepto de ese registro
      for(i = 1 ; i <= total_Reg ; i++) {
        if(i == x) {
          fread(dog, sizeof(struct dogType), 1, f); // Excluimos el registro de número numReg
      

        } else {
          fread(dog, sizeof(struct dogType), 1, f); // Leemos y escribimos del original a la copia
          fwrite(dog, sizeof(struct dogType), 1, c);
        }
      }
      free(dog);
    }

    r = fclose(f);
    if (r != 0) {
	    perror("error fclose deleteRegistry file f");
	    exit(-1);
 	}
    r = fclose(c);
    if (r != 0) {
	    perror("error fclose deleteRegistry file c");
	    exit(-1);
 	}

    system("rm dataDogs.datr+");
    system("mv dataDogs.temp dataDogs.datr+");


}

//funcion para buscar registros
void searchRegistry(char *x, int clientfd) {
    FILE* f = fopen(file, "r");
    int r, control=0;
    int invalid = -1;

    if(f == NULL) {  // Validamos errores
      perror("fopen error searchRegistry");
    }
    else {
      int total_Reg = calculateNumberRegistrys(), a=1, finish=0, i;
      struct dogType* dog = malloc(sizeof(struct dogType));

      fseek(f, 4, SEEK_SET);

      for(i = 1 ; i <= total_Reg; i++) {
        fread(dog, sizeof(struct dogType), 1, f);

        minToMay(dog->name);
        minToMay(x);


        if(strcmp(dog->name, x) == 0) {
          r = send(clientfd, &i, sizeof(int), 0);		

          r = send(clientfd, dog, sizeof(struct dogType), 0);	
        
         //mostramos las mascotas que coincidan con el nombre (sin distinguir min o may)
       
        }else{
           r = send(clientfd, &invalid, sizeof(int), 0);	
           
        }


        if(i==total_Reg){
        	r = send(clientfd, &finish, sizeof(int), 0);	
        }else{
        	r = send(clientfd, &a, sizeof(int), 0);	
        }
        
      }


      free(dog);
    }
    r = fclose(f);
    if (r != 0) {
	    perror("error fclose searchRegistry");
	    exit(-1);
 	}
}

//funcion para determinar el tamano del archivo
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