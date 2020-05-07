/*
 Laboratorio di programmazione A.A. 2019/2020
*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "bmp.h"

#define FLT_MAX 3.402823466e+38F /* max value */
#define FLT_MIN 1.175494351e-38F /* min positive value */
#define PI 3.141592654

typedef struct{
    float min;
    float max;
    float mean;
}stats;

typedef struct {
    unsigned int w; /* <- larghezza */
    unsigned int h; /* <- altezza */
    unsigned int k; /* <- canali */
    stats * stat;   /* <- statistiche per canale */
    float *** data; /* <- matrice 3D di valori float */
}ip_mat;

/**** PARTE 1: TIPO DI DATI ip_mat E MEMORIA ****/

/* Inizializza una ip_mat con dimensioni h w e k. Ogni elemento è inizializzato a v.
 * Inoltre crea un vettore di stats per contenere le statische sui singoli canali.
 * */
ip_mat * ip_mat_create(unsigned int h, unsigned int w, unsigned int k, float v) {
    
    if(h != 0 && w != 0 && k != 0) {                                    // controllo se posso creare una matrice
        ip_mat * new_mat = (ip_mat *)malloc(sizeof(ip_mat));            // creo la nuova matrice
        float *** new_data;                                             // matrice 3D (data)
        unsigned int ih, iw, ik;                                        // variabili per scorrere la matrice 3D
        stats * new_stats;                                              // matrice di stats (3D, una dimensione per canale)

        if(!*new_mat){
            printf("Errore ip_mat_create");
            printf("\n");
            printf("La malloc non è andata a buon fine");
            exit(1);
        }
                                                                        // inizializzo le varibili della matrice
        new_mat->h = h;                                                 // altezza
        new_mat->w = w;                                                 // lunghezza
        new_mat->k = k;                                                 // n° canali

        new_stats = (stats *)malloc(sizeof(stats) * k);                 // creo la matrice di stats

        if(!*new_stats){
            printf("Errore ip_mat_create");
            printf("\n");
            printf("La malloc non è andata a buon fine");
            exit(1);
        }

        for(ik = 0; ik < k; ik++) {                                     // inizializzo stats
            new_stats[ik].max = v;                                      // creo il massimo
            new_stats[ik].min = v;                                      // il minimo
            new_stats[ik].mean = v;                                     // e la media
        }
                                                                        // inizializzo la matrice 3D
        new_data = (float ***)malloc(sizeof(float **) * h);             // creo matrice altezza

        if(!***new_data){
            printf("Errore ip_mat_create");
            printf("\n");
            printf("La malloc non è andata a buon fine");
            exit(1);
        }
    
        for(ih = 0; ih < h; ih++) {
            new_data[ih] = (float **)malloc(sizeof(float *) * w);       // aggiungo dimensione lunghezza

            if(!**new_data[ih]){
                printf("Errore ip_mat_create");
                printf("\n");
                printf("La malloc non è andata a buon fine");
                exit(1);
            }

            for(iw = 0; iw < w; iw++) {
                new_data[ih][iw] = (float *)malloc(sizeof(float) * k);  // aggiungo i canali

                if(!*new_data[ih][iw]){
                    printf("Errore ip_mat_create");
                    printf("\n");
                    printf("La malloc non è andata a buon fine");
                    exit(1);
                }

                for(ik = 0; ik < k; ik++) {
                    new_data[ih][iw][ik] = v;                           // setto il valore di default
                }
            }
        }

        new_mat->stat = new_stats;                                      // collego stat
        new_mat->data = new_data;                                       // e data

        return new_mat;                                                 // ritorno la matrice inizializzata
    } else {
        printf("Errore ip_mat_create");
        printf("\n");
        printf("La matrice deve avere tre dimensioni");
        exit(1);
    }
}

