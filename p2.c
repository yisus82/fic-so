/*************************************************/ 
/*******PRÁCTICA 2 DE SISTEMAS OPERATIVOS 1*******/        
/************2º INGENIERÍA INFORMÁTICA************/
/*****************REALIZADA POR: *****************/                     
/* JOSÉ ANTONIO PEREIRA SUÁREZ (infjps00)        */                         
/*JESÚS ÁNGEL PÉREZ-ROCA FERNÁNDEZ (infjpf02)    */
/*************************************************/





#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int eleccion;
int aptdoA();
int aptdoB();
int es_seguro(int num_proc,int num_rec);
int deteccion(int num_proc,int num_rec);
int **maximos, **necesidades, **solicitados, **asignados; 
int *disponibles;                 
int *en_espera;			 			    

int main(int argc, char *argv[])
{
int i,j;
FILE *fichero;
int buffer;
int num_proc,num_rec;
int comprobar=0;
int potencia;
char enteros[100];
int *maximo;
int estado_seguro;
int interbloqueo;

if (argc>2) 	    

{
        num_proc=0;
	num_rec=0;

    for(i=0;i<strlen(argv[argc-1]);i++)
    {		
	if (((argv[argc-1][i]-'0')<=9)&&((argv[argc-1][i]-'0')>=0))
		comprobar=1;
    }
  if (comprobar!=1) 
  {
	comprobar=0;
	if ((fichero=fopen(argv[argc-1], "r"))!=NULL)
	{
		while (!feof(fichero))
		{
			fscanf(fichero,"%d",&buffer);
			num_rec++;
		}
	fclose(fichero);
	}
	else 
        {
              printf("\nNo se encontro el fichero %s \n",argv[argc-1]);
	      exit(0);
	}
	num_proc=(num_rec-1)/(argc-2);
        num_rec=(argc-2);
	
	maximos=(int **)malloc(sizeof(int *)*num_proc);
	necesidades=(int **)malloc(sizeof(int *)*num_proc);
	solicitados=(int **)malloc(sizeof(int *)*num_proc);
	asignados=(int **)malloc(sizeof(int *)*num_proc); 
	
        for (i=0;i<num_proc;i++)       
	{
		maximos[i]=(int *)malloc(sizeof(int)*num_rec);
		necesidades[i]=(int *)malloc(sizeof(int)*num_rec);
		solicitados[i]=(int *)malloc(sizeof(int)*num_rec);
		asignados[i]=(int *)malloc(sizeof(int)*num_rec);
	}

	
	fichero=fopen(argv[argc-1],"r");	
	for(i=0;i<num_proc;i++)
	{
		for(j=0;j<num_rec;j++)
		{
			fscanf(fichero,"%d",&buffer);
			maximos[i][j]=buffer;
			necesidades[i][j]=buffer;
			solicitados[i][j]=0;
			asignados[i][j]=0;
		}
		printf("\n");
	}
	fclose(fichero);
	

        disponibles=(int *)malloc(sizeof(int)*num_rec);
        en_espera=(int *)malloc(sizeof(int)*num_proc);

        for (j=0;j<num_rec;j++)
        {
	      disponibles[j]=0;
	      strcpy(enteros,argv[j+1]);
	      potencia=1;
	      for(i=0;i<strlen(enteros);i++)
	      {

                  disponibles[j]=disponibles[j]+(enteros[strlen(enteros)-(i+1)]-'0')*potencia;
		  potencia=potencia*10;
	      }
        }



/******************** APARTADO A ********************/


do
{
  comprobar=0;
  eleccion=aptdoA();

 if (eleccion==1) 
 {
	printf("\nElija el proceso que solicita recursos (eleccion del 1 al %d): ",num_proc);
	scanf("%d",&i);
	i=i-1;
	if (en_espera[i]==1) 
               printf("\nEl proceso %d no puede solicitar recursos porque esta en esperar \n",i+1); 
        else
        {
	       j=0;
	       for (j=0;j<num_rec;j++)
	       {
		     printf("\nPonga el numero de solicitudes para el recurso %d= ",j+1);
		     scanf("%d",&buffer);
		     solicitados[i][j]=buffer;
		     if (solicitados[i][j]>necesidades[i][j])
		     			comprobar=-1;
		 
	       }
	       if (comprobar==-1)
	       {
		     printf("\nNo se puede atender la solicitud porque supera las necesidades maximas del proceso\n");
		     for(j=0;j<num_rec;j++)
		   	   solicitados[i][j]=0;
	       }
	       else 
	       {
		    for(j=0;j<num_rec;j++)
		    {
                        if (solicitados[i][j]>disponibles[j])
			        comprobar=-1;
		    }
		    if (comprobar==-1) 
		    { 
				en_espera[i]=1;
				printf("\nNo se puede atender la solicitud porque supera los recursos disponibles\n");
				printf("El proceso %d pasara a estar en espera hasta que se pueda atender su solicitud\n",i+1); 
		    }
		    else
		    {   
			for (j=0;j<num_rec;j++)
			{
				asignados[i][j]=asignados[i][j]+solicitados[i][j];
				necesidades[i][j]=necesidades[i][j]-solicitados[i][j];
				disponibles[j]=disponibles[j]-solicitados[i][j];
			} 
			estado_seguro=es_seguro(num_proc,num_rec); 
			if (estado_seguro==0)
			{ 
			   en_espera[i]=1;
			   for(j=0;j<num_rec;j++)
			   {
					asignados[i][j]=asignados[i][j]-solicitados[i][j];
					necesidades[i][j]=necesidades[i][j]+solicitados[i][j];
					disponibles[j]=disponibles[j]+solicitados[i][j];
			   }
			}
			else
			{
				for (j=0;j<num_rec;j++)
					solicitados[i][j]=0;
			} 
			
		    }   	
	       }
	}
 }

 if (eleccion==2) 
 {
	printf("\nElija el proceso que libera recursos (eleccion del 1 al %d) : ",num_proc);
	scanf("%d",&i);
	i=i-1;
	if (en_espera[i]==1) 
		printf("\nEl proceso  %d al encontrarse en espera, no puede liberar recursos \n",i+1);
        else
        {
		for(j=0;j<num_rec;j++)
		{
			printf("\nIntroduzca el numero de instancias a liberar del recurso %d (como maximo %d): ",j+1,asignados[i][j]);
			scanf("%d",&buffer);
			if (buffer<=asignados[i][j]) 
	        	{
				asignados[i][j]=asignados[i][j]-buffer;
				disponibles[j]=disponibles[j]+buffer;
				necesidades[i][j]=maximos[i][j]-asignados[i][j];
				solicitados[i][j]=0;
			}
			else 
                	    printf("\nNo se pueden liberar los recursos porque el proceso %d no tiene tantos asignados\n",i+1);
		}
		for(i=0;i<num_proc;i++) 
		{
			comprobar=0;
			for (j=0;j<num_rec;j++)
			{
				if ((solicitados[i][j]>disponibles[j])&&(en_espera[i]==1))
					comprobar=-1;
			}
			if (comprobar==-1) 
			{
				en_espera[i]=1;
				printf("\nEl proceso %d no se puede sacar del estado de espera porque sus solicitudes siguen superando los recursos disponibles\n", i+1);
			}
			if ((en_espera[i]==1) && (comprobar==0))
			{
				for (j=0;j<num_rec;j++)
				{
					asignados[i][j]=asignados[i][j]+solicitados[i][j];
					necesidades[i][j]=necesidades[i][j]-solicitados[i][j];
					disponibles[j]=disponibles[j]-solicitados[i][j];
				}
				estado_seguro=es_seguro(num_proc,num_rec);
				if (estado_seguro==0)
				{
					en_espera[i]=1;
					for (j=0;j<num_rec;j++)
					{
						asignados[i][j]=asignados[i][j]-solicitados[i][j];
						necesidades[i][j]=necesidades[i][j]+solicitados[i][j];
						disponibles[j]=disponibles[j]+solicitados[i][j];
					}
				}
				else
				{
					for(j=0;j<num_rec;j++) 
						solicitados[i][j]=0;
				        en_espera[i] = 0;
				}
			}	
		}
 	}
 }

 if (eleccion==3) 
 {
	j=0;
	for (i=0;i<num_proc;i++)
	{
		if (en_espera[i]==1)
		{
			printf("\nEl proceso %d se encuentra en espera\n",i+1);
			j++;
		}
	}
	if (j==0)
             printf("\nNo hay ningun proceso en espera\n");
 }


 if (eleccion==4) 
 {
	for (j=0;j<num_rec;j++)
		printf("\nEl recurso %d tiene %d instancias disponibles\n",j+1,disponibles[j]);
 }

 if (eleccion==5) 
 {
	for(i=0;i<num_proc;i++)
	{
		printf("\nEl proceso %d tiene asignados : ",i+1);
		for (j=0;j<num_rec;j++)
			printf("%d instancias del recurso %d\t",asignados[i][j],j+1);
		printf("\n Necesita: ");
		for (j=0;j<num_rec;j++)
			printf("%d instancias del recurso %d\t",necesidades[i][j],j+1);
		printf("\nPresione Enter para seguir");
                getchar();
	}
 }


}

while(eleccion != 0);


}



else 
{
	num_rec=(argc-2);
        num_proc=0;
        disponibles=(int *)malloc(sizeof(int)*num_rec);
	maximo=(int *)malloc(sizeof(int)*num_rec);
	for(i=1;i<(argc-1);i++)
	{
		potencia=1;
		disponibles[i-1]=0;
		for(j=0;j<strlen(argv[i]);j++)
		{
			disponibles[i-1]=disponibles[i-1]+(argv[i][(strlen(argv[i])-1)-j]-'0')*potencia;
			potencia=potencia*10;
		}
		maximo[i]=disponibles[i];
	}	
        potencia=1;
	for(i=0;i<strlen(argv[argc-1]);i++)
	{
		num_proc=num_proc+(argv[argc-1][(strlen(argv[argc-1])-1)-i]-'0')*potencia;
		potencia=potencia*10;
	}

	solicitados=(int **)malloc(sizeof(int *)*num_proc);
	asignados=(int **)malloc(sizeof(int *)*num_proc);
	en_espera=(int *)malloc(sizeof(int)*num_proc);
	for (i=0;i<num_proc;i++)
	{
		solicitados[i]=(int *)malloc(sizeof(int)*num_rec);
		asignados[i]=(int *)malloc(sizeof(int)*num_rec);
		en_espera[i]=0;
		for(j=0;j<num_rec;j++)
		{
			solicitados[i][j]=0;
			asignados[i][j]=0;
		}
	}


/******************** APARTADO B ********************/  

 do
 {
   eleccion=aptdoB();
   if (eleccion==1) 
   {
	printf("\nElija el proceso que solicita recursos (eleccion del 1 al %d): ",num_proc);
	scanf("%d",&i);
	i=i-1;
	if (en_espera[i]==1) 
	  printf("\nEl proceso %d no puede solicitar recursos porque esta en espera\n",i+1); 
        else
        {
             for (j=0;j<num_rec;j++)
	     {
	             printf("\nElija el numero de solicitudes para el recurso %d: ",j+1);
	             scanf("%d",&buffer);
		     solicitados[i][j]=buffer;
	     }
	     comprobar=0;
	     for(j=0;j<num_rec;j++)
	     {
	        if (solicitados[i][j]>disponibles[j])
                                           comprobar=-1;
	     }   
	     if (comprobar==-1)
	     {
		      printf("\nEl proceso %d pasara a espera porque sus solicitudes superan lo disponible\n", i+1);
		      en_espera[i]=1;
	     }
	     else 
	     {
		      for (j=0;j<num_rec;j++)
		      {
			      asignados[i][j]=asignados[i][j]+solicitados[i][j];
			      disponibles[j]=disponibles[j]-solicitados[i][j];
			      solicitados[i][j]=0;
		      }
	     }
        }
   }

   if (eleccion==2) 
   {
	printf("\nElija el proceso que libera recursos (eleccion del 1 al %d): ",num_proc);
	scanf("%d",&i);
	i=i-1;
	if (en_espera[i]==1)
		printf("\nEl proceso %d no puede liberar recursos porque esta en espera\n",i+1);
	else
	{
		for(j=0;j<num_rec;j++)
		{
			printf("\nIntroduzca el numero de instancias a liberar del recurso %d (como maximo %d): ",j+1,asignados[i][j]); 
			scanf("%d",&buffer);
			if (buffer<=asignados[i][j])
			{
				asignados[i][j]=asignados[i][j]-buffer;
				disponibles[j]=disponibles[j]+buffer;
				solicitados[i][j]=0;
			}
			else 
                           printf("\nNo se pueden liberar los recursos porque el proceso %d no tiene tantos asignados\n",i+1);
		}
	 	for(i=0;i<num_proc;i++) 
		{
			comprobar=0;
			for (j=0;j<num_rec;j++)
			{
				if (solicitados[i][j]>disponibles[j])
					comprobar=-1;
			}
                        if (comprobar==-1) 
			{
				en_espera[i]=1;
				printf("\nEl proceso %d no se puede sacar del estado de espera porque sus solicitudes siguen superando los recursos disponibles\n",i+1);
			}
			if ((en_espera[i]==1) && (comprobar==0))
			{
                            for(j=0;j<num_rec;j++)
                            {
				asignados[i][j]=asignados[i][j]+solicitados[i][j];
				disponibles[j]=disponibles[j]-solicitados[i][j];
				solicitados[i][j]=0;
				en_espera[i]=0; 
			    }		   
			}	
	 	}
	}
   }

   if (eleccion==3) 
   {
	j=0;
	for(i=0;i<num_proc;i++)
	{
		if (en_espera[i]==1)
		{
			printf("\nEl proceso %d se encuentra en espera \n",i+1);
			j++;
		}
	}
	if (j==0) 
           printf("\nNo hay ningun proceso en espera\n");
   }

   if (eleccion==4) 
   {
	for (j=0;j<num_rec;j++)
		printf("\nEl recurso %d tiene %d instancias disponibles\n",j+1,disponibles[j]);
   }

   if (eleccion==5) 
   {
	for(i=0;i<num_proc;i++)
	{
		printf("\nEl proceso %d tiene asignadas : ",i+1);
		for (j=0;j<num_rec;j++)
			printf("%d instancias del recurso %d\t",asignados[i][j],j+1);
		if (en_espera[i]==1)
                         printf("\nEl proceso %d esta en espera\n", i+1);
		printf("Presione Enter para seguir");
                getchar();
	}
   }

   if (eleccion==6) 
   {
	interbloqueo=deteccion(num_proc,num_rec);
	if (interbloqueo==0) 
                  printf("\nNo existe interbloqueo\n");
   }

 }
 while(eleccion!=0);

}

}

else /* Si argc<=2 */
{
        printf("\nSi quiere simular evitacion de interbloqueo escriba  a.out <numero de instancias de cada recurso separadas por espacios> <archivo donde estan las necidades maximas de los procesos para cada recurso (un proceso en cada linea)>\n");
	printf("\nSi quiere simular deteccion de interbloqueo escriba  a.out <numero de instacias de cada recurso se paradas por espacios> <numero de procesos>\n");
}
}


