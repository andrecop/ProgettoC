#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.141592654


typedef struct{
    float      min;
    float      max;
    float      mean;                
}stats;


typedef struct{
    unsigned int    w;
    unsigned int    h;
    unsigned int    k;
    stats           *stat;
    float           ***data;                
}ip_mat;


ip_mat  *ip_mat_create(unsigned int h,unsigned int w,unsigned int k,float v)
{
    int        righe, colonne, terzaDimensione;
    ip_mat     *ipStructPointer;
    stats      *vettore;
    float     ***matrice;

    /*puntatore alla struttura*/
    ipStructPointer = (ip_mat *)malloc(sizeof(ip_mat));    
    /*creo il vettore a stats (va inizializzato? intanto lo lascio così)*/
    vettore = (stats *)malloc(sizeof(stats) * k);
    /*creo la matrice 3D in memoria dinamica*/
    matrice = (float ***)malloc(sizeof(float **) * h);
    /*creo la matrice 3D*/
    for(righe = 0;righe < h;righe++)
    {
        matrice[righe] = (float **)malloc(sizeof(float *) * w);
        for(colonne = 0;colonne < w;colonne++)
        {
            matrice[righe][colonne] = (float *)malloc(sizeof(float) * k);
        }
    }    
    /*inizializzo le celle al valore di v*/
    for(righe = 0;righe < h;righe++)
    {
        for(colonne = 0;colonne < w;colonne++)
        {
            for(terzaDimensione = 0;terzaDimensione < k;terzaDimensione++)
            {
                matrice[righe][colonne][terzaDimensione] = v;
            }
        }

    }
    /*assegno la locazione a data della struttura*/
    ipStructPointer->data = matrice;
    ipStructPointer->h = h;
    ipStructPointer->w = w;
    ipStructPointer->k = k;
    ipStructPointer->stat = vettore;
    /*ritorno il puntatore al chiamante*/
    return ipStructPointer;
}

void ip_mat_free(ip_mat *a)
{
    /*Indici che mi servono per liberare la matrice dinamica*/
    int  x, y;

    /*Libero il vettore stat*/
    free(a->stat);
    /*Libero la matrice 3D*/
    for(x = 0;x < a->h;x++)
    {
        for(y = 0;y < a->w;y++)
        {
            free(a->data[x][y]);
        }
        free(a->data[x]);
    }
    free(a->data);
    /*libero la struttura*/
    free(a);        
}


float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k)
{
    return (a->data[i][j][k]);
}

void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v)
{
    a->data[i][j][k] = v;
}



/*Funzione già definita dal tutor (mi serve solo per testare la mia)*/
float get_normal_random(){
    float y1 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float y2 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    return cos(2*PI*y2)*sqrt(-2.*log(y1));

}
/*---------------------------da non implementare nel sorgente finale---------------------------*/


/*Le funzioni con suffisso "Aux", sono tutte funzioni ausiliarie per altre funzioni*/
float Aux_value_of_init(float mean,float var)
{
    float   deviazioneStandard;
    float   radiceDuePI;
    float   esponenteE;
    float   normRandVar;
    float   numeratore;
    float   denominatore;
    float   Result;

    deviazioneStandard = sqrt(var);
    radiceDuePI = sqrt(2.0 * PI);
    normRandVar = get_normal_random();
    numeratore = -(pow(normRandVar-mean,2.0));
    denominatore = 2.0 * var;
    esponenteE = numeratore/denominatore;
    Result = (1/deviazioneStandard*(radiceDuePI))*(exp(esponenteE));
    return Result;
}


/*Ogni elemento è generato da una gaussiana*/
void ip_mat_init_random(ip_mat * t, float mean, float var)
{
    int  i, j, z;

    for(i = 0;i < t->h;i++)
    {
        for(j = 0;j < t->w;j++)
        {
            for(z = 0;z < t->k;z++)
            {
                t->data[i][j][z] = Aux_value_of_init(mean,var);
            }
        }
    }

}