/* Libera la memoria (data, stat e la struttura) */
void ip_mat_free(ip_mat *a) {

    if(*a) {                                                            // controllo che esista la matrice
        int ih, iw, ik;                                                 // variabili per scorrere

        for(ik=0; ik < a->k; ik++) {
            free(a->stat);                                              // libero i canali di stat
        }

        for(ih = 0; ih < a->h; ih++) {
            for(iw = 0; iw < a->w; iw++) {
                free(a->data[ih][iw]);                                  // libero i canali per ciascuna lunghezza
            }
            free(a->data[ih]);                                          // libero la lunghezza per ciascuna altezza
        }

        free(a->data);                                                  // libero l'altezza
        free(a);                                                        // libero la matrice che conteneva tutto

        return;                                                         // fine

    } else {
        printf("Errore ip_mat_free");
        printf("\n");
        printf("La matrice non esiste");
        exit(1);
    }
}

/* Restituisce il valore in posizione i,j,k */
float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k) {

    if((i > 0 && i < a->h) && (j > 0 && j < a->w) && (k > 0 && k < a->k)) {                              // controllo che la posizione abbia senso
        return a->data[i][j][k];                                        // restituisco il valore
    } else {
        printf("Errore get_val");
        printf("\n");
        printf("Le posizioni in ingresso non rientrano nella matrice");
        exit(1);
    }
}

/* Setta il valore in posizione i,j,k a v*/
void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v) {

    if((i > 0 && i < a->h) && (j > 0 && j < a->w) && (k > 0 && k < a->k)) {                              // controllo che la posizione abbia senso
        a->data[i][j][k] = v;                                           // setto il valore
        return;
    } else {
        printf("Errore set_val");
        printf("\n");
        printf("Le posizioni in ingresso non rientrano nella matrice");
        exit(1);
    }
}

/* Calcola il valore minimo, il massimo e la media per ogni canale
 * e li salva dentro la struttura ip_mat stats
 * */
void compute_stats(ip_mat * t) {

    if(*a) {                                                            // controllo che esista la matrice
        int ih, iw, ik;                                                 // variabili per scorrere la matrice 3D
        float val;                                                      // variabile per salvare il valore in "data"
        float minimo = FLT_MAX;                                         // setto il minimo al max float per cercare valori più piccoli
        float massimo = FLT_MIN;                                        // (il contrario di sopra ↑)
        float media;                                                    // qui calcolerò la media

        for(ik = 0; ik < t->k; ik++) {
            for(ih = 0; ih < t->h; ih++) {
                for(iw = 0; iw < t->w; iw++) {
                        val = t->data[ih][iw][ik];                      // estraggo il valore
                        if(val < minimo) {
                            minimo = val;                               // salvo il minimo (di sicuro sarà più piccolo di max float)
                        }
                        if(val > massimo) {
                            massimo = val;                              // (il contrario di sopra ↑)
                        }
                    
                }
            }

            media = (massimo + minimo) / 2.0;                           // calcolo la media
            t->stat[ik].max = massimo;                                  // setto il massimo
            t->stat[ik].min = minimo;                                   // il minimo
            t->stat[ik].mean = media;                                   // e la media
        }

        return;                                                         // fine
    } else {
        printf("Errore compute_stats");
        printf("\n");
        printf("La matrice non esiste");
        exit(1);
    }
}

/* Inizializza una ip_mat con dimensioni w h e k.
 * Ogni elemento è generato da una gaussiana con media mean e varianza var */
void ip_mat_init_random(ip_mat * t, float mean, float var) {
                                                                            // controllo che esista la matrice
    if(*t && (var > 0 || var < 0)) {                                        // e che la varianza non sia nulla
        unsigned int ih, iw, ik;                                            // variabili per scorrere

        for(ih = 0; ih < t->h; ih++) {
            for(iw = 0; iw < t->w; iw++) {
                for(ik = 0; ik < t->k; ik++) {
                    t->data[ih][iw][ik] = get_normal_random() * var + mean; // genero i valori
                }
            }
        }

        compute_stats(t);                                                   // calcolo le stats
        return;                                                             // fine

    } else if (var == 0) {
        printf("Errore ip_mat_init_random");
        printf("\n");
        printf("La varianza non può essere nulla");
        exit(1);
    } else {
        printf("Errore ip_mat_init_random");
        printf("\n");
        printf("La matrice non esiste");
        exit(1);
    }
}