/******************** COMPROBACION DE ESTADO SEGURO ********************/

int es_seguro(int num_proc, int num_rec)
{
int estado_seguro;
int i,j,k;
int secuencia=1;
int *aux;
int *comprobados;
int *secuencia_segura;

aux=(int *)malloc(sizeof(int)*num_rec);
for(j=0;j<num_rec;j++)
        aux[j]=disponibles[j];
secuencia_segura=(int *)malloc(sizeof(int)*num_proc);
comprobados=(int *)malloc(sizeof(int)*num_proc);
for (j=0;j<num_proc;j++)
	comprobados[j]=0;		
do
 {
	estado_seguro=0;	
	for (i=0;i<num_proc;i++)
	{
		if(comprobados[i]==0)
                {
                	k=1;
			for(j=0;j<num_rec;j++)
                		if(necesidades[i][j]>aux[j])
                                                  k=0;
                        if (k==1)
                        {
				for(j=0;j<num_rec;j++)
					aux[j]=aux[j]+asignados[i][j];
				comprobados[i]=1;
				estado_seguro=1;
				secuencia_segura[secuencia]=i+1;
				secuencia++;
				i=1;
				break;
			}
		}
	}
 }
while (estado_seguro==1);
estado_seguro=1; 
for (i=0;i<num_proc;i++)
	if(comprobados[i]==0)
		estado_seguro=0;
if (estado_seguro==0)
	printf("\nEl estado resultante no es seguro\n");
else
{
	printf("\nUna posible secuencia segura es: [ ");
	for(j=1;j<num_proc;j++)
			printf("%d - ",secuencia_segura[j]);	
	printf("%d]\n",secuencia_segura[num_proc]);
}
return(estado_seguro);
}



