#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>

#define ELEMENTS 1

const char file[12] ="dataDogs.dat";


//funcion para continuar al presionar una tecla
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

// estructura con toda la información del registro
struct dogType {
    char name[32];
    char type[32];
    int age; //años
    char breed[16];
    int height;       //en cm
    float weight;     //en kg
    char sex;
};

//funcion para ingresar un registro en el programa
int enterRegistry(void *x) {
    struct dogType *p = x;
    printf("\n\n------------------------------------------------\n\nNombre del animal: ");
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
    printf("Peso (kg): ");
    fflush(stdout);
    scanf("%f", &p->weight);
    printf("Sexo (h/m): ");
    fflush(stdout);
    scanf(" %c", &p->sex); //se pone un espacio en blanco al comienzo del formato para que lea el char

    writeFile(file, p, sizeof(struct dogType), 1);

    printf("\n\n\n Su registro ha sido guardado con exito!\n\n\n");
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

//funcion para mostrar un registro
void showRegistry(int x) {
    struct dogType *dog= malloc(sizeof(struct dogType));
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

    printf("\n\nNombre del animal:  %s\n", dog->name);
    printf("Tipo:  %s\n", dog->type);
    printf("Edad:  %i\n", dog->age);
    printf("Raza:  %s\n", dog->breed);
    printf("Estatura (cm):  %i\n", dog->height);
    printf("Peso (kg):  %0.1f\n", dog->weight);
    printf("Sexo:  %c\n\n------------------------------------------------------", dog->sex);

    r = fclose(f);
    if(r != 0) {
        perror("fclose error showRegistry");
        exit(-1);
    }

    printf("\n\n\nEl registro ha sido mostrado, puede continuar usando el sistema.\n\n\n");
}

//funcion para validar la existencia de un registro
int validateRegistry(int number) {
    int maxNum = calculateNumberRegistrys();

    if(number <= 0 || number > maxNum) {
      printf("\n\n\n\t..................................................\n\nALERTA!: El numero ingresado no corresponde a algun registro del sistema!\nIngrese un numero entre 1 y %i\n\n\t..................................................",maxNum);
      return 0;
    }
    else {
      return 1;
    }
}

//funcion para abrir la historia clinica del animal
void openClinicalHistory(int x, int ind){
  FILE *f;
  struct dogType *dog= malloc(sizeof(struct dogType));
  int r;

  f = fopen(file,"r"); //vamos a leer el archivo! :3
  if(f == NULL) {
    perror("error fopen openClinicalHistory");
    exit(-1);
  }

  fseek(f, (x-1) * (sizeof(struct dogType)) + 4, SEEK_SET);
  r = fread(dog, sizeof(struct dogType), 1, f);
  if(r != ELEMENTS){
    perror("fread error openClinicalHistory");
    exit(-1);
  }

  r = fclose(f);
  if(r!=0){
    perror("fclose error openClinicalHistory");
    exit(-1);
  }

  char *file_Name;
  char *l_par="[";
  char *r_par="]";
  char *ext=".txt";
  char *title = "- HISTORIA_CLINICA";
  char s[100];

  file_Name = dog->name;

  strcat(file_Name, l_par);
  strcat(file_Name, r_par);
  strcat(file_Name, title);
  strcat(file_Name, ext);
  strcpy(s, file_Name);

  FILE *a = fopen(s, "r");

  if(a == NULL){
    a = fopen(s, "w+"); //si no existe, crea la historia clinica
  } else{
    a = fopen(s, "r"); //si existe, abre la historia clinica
  }

  r = fclose(a);
  if (r != 0) {
    perror("error fclose openClinicalHistory");
    exit(-1);
  }

  if(ind == -1){
    remove(file_Name);
    return;
  }

  if(!fork())
    execlp("gedit", "gedit", file_Name, NULL); //abrir la historia clinica
}

//funcion para saber si el usuario quiere abrir la historia clinica
int wantClinicalHistory() {
  char *answer;

  printf("\n\n\n------------ ¿Desea ver la historia clinica del animal? ( SI / NO )   ------------\n");
  scanf("%s", answer);

  minToMay(answer);

  if(strcmp(answer, "SI") == 0) {
    return 1;
  } else if (strcmp(answer, "NO") == 0) {
    return 0;
  } else {
    return 2;
  }
}

//funcion para borrar registros
void deleteRegistry(int x) {
    FILE* f;
    FILE* c;

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
      openClinicalHistory(x, -1); //se borra la historia clinica del animal
    }

    printf("\n\n\nEl registro ha sido eliminado exitosamente!\n\n\n");

    fclose(f);
    fclose(c);

    system("rm dataDogs.datr+");
    system("mv dataDogs.temp dataDogs.datr+");
}

