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
    unsigned int        righe, colonne, terzaDimensione;
    ip_mat              *ipStructPointer;
    stats               *vettore;
    float               ***matrice;
    unsigned int        i;

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
    unsigned int  x, y;

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
    unsigned int      n = 0;
    unsigned int      i;
    unsigned int      j;
    float             somma = 0;
    float             media;

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
    unsigned int    i;
    

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
    unsigned int  i, j, z;

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
    ip_mat             *nuovaStruttura;
    unsigned int       righe;
    unsigned int       colonne;
    unsigned int       terzaDim;
    unsigned int       i;

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
    ip_mat             *result;
    unsigned int       i;
    unsigned int       j;
    unsigned int       terzaDim;
    /*Per la copia tengo traccia delle righe della matrice risultante (in modo che per
    b so dove andare a scrivere)*/
    unsigned int       iResult = 0;

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
    ip_mat                *result;
    unsigned int          i;
    unsigned int          j;
    unsigned int          terzaDim;
    /*Ora tengo traccia dell'indice di colonna (quella prima era indice di riga)*/
    unsigned int          jResult = 0;

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
    ip_mat                 *result;
    unsigned int           i;
    unsigned int           j;
    unsigned int           terzaDim;
    /*ora tengo traccia dell'indice della terza dimensione*/
    unsigned int           terzaResult = 0;

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
    ip_mat                   *result;
    unsigned int             i;
    unsigned int             j;
    unsigned int             terzaDim;
    /*dimensioni della nuova matrice*/
    unsigned int             hResult;
    unsigned int             wResult;
    /*indici della matrice "result" durante la copia*/
    unsigned int             iR;
    unsigned int             jR;

    if((row_start < t->h)&&(row_end >= row_start && row_end < t->h))
    {
        if((col_start < t->w)&&(col_end >= col_start && col_end < t->w))
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
        unsigned int      i;
        unsigned int      j;
        unsigned int      terzaDim;
        ip_mat            *result;

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
        unsigned int      i;
        unsigned int      j;
        unsigned int      terzaDim;
        ip_mat            *result;

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
    unsigned int      i;
    unsigned int      j;
    unsigned int      terzaDim;
    ip_mat            *result;

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
    unsigned int      i;
    unsigned int      j;
    unsigned int      terzaDim;
    ip_mat            *result;

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
    unsigned int      i;
    unsigned int      j;
    unsigned int      terzaDim;
    ip_mat            *result;
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
    unsigned int        i;
    unsigned int        j;
    unsigned int        terzaDim;
    ip_mat              *result;

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


/*Funzione per convertire l'immagine in una scala di grigi*/
ip_mat * ip_mat_to_gray_scale(ip_mat * in)
{
    unsigned int       i;
    unsigned int       j;
    unsigned int       terzaDim;
    unsigned int       t;
    ip_mat             *result;
    float              sommaParziale = 0.0;
    float              media = 0.0;

    result = ip_mat_create(in->h,in->w,in->k,0.0);
    if(result)
    {
        for(i = 0;i < in->h;i++)
        {
            for(j = 0;j < in->w;j++)
            {
                sommaParziale = 0.0;
                media = 0.0;
                for(terzaDim = 0;terzaDim < in->k;terzaDim++)
                {
                    sommaParziale += get_val(in,i,j,terzaDim);
                }
                media = sommaParziale/(in->k);
                for(t = 0;t < result->k;t++)
                {
                    set_val(result,i,j,t,media);
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

/*Funzione per l'aggiunta di rumore*/

ip_mat * ip_mat_corrupt(ip_mat * a, float amount)
{
    unsigned int        i;
    unsigned int        j;
    unsigned int        terzaDim;
    ip_mat              *result;

    result = ip_mat_create(a->h,a->w,a->k,0.0);
    if(result)
    {
        for(i = 0;i < a->h;i++)
        {
            for(j = 0;j < a->w;j++)
            {
                for(terzaDim = 0;terzaDim < a->k;terzaDim++)
                {
                    
                    result->data[i][j][terzaDim] = (get_val(a,i,j,terzaDim))+(get_normal_random()*amount);                    
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

/*Funzione "clamp" per evitare gli overflow durante i filtri*/
void clamp(ip_mat * t, float low, float high)
{
    unsigned int      i;
    unsigned int      j;
    unsigned int      terzaDim;
    /*creo un flag per valutare se rifare o meno la compute_stats(se i valori non vengono toccati
    non ha nessun senso rifarla) - (utilizzo un char perchè non ha senso usare 4Byte)*/
    char              flag = 0;

    for(i = 0;i < t->h;i++)
    {
        for(j = 0;j < t->w;j++)
        {
            for(terzaDim = 0;terzaDim < t->k;terzaDim++)
            {
                if(get_val(t,i,j,terzaDim) < low)
                {
                    set_val(t,i,j,terzaDim,low);
                    flag = 1;
                }
                if(get_val(t,i,j,terzaDim) > high)
                {
                    set_val(t,i,j,terzaDim,high);
                    flag = 1;
                }
            }
        }
    }
    if(flag)
    {
        compute_stats(t);
    }

}


/*Funzione Blending di due immagini*/
ip_mat * ip_mat_blend(ip_mat * a, ip_mat * b, float alpha)
{
    ip_mat      *resultA = NULL;
    ip_mat      *resultB = NULL;
    ip_mat      *resultEND = NULL;

    if(alpha >= 0.0 && alpha <= 1.0)
    {
        resultA = ip_mat_mul_scalar(a,alpha);
        resultB = ip_mat_mul_scalar(b,(1-alpha));
        resultEND = ip_mat_sum(resultA,resultB);
        return resultEND;
    }
    else
    {
        printf("\nAlpha non compreso fra [0-1]");
        exit(1);
    }
}


/*----------------------PARTE 3----------------------*/

/*Funzion di "padding" per i bordi*/
ip_mat * ip_mat_padding(ip_mat * a, int pad_h, int pad_w)
{
    ip_mat        *result;
    unsigned int  i;
    unsigned int  j;
    unsigned int  terzaDim;
    unsigned int  iA;
    unsigned int  jA;
    unsigned int  hResult = (a->h)+(2*pad_h);
    unsigned int  wResult = (a->w)+(2*pad_w);

    result = ip_mat_create(hResult,wResult,a->k,0.0);
    
    if(result)
    {
        for(i = pad_h,iA = 0;((i < result->h-pad_h) && iA < a->h);i++,iA++)
        {
            for(j = pad_w,jA = 0;((j < result->w-pad_w)&&(jA < a->w));j++,jA++)
            {
                for(terzaDim = 0;terzaDim < a->k;terzaDim++)
                {
                    result->data[i][j][terzaDim] = a->data[iA][jA][terzaDim];
                }
            }
        }
        compute_stats(result);
        return result;
    }
    else
    {
        printf("\nCreazione ip_mat padding non andata a buon fine");
        exit(1);
    }
}


/*FUNZIONI PER LA CONVOLVE*/

/*Funzione che mi ritorna il valore che devo scrivere nella cella
della matrice risultante*/
float Aux_valore_cella(ip_mat *submatrix,ip_mat *filter,unsigned int canale)
{
    unsigned int     i;
    unsigned int     j;
    unsigned int     TerzaDim = 0;
    float            valoreDiRitorno = 0;

    for(i = 0;i < submatrix->h;i++)
    {
        for(j = 0;j < submatrix->w;j++)
        {
            valoreDiRitorno += get_val(submatrix,i,j,canale)*get_val(filter,i,j,TerzaDim);
        }
    }
    return valoreDiRitorno;
}


/*Funzione "convolve"*/
ip_mat * ip_mat_convolve(ip_mat * a, ip_mat * f)
{
    unsigned int       i;
    unsigned int       iEnd;
    unsigned int       j;
    unsigned int       jEnd;
    unsigned int       canale;
    ip_mat             *padMatrix;
    ip_mat             *result;
    unsigned int       iResult;
    unsigned int       jResult;
    ip_mat             *subMatrix;
    
    int                pad_h;
    int                pad_w;

    pad_h = (f->h-1)/2;
    pad_w = (f->w-1)/2;
    padMatrix = ip_mat_padding(a,pad_h,pad_w);    
    result = ip_mat_create(a->h,a->w,a->k,0.0);
    for(i = 0,iResult = 0,iEnd = i+(f->h-1);(iEnd < padMatrix->h && iResult < result->h);i++,iEnd++,iResult++)
    {
        for(j = 0,jResult = 0,jEnd = j+(f->w-1);(jEnd < padMatrix->w && jResult < result->w);j++,jEnd++,jResult++)
        {
            subMatrix = ip_mat_subset(padMatrix,i,iEnd,j,jEnd);            
            for(canale = 0;canale < a->k;canale++)
            {
                result->data[iResult][jResult][canale] = Aux_valore_cella(subMatrix,f,canale);                                
            }
            ip_mat_free(subMatrix);
        }
    }
    ip_mat_free(padMatrix);
    compute_stats(result);
    return result;

}



/*Creazione dei valori del kernel*/


/*Funzione "SHARPEN-filter"*/
ip_mat * create_sharpen_filter()
{
    ip_mat   *result;

    result = ip_mat_create(3,3,1,0.0);
    set_val(result,0,0,0,0);
    set_val(result,0,1,0,-1);
    set_val(result,0,2,0,0);
    set_val(result,1,0,0,-1);
    set_val(result,1,1,0,5);
    set_val(result,1,2,0,-1);
    set_val(result,2,0,0,0);
    set_val(result,2,1,0,-1);
    set_val(result,2,2,0,0);
    compute_stats(result);
    return result;
}


/*Funzione "EDGE-filter"*/
ip_mat * create_edge_filter()
{
    ip_mat   *result;

    result = ip_mat_create(3,3,1,0.0);
    set_val(result,0,0,0,-1);
    set_val(result,0,1,0,-1);
    set_val(result,0,2,0,-1);
    set_val(result,1,0,0,-1);
    set_val(result,1,1,0,8);
    set_val(result,1,2,0,-1);
    set_val(result,2,0,0,-1);
    set_val(result,2,1,0,-1);
    set_val(result,2,2,0,-1);
    compute_stats(result);
    return result;
}


/*Funzione "EMBOSS-filter"*/
ip_mat * create_emboss_filter()
{
    ip_mat   *result;

    result = ip_mat_create(3,3,1,0.0);
    set_val(result,0,0,0,-2);
    set_val(result,0,1,0,-1);
    set_val(result,0,2,0,0);
    set_val(result,1,0,0,-1);
    set_val(result,1,1,0,1);
    set_val(result,1,2,0,1);
    set_val(result,2,0,0,0);
    set_val(result,2,1,0,1);
    set_val(result,2,2,0,2);
    compute_stats(result);
    return result;
}

/*Funzione "AVERAGE-filter"*/
ip_mat * create_average_filter(unsigned int w,unsigned int h,unsigned int k)
{
    unsigned int      i;
    unsigned int      j;
    unsigned int      terzaDim;
    float             valoreCella;
    ip_mat            *result;

    result = ip_mat_create(h,w,k,0.0);
    if(result)
    {
        valoreCella = 1.0/(w*h);        
        for(i = 0;i < h;i++)
        {
            for(j = 0;j < w;j++)
            {
                for(terzaDim = 0;terzaDim < k;terzaDim++)
                {
                    result->data[i][j][terzaDim] = valoreCella;
                }
            }
        }
        compute_stats(result);
        return result;
    }
    else
    {
        printf("\nFiltro Average non creato correttamente");
        exit(1);
    }
}


/*Funzione per il "FILTRO GAUSSIANO"*/



/*Funzione ausiliaria per il calcolo degli elementi delle celle*/
float Aux_compute_Gauss(long int x,long int y,float sigma)
{
    float          esponenteE;
    float          xcast;
    float          ycast;
    float          denominatore;
    float          result;

    
    xcast = (float) x;
    ycast = (float) y;    
    denominatore = 2*PI*(pow(sigma,2.0));
    esponenteE = -((pow(xcast,2.0)+pow(ycast,2.0)) / (2*(pow(sigma,2.0))));
    result = (1/denominatore)*(exp(esponenteE));    
    return result;
}


ip_mat * create_gaussian_filter(unsigned int w,unsigned int h,unsigned int k, float sigma)
{
    ip_mat        *result;
    long int      xCellaCentrale;
    long int      yCellaCentrale;
    unsigned int  i;
    unsigned int  j;
    unsigned int  terzaDim;
    long int      xDistanza;
    long int      yDistanza;
    float         sommaFiltro = 0.0;

    result = ip_mat_create(h,w,k,0.0);
    if(result)
    {
        xCellaCentrale = w/2;
        yCellaCentrale = h/2;       
        for(i = 0;i < h;i++)
        {
            for(j = 0;j < w;j++)
            {
                for(terzaDim = 0;terzaDim < k;terzaDim++)
                {
                    xDistanza = j-xCellaCentrale;
                    yDistanza = i-yCellaCentrale;                    
                    result->data[i][j][terzaDim] = Aux_compute_Gauss(xDistanza,yDistanza,sigma);
                }
            }
        }
        /*Calcoliamo la somma della matrice filtro*/        
        for(i = 0;i < h;i++)
        {
            for(j = 0;j < w;j++)
            {
                for(terzaDim = 0;terzaDim < k;terzaDim++)
                {
                    sommaFiltro += result->data[i][j][terzaDim];                    
                }
            }
        }        
        /*Normalizziamo con il valore della somma del filtro*/
        for(i = 0;i < h;i++)
        {
            for(j = 0;j < w;j++)
            {
                for(terzaDim = 0;terzaDim < k;terzaDim++)
                {
                    result->data[i][j][terzaDim] = (result->data[i][j][terzaDim])/sommaFiltro;
                }
            }
        }
        compute_stats(result);
        return result;
    }
    else
    {
        printf("\nFiltro Gaussiano non creato correttamente");
        exit(1);
    }
}


/*Funzione "RESCALE"*/

void rescale(ip_mat * t, float new_max)
{
    unsigned int    i;
    unsigned int    j;
    unsigned int    terzaDim = 0;
    float           minCanale;
    float           maxCanale;
    float           valoreScalato;
    float           valore;

    compute_stats(t);
    while(terzaDim < t->k)
    {
        minCanale = t->stat[terzaDim].min;
        maxCanale = t->stat[terzaDim].max;
        for(i = 0;i < t->h;i++)
        {
            for(j = 0;j < t->w;j++)
            {
                valore = t->data[i][j][terzaDim];                
                t->data[i][j][terzaDim] = (valore-minCanale)/(maxCanale-minCanale);
            }
        }
        terzaDim++;
    }    
    /*Ora moltiplico tutti i valori della matrice per "new_max"*/
    for(i = 0;i < t->h;i++)
    {
        for(j = 0;j < t->w;j++)
        {
            for(terzaDim = 0;terzaDim < t->k;terzaDim++)
            {
                valoreScalato = t->data[i][j][terzaDim];
                t->data[i][j][terzaDim] = valoreScalato*new_max;
            }
        }
    }
    compute_stats(t);
}