/******************** DETECCION DE INTERBLOQUEO ********************/
int deteccion(int num_proc, int num_rec)
{
int *auxiliar;
int i,j;
int *comprobado;
int interbloqueo;
int comprobar;
int elecciones;

comprobado=(int *)malloc(sizeof(int)*num_proc);
auxiliar=(int *)malloc(sizeof(int)*num_rec);
for (i=0;i<num_rec;i++)
		auxiliar[i]=disponibles[i];
for(i=0;i<num_proc;i++)
{
	comprobar=0;
	for(j=0;j<num_rec;j++)
	{
		if (asignados[i][j]!=0)
				comprobar=1;
	}
if (comprobar==0)
		comprobado[i]=1;
else
                comprobado[i]=0;
}
i=0;
while (i<num_proc) 
{
	comprobar=0;
	for(j=0;j<num_rec;j++)
	{
		if(solicitados[i][j]>auxiliar[j])
				comprobar=1;
	}
        if ((comprobado[i]==0)&&(comprobar==0))
	{
		for (j=0;j<num_rec;j++)
			auxiliar[j]=auxiliar[j]+asignados[i][j];
			comprobado[i]=1;
			i=0;
	}
	else
            i++; 
}
interbloqueo=0;	
for (i=0;i<num_proc;i++)
{
	if (comprobado[i]==0)
	{
		printf("\nEl proceso %d esta en interbloqueo\n",i+1);
		interbloqueo=1;
	}
}
if (interbloqueo==1)
{
   do
   {
	printf("\nElija una de las siguientes opciones:\n");
        printf("1. Abortar todos los procesos\n");
	printf("2. Abortar solo un proceso \n");
	printf("0. No abortar ningun proceso\n");
        scanf("%d",&elecciones);
	if (elecciones==1)
	{
		for (i=0;i<num_proc;i++)
		{
			if (comprobado[i]==0)
			{
				for(j=0;j<num_rec;j++)
				{
					disponibles[j]=disponibles[j]+asignados[i][j];
					asignados[i][j]=0;
					solicitados[i][j]=0;
				}
				en_espera[i]=0;
			}
		}
	}
	if (elecciones==2)
	{
		printf("\nElija que proceso quiere abortar de entre los siguientes: ");
		for(i=0;i<num_proc;i++)
		{
			if (comprobado[i]==0)
				printf("%d\t",i+1);
		}
		scanf("%d",&comprobar);
		if (comprobado[comprobar-1]==0)
		{
			for(j=0;j<num_rec;j++)
			{
				disponibles[j]=disponibles[j]+asignados[comprobar-1][j];
				asignados[comprobar-1][j]=0;
				solicitados[comprobar-1][j]=0;
			}
			comprobado[comprobar-1]=1;
			en_espera[comprobar-1]=0;
			for (i=0;i<num_proc;i++)
			{
				if (en_espera[i]==1)
				{				
					comprobar=0;
					for (j=0;j<num_rec;j++)
					{	
						if (solicitados[i][j]>disponibles[j])
								comprobar=1;
					}
					if (comprobar==0)
					{
						for (j=0;j<num_rec;j++)
						{
							asignados[i][j]=asignados[i][j]+solicitados[i][j];
							disponibles[j]=disponibles[j]-solicitados[i][j];
							solicitados[i][j]=0;
						}
						en_espera[i]=0;
						comprobado[i]=1;
					}
				}
			}
		}
	        else
	           printf("\nEl proceso %d no esta en interbloqueo\n",comprobar);
        }      		
   }
   while(elecciones!=0);
}

return(interbloqueo);

}	


