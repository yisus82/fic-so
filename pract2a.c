/*        PRACTICA 2 A        */
/*Soimulacion de un sistema con evitacion*/
/*I.Carala Suarez Casal*/
/*Ivan Gomez Rodriguez*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINEA 50

int entero(char *cadena);
int estseg(int r,int p);

int **sol,**nec,*aux,**asig,*yamirados,*estadoseguro;

main(int argc, char *argv[])
{
FILE *f;
int c,b=0,r,opc,opc1,num1,p=0,n=1,j=1;
int *disponible,*espera,**max;
char linea[50];

if (argc<3)
 {printf ("No hay suficientes argumentos"); return(0);}
r=argc-2;
if((f=fopen(argv[r+1],"r"))==NULL)
 {printf("Error al abrir el archivo");return 0;}
while ((fgets(linea,MAXLINEA,f))!=NULL) p++;
fseek (f,0,SEEK_SET);
max=(int **)malloc(sizeof(int *)*(p+1));
for (j=1;j<=p;j++)
 max[j]=(int *)malloc(sizeof(int)*(r+1));
nec=(int **)malloc(sizeof(int *)*(p+1));
for (j=1;j<=p;j++)
 nec[j]=(int *)malloc(sizeof(int)*(r+1));
disponible=(int *)malloc(sizeof(int)*(r+1));
for (j=1;j<=r;j++) disponible[j]=entero(argv[j]);
n=1;j=1;
while ((fscanf(f,"%d",&c))!=EOF)
 {
 max[n][j]=c;
 if (max[n][j]>disponible[j])
  {
  printf("Error, la necesidad maxima del proceso %d con respecto\n",n);
  printf("al recurso %d supera a las instancias existentes",j);
  return 0;
  }
 nec[n][j]=c;
 if (j==r)
  {n++;j=1;}
 else j++;
 }
fclose(f);
sol=(int **)malloc(sizeof(int *)*(p+1));
for (j=0;j<=p;j++)
 sol[j]=(int *)malloc(sizeof(int)*(r+1));
asig=(int **)malloc(sizeof(int *)*(p+1));
for (j=0;j<=p;j++)
 asig[j]=(int *)malloc(sizeof(int)*(r+1));
for (j=1;j<=p;j++)
 for (n=1;n<=r;n++)
  {asig[j][n]=0;sol[j][n]=0;}
espera=(int *)malloc(sizeof(int)*(p+1));
for (j=1;j<=p;j++) espera[j]=0;
opc1=1;
do
{
 printf("1. Solicitud\n2. Liberacion\n3. Estado procesos\n4. Salir\n");
 scanf("%d",&opc);
 if ((opc==1)||(opc==2))
  {
  printf("Indique el numero de proceso\n");
  scanf("%d",&num1);
  if ((num1>p)||(num1<1))
	{
	printf("Proceso inexistente\n");
	continue;
	}
  if (espera[num1])
	{
	printf
	("Este proceso esta en espera, no pude liberar ni solicitar\n");
	continue;
	}
  printf("Introduzca las instancias de cada recurso :\n");
  for (j=1;j<=r;j++)
	scanf("%d",&sol[num1][j]);
  }

 switch (opc)
 {
 case 1:
  {
  b=0;c=0;
  for (j=1;j<=r;j++)
	{
	if (sol[num1][j]>nec[num1][j]) b=1;
	if (sol[num1][j]>disponible[j]) c=1;
	}
  if (b)
	{
	printf("Se ignora esta solicitud pues supera la necesidad\n\n");
	continue;
	}
  if (c)
	{
	printf("Solicitud en espera, no hay recursos disponibles\n\n");
	espera[num1]=1;
	}
  else
	{
	for (j=1;j<=r;j++)
	 {
	 asig[num1][j]=asig[num1][j]+sol[num1][j];
	 nec[num1][j]=nec[num1][j]-sol[num1][j];
	 disponible[j]=disponible[j]-sol[num1][j];
	 }
	aux=(int *)malloc(sizeof(int)*(r+1));
	for(j=1;j<=r;j++) aux[j]=disponible[j];
	if(estseg(r,p)==0)
	 {
	 for (j=1;j<=r;j++)
	  {
	  asig[num1][j]=asig[num1][j]-sol[num1][j];
	  nec[num1][j]=nec[num1][j]+sol[num1][j];
	  disponible[j]=disponible[j]+sol[num1][j];
	  }
	 printf("Solicitud en espera, atender la solicitud dejaria\n");
	 printf("al sistema en estado inseguro\n\n");
	 espera[num1]=1;
	 }
	else
	 {
	 printf("Asignadas. Una posible secuencia segura es:\n");
	 for (j=1;j<=(p-1);j++) printf("%d-",estadoseguro[j]);
	 printf("%d\n\n",estadoseguro[p]);
	 }
	free(aux);free(estadoseguro);
	}
  continue;
  }

 case (2):
  {
  b=0;
  for (j=1;j<=r;j++)
  if(sol[num1][j]>asig[num1][j]) b=1;
  if (b)
	{
	printf("El numero de instancias asignadas es menor que \n");
	printf("las que se desean liberar\n\n");
	continue;
	}
  for (j=1;j<=r;j++)
	{
	asig[num1][j]=asig[num1][j]-sol[num1][j];
	disponible[j]=disponible[j]+sol[num1][j];
	nec[num1][j]=nec[num1][j]+sol[num1][j];
	}
  printf("Se han liberado\n\n");
  for (j=1;j<=p;j++)
	{
	if (espera[j])
	 {
	 c=0;
	 for (n=1;n<=r;n++)
	  if (sol[j][n]>disponible[n]) c=1;
	 if (c) continue;
	 for (n=1;n<=r;n++)
	  {
	  asig[j][n]=asig[j][n]+sol[j][n];
	  nec[j][n]=nec[j][n]-sol[j][n];
	  disponible[n]=disponible[n]-sol[j][n];
	  }
	 aux=(int *)malloc(sizeof(int)*(r+1));
	 for(n=1;n<=r;n++) aux[n]=disponible[n];
	 if (estseg(r,p)==0)
	  {
	  asig[j][n]=asig[j][n]-sol[j][n];
	  nec[j][n]=nec[j][n]+sol[j][n];
	  disponible[n]=disponible[n]+sol[j][n];
	  }
	 else {espera[j]=0;j=1;}
	 }
	}
  continue;
  }
 case (3):
  {
  printf("PROC\tASIG\tMAX\tNEC\tDISP\n");
  printf("1\t");
  for (n=1;n<=r;n++) printf("%d ",asig[1][n]);printf("\t");
  for (n=1;n<=r;n++) printf("%d ",max[1][n]);printf("\t");
  for (n=1;n<=r;n++) printf("%d ",nec[1][n]);printf("\t");
  for (n=1;n<=r;n++) printf("%d ",disponible[1]);printf("\n");
  for (j=2;j<=p;j++)
	{
	printf("%d\t",j);
	for (n=1;n<=r;n++) printf("%d ",asig[j][n]);printf("\t");
	for (n=1;n<=r;n++) printf("%d ",max[j][n]);printf("\t");
	for (n=1;n<=r;n++) printf("%d ",nec[j][n]);printf("\n");
	}
  printf("\n");
  continue;
  }
 case (4): {opc1=0;break;}
 default:{printf("Opcion no valida\n\n");continue;}
 }
}
while (opc1==1);
free(max);free(nec);free(disponible);
free(asig);free(espera);free(sol);
return 0;
}//main

//La funcion estseg comprueba si existe alguna secuencia segura
//en la situacion actual, si existe devuelve un 1 y ademas
//almacena una posible en el array estadoseguro .En caso
//de que no exista ninguna secuencia segura devuelve un 0.

int estseg(int r,int p)
{
int s,b,j,n,m;

yamirados=(int *)malloc(sizeof(int)*(p+1));
for (j=1;j<=p;j++) yamirados[j]=0;
estadoseguro=(int *)malloc(sizeof(int)*(p+1));
s=1;
do
 {
 b=0;
 for (n=1;n<=p;n++)
  {
  if (yamirados[n]==0)
	{
	m=1;
	for (j=1;j<=r;j++) if (nec[n][j]>aux[j]) m=0;
	if (m)
	 {
	 for (j=1;j<=r;j++) aux[j]=aux[j]+asig[n][j];
	 yamirados[n]=1;b=1;estadoseguro[s]=n;s++;n=1;break;
	 }
	}
  }
 }
while(b);
b=1;
for (n=1;n<=p;n++) if (yamirados[n]==0) b=0;
return(b);
}//estseg

//La funcion entero convierte un string en un entero

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
}//entero



