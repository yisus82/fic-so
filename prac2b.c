/*          PRACTICA 2 APARTADO B       */
/*Simulacion de un sistema con deteccion*/
/*Autores:I.Carla Suarez Casal          */
/*        Ivan Gomez Rodriguez          */

#include <stdio.h>
#include <stdlib.h>

int N;       //numero de tipos de recurso
int M;       //numero de procesos

int **asignados;
int *disponible;
int **solicitud;
char *procespera;
int *liberacion;

int i,j;
int proc=0,opc=0,numins;
char val;

void deteccion(int numrec,int numproc,int **sol,int **asig,int *disp);
int entero (char *cadena);

main(int argc,char **argv)
{
 if (argc<3) {printf ("No hay suficientes argumentos\n"); return 0;}

 N=argc-2;
 M=entero(argv[argc-1]);

 disponible=(int *)calloc(N,sizeof(int));
 procespera=(char *)calloc(M,sizeof(char));

 asignados=(int **)calloc(M,sizeof(int *));
 for(j=0;j<M;j++)
		asignados[j]=(int *)calloc(N,sizeof(int));
 solicitud=(int **)calloc(M,sizeof(int *));
 for(j=0;j<M;j++)
		solicitud[j]=(int *)calloc(N,sizeof(int));

 for(j=1;j<=(argc-2);j++) disponible[j-1]=entero(argv[j]);

 do{
    do{
       printf("\n");
       printf("1. Solicitud\n2. Liberacion\n3. Abortar\n4. Est.Procesos\n");
       printf("5. Salir\n\n");
       scanf("%d",&opc);
     }while((opc<1)||(opc>5));
	
     if(opc==5) continue;
     if(opc!=4){
       do{
	  printf("Seleccione proceso: ");
          scanf("%d",&proc);
       }while(((proc>M)||(proc<1)));
     }

     switch(opc){
        case 1: //solicitud
	   {if (procespera[proc-1]==0){
	      j=0;
	      for(j=0;j<N;j++){
		 printf("Instancias recurso %d? ",j+1);
		 scanf("%d",&solicitud[proc-1][j]);
	      }
	      val=1;
	      for(j=0;j<N;j++){
		  if(solicitud[proc-1][j]>disponible[j])
						{val=0;break;}
	      }
	      if(val){
	  	for(j=0;j<N;j++){
		  asignados[proc-1][j]=asignados[proc-1][j]+solicitud[proc-1][j];
		  disponible[j]=disponible[j]-solicitud[proc-1][j];
		  solicitud[proc-1][j]=0;
		}
	       }

	       else
		{procespera[proc-1]=1;
		 printf("Proceso %d puesto en espera\n",proc);
	         deteccion(N,M,solicitud,asignados,disponible);
		}
	    }
	    else printf("Proceso en espera no puede solicitar\n");
	    continue;}/*case1*/

	case 2:{ //liberacion
	    liberacion=(int *)calloc(N,sizeof(int *));
	    if (procespera[proc-1]==0){
	       j=0;
	       for(j=0;j<N;j++){
	          printf("Instancias recurso %d? ",j+1);
		  scanf("%d",&numins);
		  if(asignados[proc-1][j]>=numins) liberacion[j]=numins;
		}
		for(j=0;j<N;j++){
		  asignados[proc-1][j]=asignados[proc-1][j]-liberacion[j];
		  disponible[j]=disponible[j]+liberacion[j];
                }
		free(liberacion);

               /*Ahora se comprueba si alguno varios procesos
		 salen de la espera y se les asigna lo que
		tienen solicitado*/

		for(i=0;i<M;i++){
		   val=1;
		   if(procespera[i]){
		     for(j=0;j<N;j++){
			if(solicitud[i][j]>disponible[j])
						{val=0;break;}
		     }
		     if(val){
		       for(j=0;j<N;j++){
			 asignados[i][j]=asignados[i][j]+solicitud[i][j];
			 disponible[j]=(disponible[j])-(solicitud[i][j]);
			 solicitud[i][j]=0;
		       }
		       procespera[i]=0;
		       printf("Proceso %d ya no se encuentra en  espera\n",i+1);
		     }/*if(val)*/
		   }/*if(procesper..)*/
		}/*for*/
	    }/*if1*/
	    else printf("Proceso en espera no puede liberar\n");
	    continue;
	}/*case2*/

	case 3:{/*abortar*/
	    for(j=0;j<N;j++){
	       disponible[j]=disponible[j]+asignados[proc-1][j];
	       asignados[proc-1][j]=0;
	       solicitud[proc-1][j]=0;
	       procespera[proc-1]=0;
	    }			

           /*Ahora se comprueba si alguno varios procesos
             salen de la espera y se les asigna lo que
	     tienen solicitado*/

	    for(i=0;i<M;i++){
	       val=1;
	       if(procespera[i]){
		 for(j=0;j<N;j++){
		     if(solicitud[i][j]>disponible[j])
						{val=0;break;}
		 }
		 if(val){
	           for(j=0;j<N;j++){
		      asignados[i][j]=asignados[i][j]+solicitud[i][j];
		      disponible[j]=disponible[j]-solicitud[i][j];
		      solicitud[i][j]=0;
		   }
	           procespera[i]=0;
		   printf("Proceso %d ya no se encuentra en espera\n",i+1);
		 }/*if(val)*/
	       }/*if1*/
	    }/*for*/
	    continue;
	}/*case 3*/
		
	case 4:{/*mostrar estado*/
	    printf("PROC\tESP\tREC\tSOL\tASIG\tDISP\n");
	    for(i=0;i<M;i++){
	        printf("%d\t%d",i+1,procespera[i]);
	        for(j=0;j<N;j++){
	           printf("\t%d\t",j+1);
	           printf("%d\t%d\t",solicitud[i][j],asignados[i][j]);
	           printf("%d\n\t",disponible[j]);
	        }
		printf("\r");
	    }
	    printf("\n");
	    continue;
	}/*case 4*/
     }/*switch*/
 }/*do*/

while(opc!=5);

free(asignados);
free(disponible);
free(solicitud);
free(procespera);
}

