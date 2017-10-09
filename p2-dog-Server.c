#include "mainMethods.c"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "pthread.h"
#define PORT 3563
#define MAX_CLIENTS 32

//variables globales para manejar el numero de clientes conectados al server, y la escritura en los archivos .dat y .log
int total_clients = 0;
int writeLog = 0,writeInFile = 0;

void logWrite(int type,char registry[32],int socket);
void *clientManager(void *socket);

//funcion de gestion del servidor
void server(){

  struct sockaddr_in server, client;
  int serverSocket, r, optval,s;
  socklen_t optlen = sizeof(optval);
  socklen_t size = sizeof(struct sockaddr_in);
  pthread_t hilo;
  
  serverSocket = socket(AF_INET,SOCK_STREAM,0);  
  if(serverSocket == -1){
	perror("Socket creation error ");
	exit(-1);
  }
  //llenar campos para la conexion
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = INADDR_ANY;
  bzero(server.sin_zero,8);
 
  r = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, optlen);
  if(r==-1){
	perror("Setsockopt error");
	exit(-1);
  } 

  r = bind(serverSocket,(struct sockaddr*)&server,sizeof(struct sockaddr_in));
  if(r == -1){
	perror("Bind error ");
	exit(-1);
  }

  while(1){
	
    if(total_clients == MAX_CLIENTS){
	  continue;
	}
	r = listen(serverSocket,MAX_CLIENTS);
	if(r == -1){
      perror("Listen error ");
	  exit(-1);
	}
	r = accept(serverSocket,(struct sockaddr*)&client,(socklen_t*)&size);
	if(r == -1){
	  perror("Accept error ");
	  exit(-1);
	}
	//creacion del hilo para gestionar la comunicacion con el cliente
	int s = pthread_create(&hilo,NULL,(void*)clientManager,(void*)&r);
	total_clients++; //se aumenta el numero de clientes conectados al servidor
	sleep(2);
	if(s != 0){
      perror("Thread creation error ");
	  exit(-1);
	}
  }
}

//rutina que ejecuta cada hilo creado para gestionar la comunicacion con el cliente
void *clientManager(void *socket){
  int serverfd = *(int*)socket;
  int r, option, flag = 1, verification=1, validation=1, total_reg, num_reg, val;	
  char buffer[32];
  FILE* f;
  
  while(verification==1){
	if(writeInFile== 0){//se puede escribir en el archivo .dat porque ningun otro hilo lo esta haciendo
      writeInFile= 1;
	  verification=0;
	}
  }

  f = fopen(file,"r+");
  //se verifica la existencia del archivo, si no existe, se crea
  if(f == NULL)
  {
    int zero = 0, r;
    f = fopen(file,"w");
    if(f == NULL) {  // Validamos errores
   	  perror("fopen error clientManager");
    }
    r = fseek(f,0,SEEK_SET); //se situa el indicador de pos. al inicio del archivo para leer el num de registros
    if(r!=0)                  
    {
      perror("fseek error clientManager");
	  exit(-1);
	}
    fwrite(&zero,sizeof(int),ELEMENTS,f);
    r = fclose(f);
    if (r != 0) {
      perror("error fclose clientManager");
      exit(-1);
 	}
  }
  while(flag == 1){
    r = recv(serverfd,&option,sizeof(int),0);
	if(r != sizeof(int)){
	  perror("Recv Error 1 ");
	  exit(-1);
	}
	if(option==1){
	  struct dogType *dog;
      dog = malloc(sizeof(struct dogType));
      r = recv(serverfd,dog,sizeof(struct dogType),0);
	  if(r != sizeof(struct dogType)){
	    perror("Recv Error 2 ");
		exit(-1);
      }
      logWrite(1,dog->name,serverfd);
      writeRegistry(dog);
      r = send(serverfd, &validation, sizeof(int), 0);
      if(r==-1){
		  perror("Send error 1");
		  exit(-1);		
	  }
      free(dog);

	}else if (option==2){

	  struct dogType *dog;
      dog = malloc(sizeof(struct dogType));	
	  total_reg = calculateNumberRegistrys();
	  
	  r = send(serverfd, &total_reg, sizeof(int), 0);
	  if(r==-1){
		  perror("Send error 2");
		  exit(-1);		
	  }
	  r = recv(serverfd,&num_reg,sizeof(int),0);
	  if(r != sizeof(int)){
	    perror("Recv Error 3 ");
		exit(-1);
      }
      if(num_reg!=-1){
	    showRegistry(num_reg, dog);
        sprintf(buffer, "%d", num_reg);
        logWrite(2,buffer,serverfd);
        r = send(serverfd, dog, sizeof(struct dogType), 0);
        if(r==-1){
		  perror("Send error 3");
		  exit(-1);		
	    }
	  }
	  free(dog);
	  
	}else if (option==3){
	   total_reg = calculateNumberRegistrys();
	   r = send(serverfd, &total_reg, sizeof(int), 0);
       if(r==-1){
		  perror("Send error 4");
		  exit(-1);		
	   }
	   r = recv(serverfd,&num_reg,sizeof(int),0);
	   if(r != sizeof(int)){
	     perror("Recv Error 4 ");
		 exit(-1);
       }
       if(num_reg!=-1){

	   deleteRegistry(num_reg);	
       sprintf(buffer, "%d", num_reg);
       r = send(serverfd, &validation, sizeof(int), 0);
       if(r==-1){
	     perror("Send error 5");
		 exit(-1);		
	   }
       logWrite(3,buffer,serverfd);
       }	

    }else if (option==4){

	  total_reg = calculateNumberRegistrys();
	  r = send(serverfd, &total_reg, sizeof(int), 0);
	  if(r==-1){
	     perror("Send error 6");
		 exit(-1);		
	  }
	  r = recv(serverfd,&num_reg,sizeof(int),0);
	  if(r != sizeof(int)){
	     perror("Recv Error 5");
		 exit(-1);
      }
	  if(num_reg!=-1){
        r = recv(serverfd,buffer,sizeof(buffer),0);
        if(r != sizeof(buffer)){
	     perror("Recv Error 6 ");
		 exit(-1);
        }
	    searchRegistry(buffer,serverfd);
	    logWrite(4,buffer,serverfd);
      }

	}else if(option == 5) {
	  total_clients--;
	  sleep(3);
	  r = recv(serverfd, &val, sizeof(int), 0);
	  if(r != sizeof(int)){
	    perror("Recv Error 7 ");
		exit(-1);
      }
	  if(val==-1){
	    break;
	  }

	}else{

	  r = recv(serverfd, &val, sizeof(int), 0);
	  if(val==-1){
	    continue;
	  }
			
	}
  	writeInFile = 0;
		
  }

  close(serverfd); //se cierra el socket que estaba comunicandose con el cliente
	
}

