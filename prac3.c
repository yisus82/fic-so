/***********************************************************\
 *                  PRACTICAS REALIZADAS POR               *
 ***********************************************************        
 *   Hugo Pachon Moreno        *  Jorge Lopez Castro       *
 *    Login: infhpm00          *    Login: infjlc00        *
\***********************************************************/



#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

#define max 150


/*PRIORIDAD DE LOS LECTORES*/
struct SEMAFORO{
        char mutex[max];
        char delay[max];
        char valor[max];
};
typedef struct SEMAFORO sem_t;


void Pb(char *nombre);
void Vb(char *nombre);
void P(sem_t *s);
void V(sem_t *s);
void InicializarSemaforo(sem_t *s, int valorinicial, char *nombre);
void ayuda();
void creaBuffer();

int conversion(char *num);

char buffer[max];


void lectura();
void escribir();
void creaBuffer();


void Pb(char *nombre)
{
	int df;

	while ((df=open(nombre,O_CREAT|O_EXCL, 0777))==-1);
	close (df);
}

void Vb(char *nombre)
{
	unlink(nombre);
}

void P(sem_t *s)
{
        int df, valor, despertar;

        Pb(s->mutex);
        df=open (s->valor, O_RDWR);
        read(df,&valor,sizeof(int));
        valor--;
        lseek(df,0,SEEK_SET);
        write(df,&valor,sizeof(int));
        if (valor<0)
        {
                Vb(s->mutex);
                Pb(s->delay);
                Pb(s->mutex);
                lseek(df,sizeof(int),SEEK_SET);
                read(df,&despertar,sizeof(int));
                despertar--;
                lseek(df,sizeof(int),SEEK_SET);
                write(df,&despertar,sizeof(int));
		if (despertar>0)
			Vb(s->delay);
        }
        close(df);
        Vb(s->mutex);
}

void V (sem_t *s)
{
        int df, valor, despertar;

        Pb(s->mutex);
        df=open(s->valor, O_RDWR);
        read(df,&valor,sizeof(int));
        valor++;
        lseek(df,0,SEEK_SET);
        write(df,&valor,sizeof(int));
        if (valor<=0){
                lseek(df,sizeof(int),SEEK_SET);
                read(df,&despertar,sizeof(int));
                despertar++;
                lseek(df,sizeof(int),SEEK_SET);
                write(df,&despertar,sizeof(int));
                close (df);
		Vb(s->delay);
        }
        close(df);
        Vb(s->mutex);
}

void InicializarSemaforo(sem_t *s, int valorinicial,char *nombre)
{
        int df;
	int cero;

	cero=0;
	strcpy(s->valor,nombre);
        strcat(s->valor,".val");
        strcpy(s->mutex,nombre);
        strcat(s->mutex,".mut");
        strcpy(s->delay,nombre);
        strcat(s->delay,".del");

        df=open(s->valor,O_WRONLY|O_CREAT, 0777);
        write(df,&valorinicial,sizeof(int));
        write(df,&cero,sizeof(int));
        close(df);
        unlink (s->mutex);
        df=open(s->delay,O_CREAT,0777);
        close(df);
}

void ayuda()
{
        printf("\tSIMULACION DEL PROBLEMA DE LOS LECTORES ESCRITORES\n");
        printf("Realizado por:\n Hugo Pachon Moreno \t Jorge Lopez Castro\n");
        printf("Para simular:\n a.out <n.lectores> <n.escritores> <opcion>\n");
        printf("<opcion> = s entonces simula el problema con semaforos\n");
        printf("           n entonces 	no utiliza semaforos\n");
}

int conversion(char *num)
{
	int numero=0;
	int i=0;
	int dieces=1;

	for (i=0;i<strlen(num);i++)
	{
		numero+=(num[(strlen(num)-1)-i]-'0')*dieces;
		dieces*=10;
	}
	return numero;
}

void creaBuffer()
{
        int df;
	char a[80];
	int i;

        df=open("Buffer.buf",O_WRONLY|O_CREAT,0777);
	for (i=0;i<80;i++)
		a[i]='a';
	write(df,&a,80*sizeof(char));
        close(df);
}

void lectura()
{
	char leer[81];
        int t;
        int i;
        int df;
        struct timeval tv;
        t=gettimeofday(&tv,NULL);
        df=open("Buffer.buf",O_RDONLY);
        for (i=0;i<80;i++){
                read(df,&leer[i],sizeof(char));
        }
	leer[80]='\0';
        close(df);
	sprintf(buffer,"Lector: %d Tiempo: s=%ld ms=%ld Lectura: %80s\n",getpid(),tv.tv_sec%1000,tv.tv_usec,leer);

}


void escribir()
{
        int t;
        int df;
        int i;
        char letra;
        struct timeval tv;

	
        letra=(getpid()%10)+'0';
        df=open("Buffer.buf",O_WRONLY);
        for (i=0;i<80;i++){
                write(df,&letra,sizeof(char));
	}
        close(df);
        t=gettimeofday(&tv,NULL);
sprintf(buffer,"Escritor: %d Tiempo: s=%ld ms=%ld \n",getpid(),tv.tv_sec%1000,tv.tv_usec);
}


main (int argc, char *argv[])
{
    
 

      int df;
    int numlect=0, numescrit=0;
	int recuento_lectura=0;
    int i;
    int j;

   sem_t escr, mutex;

   if (argc<4)
       ayuda();
   else {
	df=open("Recuento.lec",O_WRONLY|O_CREAT,0777);
	write(df,&recuento_lectura,sizeof(int));
	close(df);

	InicializarSemaforo(&escr,1,"escritores");
   InicializarSemaforo(&mutex,1,"mutex");  


   creaBuffer();
	numlect=conversion(argv[1]);
	numescrit=conversion(argv[2]);

	if (argv[3][0]=='s'){
        for (i=0; i<numescrit;i++)
	{
		if (fork()==0)
		{
			P(&escr);
			escribir();
			V(&escr);
                        write(1,buffer,strlen(buffer)*sizeof(char));
			exit(0);
		}
	}

        for (i=0;i<numlect;i++)
	{
		if (fork()==0)
		{
			P(&mutex);   
			df=open("Recuento.lec",O_RDWR);
			read(df,&recuento_lectura,sizeof(int));
			recuento_lectura++;
			lseek(df,0,SEEK_SET);
			write(df,&recuento_lectura,sizeof(int));
			close(df);
			if (recuento_lectura==1)
				P(&escr);
			V(&mutex);
			lectura();
			P(&mutex);
			df=open("Recuento.lec",O_RDWR);
			read(df,&recuento_lectura,sizeof(int));
			recuento_lectura--;
			if (recuento_lectura==0) 
				V(&escr);
			lseek(df,0,SEEK_SET);
			write(df,&recuento_lectura,sizeof(int));
			close(df);
			V(&mutex);
                        write(1,buffer,strlen(buffer)*sizeof(char));
			exit(0);
		}
	}
	}
	
	else{
        for (i=0; i<numescrit;i++)
	{
		if (fork()==0)
		{
			escribir();
                        write(1,buffer,strlen(buffer)*sizeof(char));
			exit(0);
		}
	}

        for (i=0;i<numlect;i++)
	{
		if (fork()==0)
		{
			lectura();
                        write(1,buffer,strlen(buffer)*sizeof(char));
			exit(0);
		}
           
	}
	}	
   }
}
