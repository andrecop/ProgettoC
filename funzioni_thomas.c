#include <stdio.h>
#include <stdlib.h>





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

/*La cosa che ho lasciato pendente è il vettore a struttura stats
 che non ho trovato scritto al momento se andava inizializzato in qualche modo*/



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
    }
    /*libero la struttura*/
    free(a);        
}