//funcion para buscar registros
void searchRegistry(char *x) {
    FILE* f = fopen(file, "r");

    if(f == NULL) {  // Validamos errores
      perror("fopen error searchRegistry");
    }
    else {
      int total_Reg = calculateNumberRegistrys(), i;
      struct dogType* dog = malloc(sizeof(struct dogType));

      fseek(f, 4, SEEK_SET);

      for(i = 1 ; i <= total_Reg; i++) {
        fread(dog, sizeof(struct dogType), 1, f);

        minToMay(dog->name);
        minToMay(x);

        if(strcmp(dog->name, x) == 0) { //mostramos las mascotas que coincidan con el nombre (sin distinguir min y may)
          printf("\n\n\nREGISTRO NUMERO : %d\n\n",i );
          printf("Nombre del animal:  %s\n", dog->name);
          printf("Tipo:  %s\n", dog->type);
          printf("Edad:  %i\n", dog->age);
          printf("Raza:  %s\n", dog->breed);
          printf("Estatura (cm):  %i\n", dog->height);
          printf("Peso (kg):  %0.1f\n", dog->weight);
          printf("Sexo:  %c\n\n------------------------------------------------------", dog->sex);
        }
      }
    }

    printf("\n\n\nLa busqueda ha sido exitosa.\n\n\n");

    fclose(f);
}

//funcion para mostrar los registros y ofrecer opciones
void showNumberRegistrys(int type) {
  int number = calculateNumberRegistrys(), num_reg;
  int val = 0, petition = 0;

  if(number == 0) {
    printf("\n\nLo sentimos! Aun no se ha ingresado algun registro en el sistema.\n");
  }
  else {
    printf("\n\n------------ Cantidad de registros en el sistema:  %d ------------\n", number);
    if(type == 2) {
      printf("\n\nIngrese el numero del registro que desea consultar: ");
    }
    if(type == 3) {
      printf("\n\nIngrese el numero del registro que desea borrar: ");
    }
    scanf("%d", &num_reg);
    val = validateRegistry(num_reg);
    if(type == 2) {
      if(val != 0) {
        printf("\n\n\n------------ Mostrando registro N°  %d  ------------\n", num_reg);
        showRegistry(num_reg);
        petition = wantClinicalHistory();
        if(petition == 1) {
          openClinicalHistory(num_reg, 1);
        }
        else if(petition == 0) {
          printf("\n\n\nEl registro ha sido visualizado exitosamente!\n\n");
        }
        else {
          printf("\n\n\nEl comando es incorrecto!");
        }
      }
    }
    if(type == 3) {
      if(num_reg <= 0 || num_reg > number) {
        printf("\n\n\nEl registro no existe\n\n");
      } else {
        deleteRegistry(num_reg);
      }
    }
  }
}

//funcion para mostrar el menu en pantalla
void showMenu() {
    int *p_sel =  malloc(sizeof(int)); //apuntador a la variable que almacena el numero del menu ingresado por el usuario

    printf("\n\n\n\t\t\t\t------   MENU DE GESTION DE LA VETERINARIA  ------       \n\n\n");
    printf("1. Ingresar Registro\n");
    printf("2. Ver Registro\n");
    printf("3. Borrar Registro\n");
    printf("4. Buscar Registro\n");
    printf("5. Salir\n\n");
    printf("Por favor ingrese el numero de la opcion que desea ejecutar y presione Enter: ");
    fflush(stdout);

    scanf("%d", p_sel);

    FILE* f= fopen(file,"r+");

    //se verifica la existencia del archivo, si no existe, se crea
    if(f==NULL)
    {
        f=fopen(file,"w");
        int zero =0;
        int *p_z = &zero;

        int r = fseek(f,0,SEEK_SET); //se situa el indicador de pos. al inicio del archivo para leer el num
        if(r!=0)                   //de registros
        {
            perror("fseek error");
            exit(-1);
        }
        fwrite(p_z,sizeof(int),ELEMENTS,f);
        fclose(f);
        //printf("ARCHIVO CREADO");
    }


    if(*p_sel==1)
    {
        struct dogType *dog;
        dog = malloc(sizeof(struct dogType));
        enterRegistry(dog);
        free(dog);

    }
    else if(*p_sel==2)
    {


        showNumberRegistrys(2);


    }
    else if(*p_sel==3)
    {

        showNumberRegistrys(3);


    }
    else if(*p_sel==4)
    {

        int ind = calculateNumberRegistrys();

        if(ind!=0){

            printf("\nIngrese el registro que quiere buscar: \n");

            char search[20];

            scanf("%s",search);

            searchRegistry(search);
        }else{

            printf("\n\n\nLo sentimos, aun no hay registros en el sistema\n\n\n");

        }
    }
    else if (*p_sel==5)
    {

        printf("\n\n\nSALIENDO DEL SISTEMA......\n\n\n ");
        exit(-1);
    }
    else
    {
        printf("Seleccione una opcion correcta!");

    }

    printf("\n\nPor favor ingrese cualquier tecla para volver al menu principal: ");
    fflush(stdout);
    char a;
    keypress(a);
    keypress(a);


}



int main()
{

    int running=1;


    while(running==1)
    {


        showMenu();

    }


    return 0;
}