/* Crea una copia di una ip_mat e lo restituisce in output */
ip_mat * ip_mat_copy(ip_mat * in){
    if(*a) {                                                            // controllo che esista la matrice
        ip_mat * copia;                                                 // nuova matrice (copia)
        unsigned int ih, iw, ik;                                        // variabili per scorrere

        copia = ip_mat_create(h, w, k, 0.0);                            // creo una nuova ip_mat

        for(ih = 0; ih < in->h; ih++){
            for(iw = 0; iw < in->w; iw++){
                for(ik = 0; ik < in->k; ik++){
                    copia->data[ih][iw][ik] = in->data[ih][iw][ik];     // copio i valori di data
                }
            }
        }

        for(ik = 0; ik < in->k; ik++){
            copia->stat[ik].max = in->stat[ik].max;                     // copio in stat il massimo
            copia->stat[ik].min = in->stat[ik].min;                     // il minimo
            copia->stat[ik].mean = in->stat[ik].mean;                   // e la media
        }

        return copia;                                                   // fine

    } else {
        printf("Errore ip_mat_copy");
        printf("\n");
        printf("La matrice non esiste");
        exit(1);
    }
}

/* Restituisce una sotto-matrice, ovvero la porzione individuata da:
 * t->data[row_start...row_end][col_start...col_end][0...k]
 * La terza dimensione la riportiamo per intero, stiamo in sostanza prendendo un sottoinsieme
 * delle righe e delle colonne.
 * */
ip_mat * ip_mat_subset(ip_mat * t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end){

    // controllo la matrice esista e che row e col (sia start che end) siano all'interno della matrice in ingresso
    if(*t && (row_start > 0 && row_start < t->h) && (row_end > 0 && row_end < t->h) && (col_start > 0 && col_start < t->w) && (col_end > 0 && col_end < t->w)){
        ip_mat * sub;                                                                   // nuova sotto-matrice
        unsigned int ih, iw, ik;                                                        // variabili per scorrere
        
        sub = ip_mat_create(h - row_start, w - col_start, k, 0.0);                      // creo una nuova ip_mat
        
        for(ih = row_start; ih < row_end; ih++){
            for(iw = col_start; iw < col_end; iw++){
                for(ik = 0; ik < t->k; ik++){
                    sub->data[ih-row_start][iw-col_start][ik] = t->data[ih][iw][ik];    // copio i valori di data
                }
            }
        }

        compute_stats(sub);                                                             // calcolo le stats

        return sub;                                                                     // fine

    } else if (*t) {
        printf("Errore ip_mat_subset");
        printf("\n");
        printf("Le posizioni in ingresso non sono presenti all'interno della matrice");
        exit(1);
    } else {
        printf("Errore ip_mat_subset");
        printf("\n");
        printf("La matrice in ingresso non esiste");
        exit(1);
    }
}

/* Concatena due ip_mat su una certa dimensione.
 * Ad esempio:
 * ip_mat_concat(ip_mat * a, ip_mat * b, 0);
 *      produrrà un nuovo ip_mat di dimensioni:
 *      out.h = a.h + b.h
 *      out.w = a.w = b.w
 *      out.k = a.k = b.k
 *
 * ip_mat_concat(ip_mat * a, ip_mat * b, 1);
 *      produrrà un nuovo ip_mat di dimensioni:
 *      out.h = a.h = b.h
 *      out.w = a.w + b.w
 *      out.k = a.k = b.k
 *
 * ip_mat_concat(ip_mat * a, ip_mat * b, 2);
 *      produrrà un nuovo ip_mat di dimensioni:
 *      out.h = a.h = b.h
 *      out.w = a.w = b.w
 *      out.k = a.k + b.k
 * */