//funcion para la gestion del archivo .log
void logWrite(int type,char registry[32],int socket){ //types 1 - Insercion, 2 - Lectura, 3 - Borrado, 4 - Busqueda
  int verification=1, r;

  while(verification==1){
    if(writeLog == 0){//se puede escribir porque ningun proceso tiene abierto el archivo .log
      writeLog = 1;
	  verification=0;
    }
  }

  //se abre el archivo .log en modo 'append'
  FILE *fileLog = fopen("serverDogs.log","a");
  if(fileLog == NULL){
    perror("Error al abrir o crear el archivo log: ");
	exit(-1);
  }
  //se obtiene la fecha, hora y la IP del cliente
	time_t timer;
	struct tm* tm_info;
	char date[26];
	time(&timer);
	tm_info = localtime(&timer);
	strftime(date, 26, "%Y:%m:%d %H:%M:%S", tm_info);

	//se recoge la informacion del cliente
	struct sockaddr_storage addr;
	char clientip[INET_ADDRSTRLEN];
	socklen_t len = sizeof addr;
	getpeername(socket, (struct sockaddr*) &addr, &len);
	struct sockaddr_in *s = (struct sockaddr_in *) &addr;
	inet_ntop(AF_INET, &s->sin_addr, clientip, sizeof clientip);	
   	char option[15];

   	//se guarda en la cadena option el tipo de operacion que se esta realizando
	if(type == 1){
	  strcpy(option,"Insercion");
	}else if(type == 2){
	  strcpy(option,"Lectura");
    }else if(type == 3){
      strcpy(option,"Borrado"); 
    }else{
      strcpy(option,"Busqueda");
    }
	
	//Se guarda toda la informacion de la operacion en el archivo .log
	r = fprintf(fileLog,"|%s|	Cliente [%s] |%s| |%s|\n",date,clientip,option,registry);
	if(r < 1){
		perror("Fprintf error");
	}
	//se cierra el archivo para que otros procesos puedan escribir en el
	fclose(fileLog);

	writeLog = 0;
	verification=1;
}

int main(){

  server();
  return 0;
}