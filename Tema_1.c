#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h> // Incluye la biblioteca para mmap()
#include <bits/mman-linux.h>
#include <fcntl.h>
#include <semaphore.h>
#include <math.h>
#include <string.h>
//Solo en UNIX Linux
#define MAX_SIZE 100
// static int arr[] = {5,4,8,9,3,1,4,7,8,9,5,4,8,7,9,6};

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
 
    // Create temp arrays
    int L[n1], R[n2];
 
    // Copy data to temp arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
 
    // Merge the temp arrays back into arr[l..r
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    // Copy the remaining elements of L[],
    // if there are any
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    // Copy the remaining elements of R[],
    // if there are any
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}


void printaArray(int *arr,int start,int end){
    printf("{");
    for (int i = start; i < end; i++) {
        printf("%d,",arr[i]);
    }
    printf("%d}",arr[end]);
}
void imprimir(int *arr, int l, int r,int mid){
    printaArray(arr,l,r);

    for(int i=0;i<32-(r-l)*2;i++){
        printf(" ");
    }
    printf("\t<==\t");

    printaArray(arr,l,mid);

    for(int i=0;i<16-(mid-l)*2;i++){
        printf(" ");
    }
    printf("-\t");
    printaArray(arr,mid+1,r);
    printf("\n");
}
void printProcess(int *arr, int l, int r, int mid,char* izq, char* der){
    printaArray(arr,l,r);

    for(int i=0;i<32-(r-l)*2;i++){
        printf(" ");
    }
    printf("\t<==\t");

    // printaArray(arr,l,mid);
    printf("%s",izq);

    for(int i=0;i<16-(mid-l)*2;i++){
        printf(" ");
    }
    printf("-\t");
    // printaArray(arr,mid+1,r);
    printf("%s",der);
    printf("\n");
}