/* Algoritmo de deteccion        */
/* Entradas:numero de recursos,numero de procesos
	    matriz de solicitud,de asignacion y vector
	    de disponibles
   Indica que procesos se encuentran en interbloqueo */

void deteccion(int numrec,int numproc,int **sol,int **asig,int *disp)
{
int *AUX;
char *YAMIRADOS;
char b;
int i;
int j;

AUX=(int *)calloc(numrec,sizeof(int));
YAMIRADOS=(char *)calloc(numproc,sizeof(int));


for(i=0;i<numrec;i++)
	{
	 AUX[i]=disp[i];
	}

for(i=0;i<numproc;i++)
	{YAMIRADOS[i]=1;
	 for(j=0;j<numrec;j++)
		{
		 if(asig[i][j]!=0) {YAMIRADOS[i]=0;break;}
		}	
	}
	
	i=0;
	while(i<numproc){
		if(YAMIRADOS[i]) {i++;continue;}
		b=1;
		j=0;
		while(j<numrec)
			{
			 if((sol[i][j])>(AUX[j])) {b=0;break;}
			 j++;
			}

		 if(b==0) {i++;continue;}
		 for(j=0;j<numrec;j++)
			{
			 AUX[j]=AUX[j]+asig[i][j];
			}
		 YAMIRADOS[i]=1;
		 i=0;
	 }
	
	 for(i=0;i<numproc;i++)
		{
		 if (YAMIRADOS[i]==0)
			{
			 printf("El proceso %d esta en INTERBLOQUEO\n\n",i+1);
			}
		}
free(AUX);
free(YAMIRADOS);
}


/*Recibe un string y devuelve el int correspondiente*/

int entero(char *cadena)
{
 int numdigitos;
 int i,c,n=0,aux=0;

 numdigitos=strlen(cadena);
 for(i=0;i<numdigitos;i++)
  {
	aux=cadena[numdigitos-i-1]-48;
	for (c=0;c<i;c++) aux=aux*10;
	n+=aux;
  }
 return n;
}