ip_mat * ip_mat_concat(ip_mat * a, ip_mat * b, int dimensione){

    if(*a && *b && (dimensione < 3 && dimensione >= 0)) {

        ip_mat * concat;                                                                // nuova matrice
        unsigned int ih, iw, ik;                                                        // variabili per scorrere la matrice 3D
        unsigned int h, w, k;                                                           // dimensioni della nuova matrice

        h = a->h;                                                                       // altezza
        w = a->w;                                                                       // larghezza
        k = a->k;                                                                       // numero di canali

                                                                                        // concateno
        if(dimensione == 0) {
            h = a->h + b->h;                                                            // correggo l'altezza
        } else if(dimensione == 1) {
            w = a->w + b->w;                                                            // correggo la larghezza
        } else if(dimensione == 2) {
            k = a->k + b->k;                                                            // correggo il numero di canali
        }

        concat = ip_mat_create(h, w, k, 0.0);                                           // creo una nuova ip_mat
                                                                                        // con le dimensioni corrette

        for(ih = 0; ih < h; ih++) {
            for(iw = 0; iw < w; iw++) {
                for(ik = 0; ik < k; ik++) {
                    if(dimensione == 0) {                                               // concateno altezza
                        if(ih < a->h) {
                            concat->data[ih][iw][ik] = a->data[ih][iw][ik];             // copio i valori di *a
                        } else {
                            concat->data[ih][iw][ik] = b->data[ih-a->h][iw][ik];        // copio i valori di *b
                        }
                    } else if (dimensione == 1) {                                       // concateno lunghezza
                        if(iw < a->w) {
                            concat->data[ih][iw][ik] = a->data[ih][iw][ik];             // copio i valori di *a
                        } else {
                            concat->data[ih][iw][ik] = b->data[ih][iw-a->w][ik];        // copio i valori di *b
                        }
                    } else if (dimensione == 2) {                                       // concateno canali
                        if(ik < a->k) {
                            concat->data[ih][iw][ik] = a->data[ih][iw][ik];             // copio i valori di *a
                        } else {
                            concat->data[ih][iw][ik] = a->data[ih][iw][ik-a->k];        // copio i valori di *b
                        }
                    }
                }
            }
        }

        compute_stats(concat);                                                          // calcolo le stats

        return concat;                                                                  // fine

    } else if(!*a) {
        printf("Errore ip_mat_concat");
        printf("\n");
        printf("La matrice *a non esiste");
        exit(1);
    } else if(!*b) {
        printf("Errore ip_mat_concat");
        printf("\n");
        printf("La matrice *b non esiste");
        exit(1);
    } else {
        printf("Errore ip_mat_concat");
        printf("\n");
        printf("La dimensione deve essere compresa fra 0 e 2");
        exit(1);
    }
}

/**** PARTE 1: OPERAZIONI MATEMATICHE FRA IP_MAT ****/
/* Esegue la somma di due ip_mat (tutte le dimensioni devono essere identiche)
 * e la restituisce in output. */