char *stringArray(int *arr,int start,int end){
    // char a[(end-start)*2+4];
    char *a = malloc((end - start) * 2 + 4);
    // char *a = (char*)mmap(NULL, (end - start) * 2 + 4, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    a[0]='{';
    for (int i = start; i < end; i++) {
        a[(i-start)*2+1] = arr[i] + '0';
        a[(i-start)*2+2] = ',';
    }
    // printf("%d}",arr[end]);
    a[(end-start)*2+1]=arr[end] + '0';
    a[(end-start)*2+2]='}';
    a[(end-start)*2+3]='\0';
    return a;
}
void burbuja(int arr[], int start,int end) {
    int i, j, temp;
    for (i = start; i < end; i++) {
        for (j = start; j < end-1; j++) {
            if (arr[j] > arr[j+1]) {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}
// Función de ordenamiento Merge Sort
void mergeSort(int arr[], int l, int r, int *n,int *left,int fd[],sem_t *sem, int nproc) {
    int status;
    if (l < r) {
        *n=*n+100;
        int num_p = *n;
        int mid = l + (r - l) / 2;
        if(num_p/100>nproc) {
            *left=0;
            mergeSort(arr, l, mid,n,left,fd,sem,nproc);
            mergeSort(arr, mid + 1, r,n,left,fd,sem,nproc);
        } else {
            // sem_wait(sem);
            // printf("|%2d|\t",num_p/100);
            // // char * pruebas = stringArray(arr,l,r);
            // // printf("%s\n",pruebas);
            // printaArray(arr,l,r);
            // printf("\n");
            // // free(pruebas);
            // // usleep(1);
            // sem_post(sem);
            usleep(100);
            while(*left){}
            int lenght = r-l+1;
            sem_wait(sem);
            write(fd[1],&lenght,sizeof(int));
            for(int i=l;i<=r;i++){
                write(fd[1],&arr[i],sizeof(int));
            }
            sem_post(sem);
            //close(fd[1]); crash
            *left=1;
            pid_t left_child = fork();
            if (left_child == 0) { // Proceso hijo
                // usleep(1);
                close(fd[0]);
                mergeSort(arr, l, mid,n,left,fd,sem,nproc);
                close(fd[1]);
                exit(EXIT_SUCCESS);
            }
            pid_t right_child = fork();
            if (right_child == 0) { // Proceso hijo
                usleep(100);
                *left=0;
                close(fd[0]);
                mergeSort(arr, mid + 1, r,n,left,fd,sem,nproc);
                close(fd[1]);
                exit(EXIT_SUCCESS);
            }
            // Proceso padre
            // Esperar a que ambos hijos terminen
            waitpid(right_child, &status, 0);
            waitpid(left_child, &status, 0);
        }
        
        char* izq;
        char* der;
        int izqArray[mid-l+1];
        int derArray[r-mid];
        sem_wait(sem);
        if(num_p/100<=nproc){ 
            izq = stringArray(arr,l,mid);
            der = stringArray(arr,mid+1,r);

            for (int i = l; i <= mid; i++) {
                izqArray[i-l] = arr[i];
            }
            for (int i = mid+1; i <= r; i++) {
                derArray[i-mid-1] = arr[i];
            }
            
        }

        // Fusionar los dos sub-arreglos ordenados

        merge(arr, l, mid, r);
        if(num_p/100<=nproc){
            // sleep(1.5);
            // printf("|%2d|\t",num_p/100);
            // imprimir(arr,l,r,mid);
            // printProcess(arr,l,r,mid,izq,der);

            int lenght = mid-l+1;
            write(fd[1],&lenght,sizeof(int));
            for (int i = 0; i <= mid-l; i++) {
                write(fd[1],&izqArray[i],sizeof(int));
            }
            lenght = r-mid;
            write(fd[1],&lenght,sizeof(int));
            for (int i = 0; i < r-mid; i++) {
                write(fd[1],&derArray[i],sizeof(int));
            }
            lenght = r-l+1;
            write(fd[1],&lenght,sizeof(int));
            for (int i = l; i <= r; i++) {
                write(fd[1],&arr[i],sizeof(int));
            }
            free(izq);
            free(der);
        }
        sem_post(sem);
        // munmap(izq, sizeof(char)*((l - mid) * 2 + 4));
        // munmap(der, sizeof(char)*((mid+1 - r) * 2 + 4));
        // sleep(0.1);
    }
}


void print_heap(int **heap, int n) {
    int height = log2(n) + 1;
    int k = 0;
    int rotar = 1;
    for (int level = 0; level < height-1; level++) {
        int num_nodes = pow(2, level);

        // Imprimir espacios antes de los nodos
        int ant = k;
        int spaces_before = (int)(pow(2, height - level - 1) - 1)/2;
        for (int i = 0; i < spaces_before; i++) {
            printf("    ");
        }

        // Imprimir nodos en este nivel
        for (int i = 0; i < num_nodes && k < n; i++, k++) {
            if(k!=0){
                if(rotar){
                    printf("     ");
                }else{
                    printf("   \\ ");
                }
                rotar=!rotar;
            }else{
                continue;
            }

            // Imprimir espacios entre los nodos
            int spaces_between = (int)pow(2, height - level) - 1;
            for (int j = 0; j < spaces_between; j++) {
                if(!rotar && j==0)
                    printf("/    ");
                else
                    printf("     ");
            }
        }
        printf("\n");

        for (int i = 0; i < spaces_before; i++) {
            printf("    ");
        }
        k=ant;
        // Imprimir nodos en este nivel
        for (int i = 0; i < num_nodes && k < n; i++, k++) {
                int jj = 0;
                for (jj = 1; jj < heap[k][0]; jj++) {
                    printf("%d,",heap[k][jj]);
                }
                printf("%d",heap[k][jj]);
            // Imprimir espacios entre los nodos
            int spaces_between = (int)pow(2, height - level) - 1;
            for (int j = 0; j < spaces_between; j++) {
                printf("    ");
            }
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int n = 16;
    char **mapeo = (char**)mmap(NULL, sizeof(char*)*7, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for(int i=0;i<7;i++){
        *(mapeo+i)=(char*)mmap(NULL, sizeof(char)*16, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }
    int *left = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *left = 0;
    // int count = 0;
    int *number = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *number = 0;

    
    int *arr = (int*)mmap(NULL, sizeof(int)*16, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
     // Obtener el primer token
    char *token = strtok(argv[2], ",");
    int i=0;
    // Recorrer los tokens restantes
    while (token != NULL) {
        // Convertir el token a entero y almacenarlo en el array
        arr[i] = atoi(token);
        i++;

        // Obtener el siguiente token
        token = strtok(NULL, ",");
    }
    int lenghtArg = i-1;

    sem_t *sem;
    char *sem_name = "/mi_semaforo";

    // Crear un semáforo para sincronizar el acceso a la tubería
    sem = sem_open(sem_name, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    int fd[2];
    int nproc = atoi(argv[1]);
    pipe(fd);
    // Llamar a la función de ordenamiento Merge Sort
    mergeSort(arr, 0, n - 1,number,left,fd,sem,nproc);
    int lenght = 0;
    int newArr[50];


    int **map = (int**)malloc(sizeof(int*)*nproc);
    for(int i=0;i<nproc;i++){
        if(read(fd[0],&lenght,sizeof(int))<0){
            printf("problem lenght");}
        if(read(fd[0],newArr,sizeof(int)*lenght)<0){
            printf("problem array");}
        *(map+i)=(int*)malloc(sizeof(int)*(lenght+1));
        map[i][0]=lenght;
        for (int j = 1; j <= lenght; j++) {
            map[i][j]=newArr[j-1];
        }
    }

    printf("=========================Esquema de Arbol===========================\n");
    print_heap(map, lenghtArg);

    printf("=========================Mapeos===========================\n");
    for(int i=0;i<nproc;i++){
        printf("|%2d|\t[",i);
        int j;
        for (j = 1; j < map[i][0]; j++) {
            printf("%d,",map[i][j]);
        }
        printf("%d]\n",map[i][j]);
        free(map[i]);
    }
    free(map);
    int **proc = (int**)malloc(sizeof(int*)*nproc);
    int **izq = (int**)malloc(sizeof(int*)*nproc);
    int **der = (int**)malloc(sizeof(int*)*nproc);
    int maxizq = 0;
    int maxder = 0;
    for(int i=0;i<nproc;i++){
        if(read(fd[0],&lenght,sizeof(int))<0){
            printf("problem lenght");}
        if(read(fd[0],newArr,sizeof(int)*lenght)<0){
            printf("problem array");}
        *(izq+i)=(int*)malloc(sizeof(int)*(lenght+1));
        izq[i][0]=lenght;
        if(maxizq<lenght){
            maxizq=lenght;
        }
        for (int j = 1; j <= lenght; j++) {
            izq[i][j]=newArr[j-1];
        }
        if(read(fd[0],&lenght,sizeof(int))<0){
            printf("problem lenght");}
        if(read(fd[0],newArr,sizeof(int)*lenght)<0){
            printf("problem array");}
        *(der+i)=(int*)malloc(sizeof(int)*(lenght+1));
        der[i][0]=lenght;
        if(maxder<lenght){
            maxder=lenght;
        }
        for (int j = 1; j <= lenght; j++) {
            der[i][j]=newArr[j-1];
        }
        if(read(fd[0],&lenght,sizeof(int))<0){
            printf("problem lenght");}
        if(read(fd[0],newArr,sizeof(int)*lenght)<0){
            printf("problem array");}
        *(proc+i)=(int*)malloc(sizeof(int)*(lenght+1));
        proc[i][0]=lenght;
        for (int j = 1; j <= lenght; j++) {
            proc[i][j]=newArr[j-1];
        }
        // printaArray(proc[i],1,proc[i][0]);
        // printf("\n");
    }
    printf("=======================================Procesamiento=======================================\n");
    for(int i=0;i<nproc;i++) {
        printf("|%2d|\t[",i);
        int j;
        for (j = 1; j < izq[i][0]; j++) {
            printf("%d,",izq[i][j]);
        }
        printf("%d]",izq[i][j]);
        for(j=2*izq[i][0];j<2*maxizq;j++){
            printf(" ");
        }
        printf("\t-\t[%d",izq[i][j]);

        for (j = 1; j < der[i][0]; j++) {
            printf("%d,",der[i][j]);
        }
        printf("%d]",der[i][j]);
        for(j=2*izq[i][0];j<2*maxizq;j++){
            printf(" ");
        }
        printf("\t==>\t[%d",der[i][j]);

        for (j = 1; j < proc[i][0]; j++) {
            printf("%d,",proc[i][j]);
        }
        printf("%d]\n",proc[i][j]);
        free(izq[i]);
        free(der[i]);
        free(proc[i]);
    }
    free(izq);
    free(der);
    free(proc);

    close(fd[1]);
    close(fd[0]);
    munmap(number, sizeof(int));
    munmap(arr, sizeof(int)*16);
    munmap(left, sizeof(int));
    // for (int i = 0; i < 7; i++) {
    //     printf("|%2d|\t%s\n",i,mapeo[i]);
    // }
    // printf("\n");
    sem_close(sem);
    sem_unlink(sem_name);
    return 0;
}