/*Funzione che crea una copia della struttura "ip_mat"*/
ip_mat * ip_mat_copy(ip_mat * in)
{
    ip_mat    *nuovaStruttura;
    int       righe;
    int       colonne;
    int       terzaDim;
    int       i;

    /*Utilizzo la funzione di creazione ricavandomi le dimensioni dal puntatore in input*/
    nuovaStruttura = ip_mat_create(in->h,in->w,in->k,0);
    /*riempio il "data" della nuova struttura con il "data" della vecchia*/
    for(righe = 0;righe < in->h;righe++)
    {
        for(colonne = 0;colonne < in->w;colonne++)
        {
            for(terzaDim = 0;terzaDim < in->k;terzaDim++)
            {
                nuovaStruttura->data[righe][colonne][terzaDim] = in->data[righe][colonne][terzaDim];
            }
        }
    }
    /*copio anche i valori del vettore dinamico stats*/
    for(i = 0;i < in->k;i++)
    {
        nuovaStruttura->stat[i].min = in->stat[i].min;
        nuovaStruttura->stat[i].max = in->stat[i].max;
        nuovaStruttura->stat[i].mean = in->stat[i].mean;
    }
    /*ritorno al chiamante il mio puntatore copia creato*/
    return nuovaStruttura;

}

/*Faccio tre funzioni ausiliarie che verranno chiamate dalla funzione "ip_mat_concat"
in base a valore che viene passato a quest'ultima (0,1,2)*/

ip_mat *Aux_concat0(ip_mat *a,ip_mat *b)
{
    ip_mat    *result;
    int       i;
    int       j;
    int       terzaDim;
    /*Per la copia tengo traccia delle righe della matrice risultante (in modo che per
    b so dove andare a scrivere)*/
    int       iResult = 0;

    result = ip_mat_create((a->h + b->h),a->w,a->k,0.0);
    /*Comincio con la copia della matrice "a" */
    for(i = 0;i < a->h;i++)
    {
        for(j = 0;j < a->w;j++)
        {
            for(terzaDim = 0;terzaDim < a->k;terzaDim++)
            {
                result->data[i][j][terzaDim] = a->data[i][j][terzaDim];
            }
        }
        iResult++;
    }
    /*Finito il ciclo per "a" comincio il ciclo per "b"*/
    for(i = 0;i < b->h;i++)
    {
        for(j = 0;j < b->w;j++)
        {
            for(terzaDim = 0;terzaDim < b->k;terzaDim++)
            {
                result->data[iResult][j][terzaDim] = b->data[i][j][terzaDim];
            }
        }
        iResult++;
    }
    return result;
}

/*Seconda funzione ausiliaria (quella che viene chiamata nel caso il parametro dimensione
sia = a 1)*/
ip_mat *Aux_concat1(ip_mat *a,ip_mat *b)
{
    ip_mat       *result;
    int          i;
    int          j;
    int          terzaDim;
    /*Ora tengo traccia dell'indice di colonna (quella prima era indice di riga)*/
    int          jResult = 0;

    result = ip_mat_create(a->h,(a->w + b->w),a->k,0.0);
    /*Faccio la copia della matrice puntata da "a"*/
    for(i = 0;i < a->h;i++)
    {
        for(j = 0;j < a->w;j++)
        {
            for(terzaDim = 0;terzaDim < a->k;terzaDim++)
            {
                result->data[i][j][terzaDim] = a->data[i][j][terzaDim];
            }
        }
    }
    /*WARNING: attenzione qui, quando si cambia riga devo tornare alla posizione
    della colonna che si trova immediatamente dopo la "larghezza a->w" */
    for(i = 0;i < b->h;i++)
    {
        jResult = a->w;
        for(j = 0;j < b->w;j++)
        {
            for(terzaDim = 0;terzaDim < b->k;terzaDim++)
            {
                result->data[i][jResult][terzaDim] = b->data[i][j][terzaDim];
            }
            jResult++;
        }
    }
    return result;

}

