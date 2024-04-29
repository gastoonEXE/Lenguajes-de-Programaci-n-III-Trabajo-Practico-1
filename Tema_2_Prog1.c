//////////////////////////          Ejercicio 2 - Programa 1       ////////////////////////////
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

typedef struct{     //estructura para almacenar datos de archivo
  int procesoID;
  int senal;
  int segundos;
}Procesos;

Procesos tabla[2];  //creamos una tabla de tipo de Procesos de acuerdo a cantidad de datos de archivo

int main(){
  
  Procesos dato;    //declaramos dato de tipo Procesos para almacenar los cada dato en la estructura

  int pid;
  int i=0;
  int estado;
  FILE *myPtr;
  myPtr = fopen("planificacion_señales.txt","r");   //leemos datos del archivo
  if (myPtr == NULL){
    printf("No se pudo abrir el archivo\n");
  }
  char palabra[100];          
  while (!feof(myPtr)){
    memset(palabra,0,100);
    fgets(palabra,100,myPtr);   //en char palabra almcenamos el contenido de cada linea del archivo
    sscanf(palabra,"%d %d %d",&dato.procesoID,&dato.senal,&dato.segundos);  //obtenemos cada uno de los 3 datos que nos brinda el archivo
    tabla[i] = dato;     
    i++;
  }
  int cant_avisos = 3;      //en este caso la cantidad de avisos es 3
  int aviso_actual = 0;
  while (aviso_actual<cant_avisos){
    pid = fork();     
    if (pid==0){
      sleep(tabla[aviso_actual].segundos);    //dormimos el proceso de acuerdo al dato segundos
      printf("Tardo %d segundos, tengo el id: %d y la senal: %d\n",tabla[aviso_actual].segundos,tabla[aviso_actual].procesoID,tabla[aviso_actual].senal);
      kill(getpid(),SIGTERM);    //una vez que realiza el printf terminamos el proceso hijo
    }
    else if (pid>0){
      //waitpid(pid,&estado,0);
    }
    aviso_actual=aviso_actual+1;     //contador para las señales
  }
  aviso_actual = 0;
  while (aviso_actual < cant_avisos){
    wait(NULL);
    aviso_actual += 1;
  }
  
  fclose(myPtr);
  return 0;
}