/******************** MENU APARTADO A ********************/
	
int aptdoA()
{
printf("\t\t\n Ha elegido simular un sistema con evitacion de interbloqueo\n");
printf("\t\t Elija una de las siguientes opciones:\n");
printf("\t\t\t 1.Solicitud de recursos por parte de un proceso\n");
printf("\t\t\t 2.Liberación de recursos por parte de un proceso\n");
printf("\t\t\t 3.Ver los procesos que estan en espera\n");
printf("\t\t\t 4.Ver las instancias disponibles de cada recurso\n");
printf("\t\t\t 5.Ver el estado de las asignaciones\n");
printf("\t\t\t 0.Salir\n");
scanf("%d",&eleccion);
return(eleccion);
}


/******************** MENU APARTADO B ********************/

int aptdoB()
{
printf("\t\t\n Ha elegido simular un sistema con deteccion de interbloqueo\n");
printf("\t\t Elija una de las siguientes opciones:\n");
printf("\t\t\t 1.Solicitud de recursos por parte de un proceso\n");
printf("\t\t\t 2.Liberación de recursos por parte de un proceso\n");
printf("\t\t\t 3.Ver los procesos que estan en espera\n");
printf("\t\t\t 4.Ver las instancias disponibles de cada recurso\n");
printf("\t\t\t 5.Ver el estado de las asignaciones\n");
printf("\t\t\t 6.Comprobar si hay interbloqueo\n");
printf("\t\t\t 0.Salir\n");
scanf("%d",&eleccion);
return(eleccion);
}
