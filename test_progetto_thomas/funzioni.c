#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bmp.h"
#include "ip_lib.h"

#define PI 3.141592654

/*
typedef struct{
    float      min;
    float      max;
    float      mean;                
}stats;
*/

/*
typedef struct{
    unsigned int    w;
    unsigned int    h;
    unsigned int    k;
    stats           *stat;
    float           ***data;                
}ip_mat;
*/

ip_mat  *ip_mat_create(unsigned int h,unsigned int w,unsigned int k,float v)
{
    int        righe, colonne, terzaDimensione;
    ip_mat     *ipStructPointer;
    stats      *vettore;
    float     ***matrice;
    int        i;

    /*puntatore alla struttura*/
    ipStructPointer = (ip_mat *)malloc(sizeof(ip_mat));
    if(ipStructPointer)
    {    
        /*creo il vettore a stats (va inizializzato? intanto lo lascio così)*/
        vettore = (stats *)malloc(sizeof(stats) * k);
        if(vettore)
        {
            /*creo la matrice 3D in memoria dinamica*/
            matrice = (float ***)malloc(sizeof(float **) * h);
            if(matrice)
            {
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
                /*Inizializzo anche il vettore a "stats"*/
                for(i = 0;i < k;i++)
                {
                    vettore[i].min = v;
                    vettore[i].max = v;
                    vettore[i].mean = v;
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
            else
            {
                exit(1);
            }            
        }
        else
        {
            exit(1);
        }        
    }
    else
    {
        exit(1);
    }    
}

void ip_mat_free(ip_mat *a)
{
    /*Indici che mi servono per liberare la matrice dinamica*/
    int  x, y;

    if(a)
    {
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
}


float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k)
{
    if((i >= 0 && i < a->h) && (j >= 0 && j < a->w) && (k >= 0 && k < a->k))
    {
        return (a->data[i][j][k]);
    }
    else
    {
        exit(1);
    }    
}

void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v)
{
    if((i >= 0 && i < a->h) && (j >= 0 && j < a->w) && (k >= 0 && k < a->k))
    {
        a->data[i][j][k] = v;
    }
    else
    {
        exit(1);
    }    
}



/*Funzione già definita dal tutor (mi serve solo per testare la mia)*/
float get_normal_random(){
    float y1 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float y2 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    return cos(2*PI*y2)*sqrt(-2.*log(y1));

}
/*---------------------------da non implementare nel sorgente finale---------------------------*/


/*Funzione ausiliaria per il calcolo delle statistiche del canale*/
void Aux_calcola_canale(ip_mat *t,int canale)
{
    int      n = 0;
    int      i;
    int      j;
    int      somma = 0;
    int      media;

    for(i = 0;i < t->h;i++)
    {
        for(j = 0;j < t->w;j++)
        {
            if(t->data[i][j][canale] < t->stat[canale].min)
            {
                t->stat[canale].min = t->data[i][j][canale];
            }
            if(t->data[i][j][canale] > t->stat[canale].max)
            {
                t->stat[canale].max = t->data[i][j][canale];
            }
            n++;
            somma += t->data[i][j][canale];
        }
    }
    media = somma/n;
    t->stat[canale].mean = media;
}



/*QUI METTO LA COMPUTE STAT*/
/*N.B. -> il vettore stats è già riempito (il riempimento è avvenuto durante la ip_mat_create)*/
void compute_stats(ip_mat * t)
{
    int    i;
    

    for(i = 0;i < t->k;i++)
    {
        Aux_calcola_canale(t,i);
    }

}




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
    /*Risettiamo anche i valori aggiornati dei canali*/
    compute_stats(t);

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

    if((a->w == b->w)&&(a->k == b->k))
    {
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
    else
    {
        printf("\nDimensioni w/k differenti");
        exit(1);
    }    
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

    if((a->h == b->h)&&(a->k == b->k))
    {
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
    else
    {
        printf("\nDimensioni di h/k differenti");
        exit(1);
    }    
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

    if((a->h == b->h)&&(a->w == b->w))
    {
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
    else
    {
        printf("\nDimensioni di h/w differenti");
        exit(1);
    }    
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

/*Funzione "ip_mat_subset"*/
ip_mat * ip_mat_subset(ip_mat * t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end)
{
    ip_mat          *result;
    int             i;
    int             j;
    int             terzaDim;
    /*dimensioni della nuova matrice*/
    unsigned int    hResult;
    unsigned int    wResult;
    /*indici della matrice "result" durante la copia*/
    int             iR;
    int             jR;

    if((row_start >= 0 && row_start < t->h)&&(row_end >= row_start && row_end < t->h))
    {
        if((col_start >= 0 && col_start < t->w)&&(col_end >= col_start && col_end < t->w))
        {
            hResult = (row_end+1)-row_start;
            wResult = (col_end+1)-col_start;
            result = ip_mat_create(hResult,wResult,t->k,0.0);
            /*copiamo i valori dalla porzione della matrice "t" */
            for(i = row_start,iR = 0;i <= row_end;i++,iR++)
            {
                for(j = col_start,jR = 0;j <= col_end;j++,jR++)
                {
                    for(terzaDim = 0;terzaDim < t->k;terzaDim++)
                    {
                        result->data[iR][jR][terzaDim] = t->data[i][j][terzaDim];
                    }
                }
            }
            compute_stats(result);
            return result;         
        }
        else
        {
            printf("\nIndici sotto-matrice non validi");
            exit(1);
        }        
    }
    else
    {
        printf("\nIndici sotto-matrice non validi");
        exit(1);
    }    
}

/*Funzione che somma due "ip_mat"*/
ip_mat * ip_mat_sum(ip_mat * a, ip_mat * b)
{
    /*Facciamo un controllo che le dimensioni delle due ip_mat siano uguali*/
    if((a->h == b->h) && (a->w == b->w) && (a->k == b->k))
    {
        int      i;
        int      j;
        int      terzaDim;
        ip_mat   *result;

        result = ip_mat_create(a->h,a->w,a->k,0.0);
        /*Faccio la somma*/
        for(i = 0;i < a->h;i++)
        {
            for(j = 0;j < a->w;j++)
            {
                for(terzaDim = 0;terzaDim < a->k;terzaDim++)
                {
                    result->data[i][j][terzaDim] = (a->data[i][j][terzaDim] + b->data[i][j][terzaDim]);
                }
            }
        }
        /*QUI RICALCOLO ANCHE I VALORI PER OGNI SINGOLO CANALE DEL VETTORE STATS*/
        compute_stats(result);
        return result;
    }
    else
    {
        printf("\nDimensioni differenti delle due matrici");
        exit(1);
    }
    
}

/*Funzione che sottrae due "ip_mat"*/
ip_mat * ip_mat_sub(ip_mat * a, ip_mat * b)
{
    /*Facciamo un controllo che le dimensioni delle due ip_mat siano uguali*/
    if((a->h == b->h) && (a->w == b->w) && (a->k == b->k))
    {
        int      i;
        int      j;
        int      terzaDim;
        ip_mat   *result;

        result = ip_mat_create(a->h,a->w,a->k,0.0);
        /*Faccio la sottrazione*/
        for(i = 0;i < a->h;i++)
        {
            for(j = 0;j < a->w;j++)
            {
                for(terzaDim = 0;terzaDim < a->k;terzaDim++)
                {
                    result->data[i][j][terzaDim] = (a->data[i][j][terzaDim] - b->data[i][j][terzaDim]);
                }
            }
        }
        /*RICALCOLATO LE STATISTICHE DEI 3 CANALI*/
        compute_stats(result);
        return result;
    }
    else
    {
        printf("\nDimensioni differenti delle due matrici");
        exit(1);
    }
}

/*Funzione che moltiplica una "ip_mat" per uno scalare*/
ip_mat * ip_mat_mul_scalar(ip_mat *a, float c)
{
    int      i;
    int      j;
    int      terzaDim;
    ip_mat   *result;

    result = ip_mat_create(a->h,a->w,a->k,0.0);
    if(result)
    {
        /*Faccio la moltiplicazione per lo scalare*/
        for(i = 0;i < a->h;i++)
        {
            for(j = 0;j < a->w;j++)
            {
                for(terzaDim = 0;terzaDim < a->k;terzaDim++)
                {
                    result->data[i][j][terzaDim] = a->data[i][j][terzaDim] * c;
                }
            }
        }
        /*RICALCOLATO LE STATISTICHE DEI 3 CANALI*/
        compute_stats(result);
        return result;
    }
    else
    {
        exit(1);
    }
}

/*Aggiunge uno scalare c alla ip_mat*/
ip_mat *  ip_mat_add_scalar(ip_mat *a, float c)
{
    int      i;
    int      j;
    int      terzaDim;
    ip_mat   *result;

    result = ip_mat_create(a->h,a->w,a->k,0.0);
    if(result)
    {
        /*Faccio la moltiplicazione per lo scalare*/
        for(i = 0;i < a->h;i++)
        {
            for(j = 0;j < a->w;j++)
            {
                for(terzaDim = 0;terzaDim < a->k;terzaDim++)
                {
                    result->data[i][j][terzaDim] = a->data[i][j][terzaDim] + c;
                }
            }
        }
        /*RICALCOLATO LE STATISTICHE DEI 3 CANALI*/
        compute_stats(result);
        return result;
    }
    else
    {
        exit(1);
    }
}

/*Calcolo della media fra due matrici*/
ip_mat * ip_mat_mean(ip_mat * a, ip_mat * b)
{
    int      i;
    int      j;
    int      terzaDim;
    ip_mat   *result;
    /*Le due ip_mat devono avere le stesse dimensioni per fare la media dei pixel*/

    result = ip_mat_create(a->h,a->w,a->k,0.0);
    if((a->h == b->h)&&(a->w == b->w)&&(a->k == b->k)&&result)
    {
        for(i = 0;i < a->h;i++)
        {
            for(j = 0;j < a->w;j++)
            {
                for(terzaDim = 0;terzaDim < a->k;terzaDim++)
                {
                    result->data[i][j][terzaDim] = (a->data[i][j][terzaDim] + b->data[i][j][terzaDim]) / 2.0;
                }
            }
        }
        /*METTO LE STATISTICHE DEI CANALI*/
        compute_stats(result);
        return result;
    }
    else
    {
        exit(1);
    }
    
}



/*-----------------------------FUNZIONI DA NON IMPLEMENTARE-----------------------------*/
/*Funzioni già implementate per la visualizzazione delle matrici*/
void ip_mat_show(ip_mat * t){
    unsigned int i,l,j;
    printf("Matrix of size %d x %d x %d (hxwxk)\n",t->h,t->w,t->k);
    for (l = 0; l < t->k; l++) {
        printf("Slice %d\n", l);
        for(i=0;i<t->h;i++) {
            for (j = 0; j < t->w; j++) {
                printf("%f ", get_val(t,i,j,l));
            }
            printf("\n");
        }
        printf("\n");
    }
}

void ip_mat_show_stats(ip_mat * t){
    unsigned int k;

    compute_stats(t);

    for(k=0;k<t->k;k++){
        printf("Channel %d:\n", k);
        printf("\t Min: %f\n", t->stat[k].min);
        printf("\t Max: %f\n", t->stat[k].max);
        printf("\t Mean: %f\n", t->stat[k].mean);
    }
}

ip_mat * bitmap_to_ip_mat(Bitmap * img){
    unsigned int i=0,j=0;

    unsigned char R,G,B;

    unsigned int h = img->h;
    unsigned int w = img->w;

    ip_mat * out = ip_mat_create(h, w,3,0);

    for (i = 0; i < h; i++)              /* rows */
    {
        for (j = 0; j < w; j++)          /* columns */
        {
            bm_get_pixel(img, j,i,&R, &G, &B);
            set_val(out,i,j,0,(float) R);
            set_val(out,i,j,1,(float) G);
            set_val(out,i,j,2,(float) B);
        }
    }

    return out;
}


Bitmap * ip_mat_to_bitmap(ip_mat * t){

    Bitmap *b = bm_create(t->w,t->h);

    unsigned int i, j;
    for (i = 0; i < t->h; i++)              /* rows */
    {
        for (j = 0; j < t->w; j++)          /* columns */
        {
            bm_set_pixel(b, j,i, (unsigned char) get_val(t,i,j,0),
                    (unsigned char) get_val(t,i,j,1),
                    (unsigned char) get_val(t,i,j,2));
        }
    }
    return b;
}


/*-----------------------------FUNZIONI DA NON IMPLEMENTARE-----------------------------*/




/*------------------------PARTE 2------------------------*/


/*Funzione per incrementare la luminosità (controllo valori range[0 - 255])*/
ip_mat * ip_mat_brighten(ip_mat * a, float bright)
{
    int        i;
    int        j;
    int        terzaDim;
    ip_mat     *result;

    result = ip_mat_create(a->h,a->w,a->k,0.0);
    if(result)
    {
        for(i = 0;i < a->h;i++)
        {
            for(j = 0;j < a->w;j++)
            {
                for(terzaDim = 0;terzaDim < a->k;terzaDim++)
                {
                    if(a->data[i][j][terzaDim] + bright > 255)
                    {
                        result->data[i][j][terzaDim] = 255.0;
                    }
                    else
                    {
                        if(a->data[i][j][terzaDim] + bright < 0)
                        {
                            result->data[i][j][terzaDim] = 0.0;
                        }
                        else
                        {
                            result->data[i][j][terzaDim] = a->data[i][j][terzaDim] + bright;
                        }                        
                    }
                    
                }
            }
        }
        compute_stats(result);
        return result;
    }
    else
    {
        exit(1);
    }
}