/*Terza funzione ausiliaria (quella che viene chiamata nel caso il parametro dimensione
sia = a 2)*/
ip_mat *Aux_concat2(ip_mat *a,ip_mat *b)
{
    ip_mat        *result;
    int           i;
    int           j;
    int           terzaDim;
    /*ora tengo traccia dell'indice della terza dimensione*/
    int           terzaResult = 0;

    result = ip_mat_create(a->h,a->w,(a->k + b->k),0.0);
    /*copia della matrice puntata da "a"*/
    for(i = 0;i < a->h;i++)
    {
        for(j = 0;j < a->w;j++)
        {
            for(terzaDim = 0;terzaDim < a->k;terzaDim++)
            {
                result->data[i][j][terzaDim] = a->data[i][j][terzaDim];
            }
        }
    }
    /*copiamo la matrice puntata da "b" tenendo traccia di K*/
    for(i = 0;i < b->h;i++)
    {
        for(j = 0;j < b->w;j++)
        {
            terzaResult = a->k;
            for(terzaDim = 0;terzaDim < b->k;terzaDim++)
            {
                result->data[i][j][terzaResult] = b->data[i][j][terzaDim];
                terzaResult++;
            }
        }
    }
    return result;

}



/*Funzione concat da implementare*/
ip_mat * ip_mat_concat(ip_mat * a, ip_mat * b, int dimensione)
{
    ip_mat     *result = NULL;

    /*intanto testo la chiamata con il parametro 0*/
    if(dimensione == 0)
    {
        result = Aux_concat0(a,b);
    }
    else
    {
        if(dimensione == 1)
        {
            result = Aux_concat1(a,b);
        }
        else
        {
            if(dimensione == 2)
            {
                result = Aux_concat2(a,b);
            }
        }
        
    }
    
    return result;
}



void __stampa_matrice3D(ip_mat *pointer)
{
    int  i, j ,z;

    for(i = 0;i < pointer->h;i++)
    {
        for(j = 0;j < pointer->w;j++)
        {
            for(z = 0;z < pointer->k;z++)
            {
                printf(" valore_cella[%d][%d][%d] = < %f >",i,j,z,pointer->data[i][j][z]);
                
            }
            
        }
        
    }

}




/*Scrivo un main di test per concatenare le due strutture*/
int main(void)
{
    ip_mat   *matrice1;
    ip_mat   *matrice2;
    ip_mat   *matriceResult;

    matrice1 = ip_mat_create(2,2,3,0.0);
    matrice2 = ip_mat_create(2,2,3,0.0);
    ip_mat_init_random(matrice1,1.0,2.0);
    ip_mat_init_random(matrice2,0.0,1.0);
    printf("\nMatrice 1 = \n");
    __stampa_matrice3D(matrice1);
    printf("\n\nMatrice 2 = \n");
    __stampa_matrice3D(matrice2);
    printf("\n\n\nFacciamo la concatenazione: \n");
    matriceResult = ip_mat_concat(matrice1,matrice2,2);
    printf("\n\nMatrice Risultante: \n\n");
    __stampa_matrice3D(matriceResult);
    printf("\n\n");
    ip_mat_free(matrice1);
    ip_mat_free(matrice2);
    ip_mat_free(matriceResult);
    return 0;







}



/*
int main(void)
{
    ip_mat   *p;
    ip_mat   *copiaStruttura;

    p = ip_mat_create(2,2,3,1);
    __stampa_matrice3D(p);
    printf("\n\n");
    ip_mat_init_random(p,0.0,1.0);
    __stampa_matrice3D(p);
    printf("\n\n");
    printf("\nOra creiamo una copia della struttura originale e la stampiamo: \n\n");
    copiaStruttura = ip_mat_copy(p);
    __stampa_matrice3D(copiaStruttura);
    printf("\n\n");
    ip_mat_free(p);
    ip_mat_free(copiaStruttura);
    return 0;
}
*/