ip_mat * ip_mat_sum(ip_mat * a, ip_mat * b){

    if(*a && *b && (a->h == b->h) && (a->w == b->w) && (a->k == b->k)) {
        ip_mat * somma;                                                                     // nuova matrice
        unsigned int ih, iw, ik;                                                            // variabili per scorrere la matrice 3D

        somma = ip_mat_create(a->h, a->w, a->k, 0.0);                                       // creo una nuova ip_mat

        for(ih = 0; ih < somma->h; ih++){
            for(iw = 0; iw < somma->w; iw++){
                for(ik = 0; ik < somma->k; ik++){
                    somma->data[ih][iw][ik] = a->data[ih][iw][ik] + b->data[ih][iw][ik];    // sommo i valori e li inserisco in data
            }
        }

        compute_stats(somma);                                                               // calcolo le stats

        return somma;                                                                       // fine

    } else if(!*a) {
        printf("Errore ip_mat_sum");
        printf("\n");
        printf("La matrice *a non esiste");
        exit(1);
    } else if(!*b) {
        printf("Errore ip_mat_sum");
        printf("\n");
        printf("La matrice *b non esiste");
        exit(1);
    } else {
        printf("Errore ip_mat_sum");
        printf("\n");
        printf("Le matrici devono avere le stesse dimensioni");
        exit(1);
    }
}

/* Esegue la sottrazione di due ip_mat (tutte le dimensioni devono essere identiche)
 * e la restituisce in output.
 * */
ip_mat * ip_mat_sub(ip_mat * a, ip_mat * b){

    if(*a && *b && (a->h == b->h) && (a->w == b->w) && (a->k == b->k)) {
        ip_mat * sottrazione;                                                                   // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                                // variabili per scorrere

        sottrazione = ip_mat_create(a->h, a->w, a->k, 0.0);                                     // creo una nuova ip_mat

        for(ih = 0; ih < sottrazione->h; ih++){
            for(iw = 0; iw < sottrazione->w; iw++){
                for(ik = 0; ik < sottrazione->k; ik++){
                    sottrazione->data[ih][iw][ik] = a->data[ih][iw][ik] - b->data[ih][iw][ik];  // sottraggo i valori e li inserisco in data
                }
            }
        }

        compute_stats(sottrazione);                                                             // calcolo le stats
        return sottrazione;                                                                     // fine

    } else if(!*a) {
        printf("Errore ip_mat_sub");
        printf("\n");
        printf("La matrice *a non esiste");
        exit(1);
    } else if(!*b) {
        printf("Errore ip_mat_sub");
        printf("\n");
        printf("La matrice *b non esiste");
        exit(1);
    } else {
        printf("Errore ip_mat_sub");
        printf("\n");
        printf("Le matrici devono avere le stesse dimensioni");
        exit(1);
    }
}

/* Moltiplica un ip_mat per uno scalare c. Si moltiplica c per tutti gli elementi di "a"
 * e si salva il risultato in un nuovo tensore in output. */
ip_mat * ip_mat_mul_scalar(ip_mat *a, float c){

    if(*a) {
        ip_mat * moltiplicaScalare;                                                 // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                    // variabili per scorrere

        moltiplicaScalare = ip_mat_create(a->h, a->w, a->k, 0.0);                   // creo una nuova ip_mat

        for(ih = 0; ih < moltiplicaScalare->h; ih++){
            for(iw = 0; iw < moltiplicaScalare->w; iw++){
                for(ik = 0; ik < moltiplicaScalare->k; ik++){
                    moltiplicaScalare->data[ih][iw][ik] = a->data[ih][iw][ik] * c;  // moltiplico i valori di *a per lo scalare c
                }                                                                   // e li inserisco in data
            }
        }
        compute_stats(moltiplicaScalare);                                           // calcolo le stats
        return moltiplicaScalare;                                                   // fine

    } else {
        printf("Errore ip_mat_mul_scalar");
        printf("\n");
        printf("La matrice in ingresso non esiste");
        exit(1);
    }
}

/* Aggiunge ad un ip_mat uno scalare c e lo restituisce in un nuovo tensore in output. */
ip_mat *  ip_mat_add_scalar(ip_mat *a, float c){

    if(*a) {
        ip_mat * sommaScalare;                                                  // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                // variabili per scorrere

        sommaScalare = ip_mat_create(a->h, a->w, a->k, 0.0);                    // creo una nuova ip_mat

        for(ih = 0; ih < sommaScalare->h; ih++){
            for(iw = 0; iw < sommaScalare->w; iw++){
                for(ik = 0; ik < sommaScalare->k; ik++){
                    sommaScalare->data[ih][iw][ik] = a->data[ih][iw][ik] + c;   // sommo i valori di *a per lo scalare c
                }                                                               // e li inserisco in data
            }
        }
        compute_stats(sommaScalare);                                            // calcolo le stats
        return sommaScalare;                                                    // fine

    } else {
        printf("Errore ip_mat_add_scalar");
        printf("\n");
        printf("La matrice in ingresso non esiste");
        exit(1);
    }
}

/* Calcola la media di due ip_mat a e b e la restituisce in output.*/
ip_mat * ip_mat_mean(ip_mat * a, ip_mat * b){
    if(*a && *b) {
        ip_mat * media;                                                                         // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                                // variabili per scorrere

        media = ip_mat_create(a->h, a->w, a->k, 0.0);                                           // creo una nuova ip_mat

        for(ih = 0; ih < media->h; ih++){
            for(iw = 0; iw < media->w; iw++){
                for(ik = 0; ik < media->k; ik++){
                    media->data[ih][iw][ik] = (a->data[ih][iw][ik] + b->data[ih][iw][ik]) / 2.0;  // calcolo la media dei valori
                }                                                                               // presenti in *a e *b e li
            }                                                                                   // inserisco in data
        }

        compute_stats(media);                                                                   // calcolo le stats

        return media;                                                                           // fine

    } else if(!*a) {
        printf("Errore ip_mat_mean");
        printf("\n");
        printf("La matrice *a non esiste");
        exit(1);
    } else {
        printf("Errore ip_mat_mean");
        printf("\n");
        printf("La matrice *b non esiste");
        exit(1);
    }
}

/**** PARTE 2: SEMPLICI OPERAZIONI SU IMMAGINI ****/
/* Converte un'immagine RGB ad una immagine a scala di grigio.
 * Quest'operazione viene fatta calcolando la media per ogni pixel sui 3 canali
 * e creando una nuova immagine avente per valore di un pixel su ogni canale la media appena calcolata.
 * Avremo quindi che tutti i canali saranno uguali.
 * */
ip_mat * ip_mat_to_gray_scale(ip_mat * in) {
    if(*a) {
        ip_mat * scalaGrigi;                                                                    // nuova matrice
        unsigned int ih, iw, ik;                                                                // variabili per scorrere
        float media = 0.0;                                                                      // media
        float nCanali = in->k;                                                                   // numero di canali

        scalaGrigi = ip_mat_create(in->h, in->w, in->k, 0.0);                                   // creo una nuova ip_mat

        for(ih = 0; ih < scalaGrigi->h; ih++){
            for(iw = 0; iw < scalaGrigi->w; iw++){
                for(ik = 0; ik < scalaGrigi->k; ik++){
                    media += in->data[ih][iw][ik];  // calcolo la media dei valori
                }                                                                               
                media /= nCanali;
                for(ik = 0; ik < scalaGrigi->k; ik++){
                   scalaGrigi->data[ih][iw][ik] = media;  // calcolo la media dei valori
                }
                media = 0.0;
            }                                                                                   // inserisco in data
        }

        compute_stats(scalaGrigi);                                                                   // calcolo le stats

        return scalaGrigi;                                                                           // fine

    } else {
        printf("Errore ip_mat_to_gray_scale");
        printf("\n");
        printf("La matrice in ingresso non esiste");
        exit(1);
    }
}

/* Effettua la fusione (combinazione convessa) di due immagini */
ip_mat * ip_mat_blend(ip_mat * a, ip_mat * b, float alpha){
    if(*a && *b) {
        ip_mat * bleded;                                                                         // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                                // variabili per scorrere

        bleded = ip_mat_create(a->h, a->w, a->k, 0.0);                                           // creo una nuova ip_mat

        for(ih = 0; ih < bleded->h; ih++){
            for(iw = 0; iw < bleded->w; iw++){
                for(ik = 0; ik < bleded->k; ik++){
                    bleded->data[ih][iw][ik] = alpha*(a->data[ih][iw][ik]) + (1 - alpha)*(b->data[ih][iw][ik]);  // calcolo la media dei valori
                }                                                                               // presenti in *a e *b e li
            }                                                                                   // inserisco in data
        }

        compute_stats(bleded);                                                                   // calcolo le stats

        return bleded;                                                                           // fine

    } else if(!*a) {
        printf("Errore ip_mat_blend");
        printf("\n");
        printf("La matrice *a non esiste");
        exit(1);
    } else {
        printf("Errore ip_mat_blend");
        printf("\n");
        printf("La matrice *b non esiste");
        exit(1);
    }
}

/* Operazione di brightening: aumenta la luminosità dell'immagine
 * aggiunge ad ogni pixel un certo valore*/
ip_mat * ip_mat_brighten(ip_mat * a, float bright){
    if(*a) {
        ip_mat * brightend;                                                                         // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                                // variabili per scorrere

        brightend = ip_mat_create(a->h, a->w, a->k, 0.0);                                           // creo una nuova ip_mat

        for(ih = 0; ih < brightend->h; ih++){
            for(iw = 0; iw < brightend->w; iw++){
                for(ik = 0; ik < brightend->k; ik++){
                    brightend->data[ih][iw][ik] = bright*(a->data[ih][iw][ik]);  // calcolo la media dei valori
                }                                                                               // presenti in *a e *b e li
            }                                                                                   // inserisco in data
        }

        compute_stats(brightend);                                                                   // calcolo le stats

        return brightend;                                                                           // fine

    } else if(!*a) {
        printf("Errore ip_mat_brighten");
        printf("\n");
        printf("La matrice in ingresso non esiste");
        exit(1);
    }
}

/* Operazione di corruzione con rumore gaussiano:
 * Aggiunge del rumore gaussiano all'immagine, il rumore viene enfatizzato
 * per mezzo della variabile amount.
 * out = a + gauss_noise*amount
 * */
ip_mat * ip_mat_corrupt(ip_mat * a, float amount){
    if(*a) {
        ip_mat * corrupted;                                                                         // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                                // variabili per scorrere

        corrupted = ip_mat_create(a->h, a->w, a->k, 0.0);                                           // creo una nuova ip_mat

        for(ih = 0; ih < corrupted->h; ih++){
            for(iw = 0; iw < corrupted->w; iw++){
                for(ik = 0; ik < corrupted->k; ik++){
                    corrupted->data[ih][iw][ik] = a->data[ih][iw][ik] + get_normal_random() * amount;  // calcolo la media dei valori
                }                                                                               // presenti in *a e *b e li
            }                                                                                   // inserisco in data
        }

        compute_stats(corrupted);                                                                   // calcolo le stats

        return corrupted;                                                                           // fine

    } else if(!*a) {
        printf("Errore ip_mat_corrupt");
        printf("\n");
        printf("La matrice in ingresso non esiste");
        exit(1);
    }
}

/**** PARTE 3: CONVOLUZIONE E FILTRI *****/

/* Effettua la convoluzione di un ip_mat "a" con un ip_mat "f".
 * La funzione restituisce un ip_mat delle stesse dimensioni di "a".
 * */
ip_mat * ip_mat_convolve(ip_mat * a, ip_mat * f);

/* Aggiunge un padding all'immagine. Il padding verticale è pad_h mentre quello
 * orizzontale è pad_w.
 * L'output sarà un'immagine di dimensioni:
 *      out.h = a.h + 2*pad_h;
 *      out.w = a.w + 2*pad_w;
 *      out.k = a.k
 * con valori nulli sui bordi corrispondenti al padding e l'immagine "a" riportata
 * nel centro
 * */
ip_mat * ip_mat_padding(ip_mat * a, int pad_h, int pad_w);

/* Crea un filtro di sharpening */
ip_mat * create_sharpen_filter();

/* Crea un filtro per rilevare i bordi */
ip_mat * create_edge_filter();

/* Crea un filtro per aggiungere profondità */
ip_mat * create_emboss_filter();

/* Crea un filtro medio per la rimozione del rumore */
ip_mat * create_average_filter(int w, int h, int k);

/* Crea un filtro gaussiano per la rimozione del rumore */
ip_mat * create_gaussian_filter(int w, int h, int k, float sigma);

/* Effettua una riscalatura dei dati tale che i valori siano in [0,new_max].
 * Utilizzate il metodo compute_stat per ricavarvi il min, max per ogni canale.
 *
 * I valori sono scalati tramite la formula valore-min/(max - min)
 *
 * Si considera ogni indice della terza dimensione indipendente, quindi l'operazione
 * di scalatura va ripetuta per ogni "fetta" della matrice 3D.
 * Successivamente moltiplichiamo per new_max gli elementi della matrice in modo da ottenere un range
 * di valori in [0,new_max].
 * */
void rescale(ip_mat * t, float new_max);

/* Nell'operazione di clamping i valori <low si convertono in low e i valori >high in high.*/
void clamp(ip_mat * t, float low, float high);

/**** METODI GIA' IMPLEMENTATI ****/
/* Genera dei numeri casuali con distribuzione Normale (versione base)
 * https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
 * */
float get_normal_random();

/* Converte una Bitmap in una ip_mat*/
ip_mat * bitmap_to_ip_mat(Bitmap * img);

/* Converte una ip_mat in una bitmap*/
Bitmap * ip_mat_to_bitmap(ip_mat * t);

/* Visualizza i dati stampando in ordine le matrici rispetto
 * la terza dimensione.
 * Prima stamperemo t->data[...][...][0] poi t->data[...][...][1] ...
 * */
void ip_mat_show(ip_mat * t);

/* Visualizza a video le statistiche per ogni canale.
 * */
void ip_mat_show_stats(ip_mat * t);