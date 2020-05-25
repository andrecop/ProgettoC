/*
 Created by Sebastiano Vascon on 23/03/20.
*/

#include <stdio.h>
#include "ip_lib.h"
#include "bmp.h"

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

    compute_stats(out);

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

float get_normal_random(float media, float std){

    float y1 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float y2 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float num = cos(2*PI*y2)*sqrt(-2.*log(y1));

    return media + num*std;
}

ip_mat * ip_mat_create(unsigned int h, unsigned int w, unsigned int k, float v) {

    if(h > 0 && w > 0 && k > 0) {                                       // controllo se posso creare una matrice
        ip_mat * new_mat = (ip_mat *)malloc(sizeof(ip_mat));            // creo la nuova matrice
        float *** new_data;                                             // matrice 3D (data)
        unsigned int ih, iw, ik;                                        // variabili per scorrere la matrice 3D
        stats * new_stats;                                              // matrice di stats (3D, una dimensione per canale)

                                                                        // inizializzo le varibili della matrice
        new_mat->h = h;                                                 // altezza
        new_mat->w = w;                                                 // lunghezza
        new_mat->k = k;                                                 // n° canali

        new_stats = (stats *)malloc(sizeof(stats) * k);                 // creo la matrice di stats

        for(ik = 0; ik < k; ik++) {                                     // inizializzo stats
            new_stats[ik].max = v;                                      // creo il massimo
            new_stats[ik].min = v;                                      // il minimo
            new_stats[ik].mean = v;                                     // e la media
        }
                                                                        // inizializzo la matrice 3D
        new_data = (float ***)malloc(sizeof(float **) * h);             // creo matrice altezza

        for(ih = 0; ih < h; ih++) {
            new_data[ih] = (float **)malloc(sizeof(float *) * w);       // aggiungo dimensione lunghezza



            for(iw = 0; iw < w; iw++) {
                new_data[ih][iw] = (float *)malloc(sizeof(float) * k);  // aggiungo i canali



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
        exit(1);
    }
}

void ip_mat_free(ip_mat *a) {

    if(a) {                                                             // controllo che esista la matrice
        int ih, iw;                                                     // variabili per scorrere

        free(a->stat);                                                  // libero i canali di stat

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
        exit(1);
    }
}

float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k) {

    if(a && (i >= 0 && i < a->h) && (j >= 0 && j < a->w) && (k >= 0 && k < a->k)) {     // controllo che la posizione abbia senso
        return a->data[i][j][k];                                                        // restituisco il valore

    } else {
        printf("Errore get_val");
        exit(1);
    }
}

void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v) {

    if(a && (i >= 0 && i < a->h) && (j >= 0 && j < a->w) && (k >= 0 && k < a->k)) {     // controllo che la posizione abbia senso
        a->data[i][j][k] = v;                                                           // setto il valore

        
        return;
    } else {
        printf("Errore set_val");
        exit(1);
    }
}

void compute_stats_channel(ip_mat * t, int k, float * minimo, float * massimo, float * somma_valori, unsigned int * quantita_valori){

    if(t && k >= 0 && k < t->k) {
        
        int ih, iw;                                                                     // variabili per scorrere la matrice 3D
        float val;                                                                      // variabile per salvare il valore in "data"

        for(ih = 0; ih < t->h; ih++) {
            for(iw = 0; iw < t->w; iw++) {
                val = t->data[ih][iw][k];                                   // estraggo il valore
                *somma_valori += val;                                       // salvo la somma di tutti i valori
                (*quantita_valori)++;                                         // conto quanti valori ci sono nel canale
                if(val < *minimo) {
                    *minimo = val;                                          // salvo il minimo (di sicuro sarà più piccolo di max float)
                }
                if(val > *massimo) {
                    *massimo = val;                                         // (il contrario di sopra ↑)
                }
            }
        }

        return;

    } else {
        printf("Errore compute_stats_channel");
        exit(1);
    }
} 

void compute_stats(ip_mat * t) {

    if(t) {                                                                                     // controllo che esista la matrice

        int ik;                                                                                 // variabili per scorrere la matrice 3D
        float somma_valori;                                                                     // variabile per conteggiare la somma dei valori per canale
        unsigned int quantita_valori;                                                           // variabile per conteggiare i valori per canale
        float minimo;                                                                           // variabile per conteggiare il minimo
        float massimo;                                                                          // variabile per conteggiare il massimo
        float media;                                                                            // variabile per conteggiare la media

        for(ik = 0; ik < t->k; ik++) {
            minimo = FLT_MAX;                                                                   // setto a MAX FLOAT per cercare valori sempre più piccoli
            massimo = FLT_MIN;                                                                  // setto a MIN FLOAT per cercare valori sempre più grandi
            somma_valori = 0;                                                                   // inizializzo
            quantita_valori = 0;                                                                // inizializzo
            compute_stats_channel(t, ik, &minimo, &massimo, &somma_valori, &quantita_valori);   // computo tutte le stats del canale
            media = somma_valori / (float)quantita_valori;                                      // calcolo la media
            t->stat[ik].max = massimo;                                                          // setto in *t il massimo
            t->stat[ik].min = minimo;                                                           // il minimo
            t->stat[ik].mean = media;                                                           // e la media
        }

        return;                                                                                 // fine

    } else {
        printf("Errore compute_stats");
        exit(1);
    }
}

void ip_mat_init_random(ip_mat * t, float mean, float var) {
                                                                                                // controllo che esista la matrice
    if(t && (var > 0 || var < 0)) {                                                             // e che la varianza non sia nulla
        unsigned int ih, iw, ik;                                                                // variabili per scorrere

        for(ih = 0; ih < t->h; ih++) {
            for(iw = 0; iw < t->w; iw++) {
                for(ik = 0; ik < t->k; ik++) {
                    t->data[ih][iw][ik] = get_normal_random(mean, var*var);                     // genero i valori
                }
            }
        }

        compute_stats(t);                                                                       // calcolo le stats
        return;                                                                                 // fine

    } else {
        printf("Errore ip_mat_init_random");
        exit(1);
    }
}

/* Crea una copia di una ip_mat e lo restituisce in output */
ip_mat * ip_mat_copy(ip_mat * in){

    if(in) {                                                                        // controllo che esista la matrice
        ip_mat * copia;                                                             // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                    // variabili per scorrere

        copia = ip_mat_create(in->h, in->w, in->k, 0.0);                            // creo una nuova ip_mat

        for(ih = 0; ih < in->h; ih++){
            for(iw = 0; iw < in->w; iw++){
                for(ik = 0; ik < in->k; ik++){
                    copia->data[ih][iw][ik] = in->data[ih][iw][ik];                 // copio i valori di data
                }
            }
        }

        for(ik = 0; ik < in->k; ik++){
            copia->stat[ik].max = in->stat[ik].max;                                 // copio in stat il massimo
            copia->stat[ik].min = in->stat[ik].min;                                 // il minimo
            copia->stat[ik].mean = in->stat[ik].mean;                               // e la media
        }

        return copia;                                                               // fine

    } else {
        printf("Errore ip_mat_copy");
        exit(1);
    }
}

ip_mat * ip_mat_subset(ip_mat * t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end){

    // controllo la matrice esista e che row e col (sia start che end) siano all'interno della matrice in ingresso
    if(t && (row_start >= 0 && row_start <= row_end && row_start <= t->h) && (row_end >= 0 && row_end >= row_start && row_end <= t->h) && (col_start >= 0 && col_start <= col_end && col_start <= t->w) && (col_end >= 0 && col_end >= col_start && col_end <= t->w)){
        ip_mat * sub;                                                                       // nuova sotto-matrice
        unsigned int ih, iw, ik;                                                            // variabili per scorrere

        sub = ip_mat_create((row_end+1 - row_start), (col_end+1 - col_start), t->k, 0.0);   // creo una nuova ip_mat

        for(ih = row_start; ih < row_end; ih++){
            for(iw = col_start; iw < col_end; iw++){
                for(ik = 0; ik < t->k; ik++){
                    sub->data[ih-row_start][iw-col_start][ik] = t->data[ih][iw][ik];        // copio i valori di data
                }
            }
        }

        compute_stats(sub);                                                                 // calcolo le stats

        return sub;                                                                         // fine

    } else {
        printf("Errore ip_mat_subset");
        exit(1);
    }
}

ip_mat * ip_mat_concat(ip_mat * a, ip_mat * b, int dimensione){

    if(a && b && (dimensione < 3 && dimensione >= 0)) {

        ip_mat * concat;                                                                // nuova matrice
        unsigned int ih, iw, ik;                                                        // variabili per scorrere la matrice 3D
        unsigned int h, w, k;                                                           // dimensioni della nuova matrice

        h = a->h;                                                                       // altezza
        w = a->w;                                                                       // larghezza
        k = a->k;                                                                       // numero di canali

                                                                                        // concateno
        if(dimensione == 0) {
            h = a->h + b->h;                                                            // correggo l'altezza
        } else if (dimensione == 1) {
            w = a->w + b->w;                                                            // correggo la larghezza
        } else if (dimensione == 2) {
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

    } else {
        printf("Errore ip_mat_concat");
        exit(1);
    }
}

ip_mat * ip_mat_sum(ip_mat * a, ip_mat * b){

    if(a && b && (a->h == b->h) && (a->w == b->w) && (a->k == b->k)) {

        ip_mat * somma;                                                                     // nuova matrice
        unsigned int ih, iw, ik;                                                            // variabili per scorrere la matrice 3D

        somma = ip_mat_create(a->h, a->w, a->k, 0.0);                                       // creo una nuova ip_mat

        for(ih = 0; ih < somma->h; ih++){
            for(iw = 0; iw < somma->w; iw++){
                for(ik = 0; ik < somma->k; ik++){
                    somma->data[ih][iw][ik] = a->data[ih][iw][ik] + b->data[ih][iw][ik];    // sommo i valori e li inserisco in data
                }
            }
        }

        compute_stats(somma);                                                               // calcolo le stats

        return somma;                                                                       // fine

    } else {
        printf("Errore ip_mat_sum");
        exit(1);
    }
}

ip_mat * ip_mat_sub(ip_mat * a, ip_mat * b){

    if(a && b && (a->h == b->h) && (a->w == b->w) && (a->k == b->k)) {

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

    } else {
        printf("Errore ip_mat_sub");
        exit(1);
    }
}

ip_mat * ip_mat_mul_scalar(ip_mat *a, float c){

    if(a) {
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
        exit(1);
    }
}

ip_mat *  ip_mat_add_scalar(ip_mat *a, float c){

    if(a) {
        
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
        exit(1);
    }
}

ip_mat * ip_mat_mean(ip_mat * a, ip_mat * b){

    if(a && b) {

        ip_mat * media;                                                                             // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                                    // variabili per scorrere

        media = ip_mat_create(a->h, a->w, a->k, 0.0);                                               // creo una nuova ip_mat

        for(ih = 0; ih < media->h; ih++){
            for(iw = 0; iw < media->w; iw++){
                for(ik = 0; ik < media->k; ik++){
                    media->data[ih][iw][ik] = (a->data[ih][iw][ik] + b->data[ih][iw][ik]) / 2.0;    // calcolo la media dei valori
                }                                                                                   // presenti in *a e *b e li
            }                                                                                       // inserisco in data
        }

        compute_stats(media);                                                                       // calcolo le stats

        return media;                                                                               // fine

    } else {
        printf("Errore ip_mat_mean");
        exit(1);
    }
}

ip_mat * ip_mat_to_gray_scale(ip_mat * in) {

    if(in) {

        ip_mat * scalaGrigi;                                                                    // nuova matrice
        unsigned int ih, iw, ik;                                                                // variabili per scorrere
        float media = 0.0;                                                                      // media
        float nCanali = in->k;                                                                  // numero di canali

        scalaGrigi = ip_mat_create(in->h, in->w, in->k, 0.0);                                   // creo una nuova ip_mat

        for(ih = 0; ih < scalaGrigi->h; ih++){
            for(iw = 0; iw < scalaGrigi->w; iw++){
                for(ik = 0; ik < scalaGrigi->k; ik++){
                    media += in->data[ih][iw][ik];                                              // calcolo la media dei valori
                }
                media /= nCanali;
                for(ik = 0; ik < scalaGrigi->k; ik++){
                   scalaGrigi->data[ih][iw][ik] = media;                                        // calcolo la media dei valori
                }
                media = 0.0;
            }                                                                                   // inserisco in data
        }

        compute_stats(scalaGrigi);                                                              // calcolo le stats
        return scalaGrigi;                                                                      // fine

    } else {
        printf("Errore ip_mat_to_gray_scale");
        exit(1);
    }
}

ip_mat * ip_mat_blend(ip_mat * a, ip_mat * b, float alpha){

    if(a && b) {

        ip_mat * bleded;                                                                                            // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                                                    // variabili per scorrere
        bleded = ip_mat_create(a->h, a->w, a->k, 0.0);                                                              // creo una nuova ip_mat

        for(ih = 0; ih < bleded->h; ih++){
            for(iw = 0; iw < bleded->w; iw++){
                for(ik = 0; ik < bleded->k; ik++){
                    bleded->data[ih][iw][ik] = alpha*(a->data[ih][iw][ik]) + (1 - alpha)*(b->data[ih][iw][ik]);     // calcolo la media dei valori
                }                                                                                                   // presenti in *a e *b e li
            }                                                                                                       // inserisco in data
        }

        compute_stats(bleded);                                                                                      // calcolo le stats
        return bleded;                                                                                              // fine

    } else {
        printf("Errore ip_mat_blend");
        exit(1);
    }
}

ip_mat * ip_mat_brighten(ip_mat * a, float bright){
    if(a) {
        ip_mat * brightend;                                                                         // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                                    // variabili per scorrere

        brightend = ip_mat_create(a->h, a->w, a->k, 0.0);                                           // creo una nuova ip_mat

        for(ih = 0; ih < brightend->h; ih++){
            for(iw = 0; iw < brightend->w; iw++){
                for(ik = 0; ik < brightend->k; ik++){
                    brightend->data[ih][iw][ik] = bright*(a->data[ih][iw][ik]);                     // calcolo la media dei valori
                }                                                                                   // presenti in *a e *b e li
            }                                                                                       // inserisco in data
        }

        compute_stats(brightend);                                                                   // calcolo le stats
        return brightend;                                                                           // fine

    } else {
        printf("Errore ip_mat_brighten");
        exit(1);
    }
}

ip_mat * ip_mat_corrupt(ip_mat * a, float amount){

    if(a) {

        ip_mat * corrupted;                                                                                         // nuova matrice (copia)
        unsigned int ih, iw, ik;                                                                                    // variabili per scorrere
        corrupted = ip_mat_create(a->h, a->w, a->k, 0.0);                                                           // creo una nuova ip_mat

        for(ih = 0; ih < corrupted->h; ih++){
            for(iw = 0; iw < corrupted->w; iw++){
                for(ik = 0; ik < corrupted->k; ik++){
                    corrupted->data[ih][iw][ik] = a->data[ih][iw][ik] + get_normal_random(0.0, 0.9545/2)*amount;    // calcolo la media dei valori
                }                                                                                                   // presenti in *a e *b e li
            }                                                                                                       // inserisco in data
        }

        compute_stats(corrupted);                                                                                   // calcolo le stats
        return corrupted;                                                                                           // fine

    } else {
        printf("Errore ip_mat_corrupt");
        exit(1);
    }
}

void ip_mat_convolve_channel(ip_mat * result, ip_mat * a, ip_mat * f, unsigned int k){ 

    if(a && f){

            unsigned int ih,iw;
            unsigned int ih2,iw2;
            float val = 0.0;

            for(ih = 0; ih < a->h; ih++){
                for(iw = 0; iw < a->w; iw++){
                    for(ih2 = 0; ih2 < f -> h;ih2++){
                        for(iw2 = 0; iw2 < f -> w;iw2++){
                            val+=((result->data[ih+ih2][iw+iw2][k])*(f->data[ih2][iw2][k]));
                        }
                    }
                    result->data[ih][iw][k] = val;
                    val = 0.0;
                }
            }

            return;

    } else {
        printf("Errore ip_mat_convolve_channel");
        exit(1);
    }
}

ip_mat * ip_mat_convolve(ip_mat * a, ip_mat * f){ 

    if(a && f){

            unsigned int k;
            int pad_h, pad_w;                                               //interi richiesti da ip_mat_padding per funzionare
            pad_h = (f -> h - 1) /2;                                        //dimensione padding (per singolo lato) P = (dim.filtro - 1) / 2
            pad_w = (f -> w - 1) /2;
            ip_mat * result;
            result = ip_mat_padding(a, pad_h, pad_w);                       //creazione ip_mat per il risulatato

            for(k = 0; k < f->k; k++){
                ip_mat_convolve_channel(result, a, f, k);
            }

            result = ip_mat_subset(result, 0, a->h, 0, a->w);
            return result;

    } else {
        printf("Errore ip_mat_convolve");
        exit(1);
    }
}

ip_mat * ip_mat_padding(ip_mat * a, unsigned int pad_h, unsigned int pad_w){

    if(a){

        ip_mat * padded;                                       //nuova matrice
        unsigned int ih, iw, ik;                               //variabili scorrimento cicli
        unsigned int ph, pw, pk;                               //nuove dimensioni

        ph = a->h + 2*pad_h;                                   //calcolo dimensioni paddate
        pw = a->w + 2*pad_w;
        pk = a->k;

        padded = ip_mat_create(ph, pw, pk, 0.0);               //creazione ip_mat paddata (vuota)

        // copiatura matrice originale al centro della matrice paddata vuota
        for (ih = 0; ih < a -> h; ih++){
                for (iw = 0; iw < a -> w; iw++){
                    for (ik = 0; ik < a -> k; ik++){
                        padded -> data[ih+pad_h][iw+pad_w][ik] = a -> data[ih][iw][ik];
                    }
                }
        }

        compute_stats(padded);
        return padded;

    } else {
        printf("Errore ip_mat_padding");
        exit(1);
    }
}

ip_mat * create_sharpen_filter(){                    

        ip_mat * filter;                                                //nuova matrice
        float values[] = {0, -1, 0, -1, 5, -1, 0, -1, 0};               //vettore inizializzato con i valori del filtro
        unsigned int ih, iw, ik, dim;                                   //variabili scorrimento cicli
        dim = 0;

        filter = ip_mat_create(3, 3, 3, 0.0);                           //creazione filtro (vuoto)

        for (ih = 0; ih < filter -> h; ih++){                           //inserimento valori nel filtro
            for (iw = 0; iw < filter -> w; iw++){
                for(ik = 0; ik < filter -> k; ik++){
                    filter -> data[ih][iw][ik] = values[dim];
                }
                dim += 1;
            }
        }

        compute_stats(filter);
        return filter;                                                  //output filtro
}

ip_mat * create_edge_filter(){                     

        ip_mat * filter;                                                //nuova matrice
        float values[] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};           //vettore inizializzato con i valori del filtro
        unsigned int ih, iw, ik, dim;                                   //variabili scorrimento cicli
        dim = 0;

        filter = ip_mat_create(3, 3, 3, 0.0);                           //creazione filtro (vuoto)

        for (ih = 0; ih < filter -> h; ih++){                           //inserimento valori nel filtro
            for (iw = 0; iw < filter -> w; iw++){
                for(ik = 0; ik < filter -> k; ik++){
                    filter -> data[ih][iw][ik] = values[dim];
                }
                dim += 1;
            }
        }

        compute_stats(filter);
        return filter;                                                  //output filtro
}

ip_mat * create_emboss_filter(){                    

        ip_mat * filter;                                                //nuova matrice
        float values[] = {-2, -1, 0, -1, 1, 1, 0, 1, 2};                //vettore inizializzato con i valori del filtro
        unsigned int ih, iw, ik, dim;                                   //variabili scorrimento cicli
        dim = 0;

        filter = ip_mat_create(3, 3, 3, 0.0);                           //creazione filtro (vuoto)

        for (ih = 0; ih < filter -> h; ih++){                           //inserimento valori nel filtro
            for (iw = 0; iw < filter -> w; iw++){
                for(ik = 0; ik < filter -> k; ik++){
                    filter -> data[ih][iw][0] = values[dim];
                }
                dim++;
            }
        }

        compute_stats(filter);
        return filter;                                                  //output filtro
}

ip_mat * create_average_filter(unsigned int h, unsigned int w, unsigned int k){

    if(w>0 && h>0 && k>0){
                                                        //matrice w x h; valore su ogni cella è c = 1 / (w*h)
        ip_mat * filter;                                //nuova matrice (ovvero il filtro)
        float c = 1.0 / ((w*1.0) * (h*1.0));                          //calcolo valore medio del filtro

        filter = ip_mat_create(h, w, k, c);             //creazione filtro medio con valore c

        compute_stats(filter);                          //calcolo stats

        return filter;                                  //output filtro
    } else {
        printf("Errore create_average_filter");
        exit(1);
    }
}

/* Crea un filtro gaussiano per la rimozione del rumore */
ip_mat * create_gaussian_filter(unsigned int h, unsigned int w, unsigned int k, float sigma) {
    if(w>0 && h>0 && k>0){
                                                                        //matrice w x h; valore su ogni cella è c = 1 / (w*h)
        ip_mat * filter;                                                //nuova matrice (ovvero il filtro)
        unsigned int ih, iw, ik;
        unsigned int cx, cy;
        cx = (w-1)/2;
        cy= (h-1)/2;
        filter = ip_mat_create(h, w, k, 0.0);                           //creazione filtro medio con valore c
        float val, x, y, sigma2;

        for (ih = 0; ih < filter -> h; ih++){                           //inserimento valori nel filtro
            for (iw = 0; iw < filter -> w; iw++){
                for (ik = 0; ik < filter -> k; ik++){
                        x = abs(iw-cx);
                        y = abs(ih-cy);
                        sigma2 = sigma*sigma*2.0;
                        val = (1.0/(PI*sigma2))*expf(-(((x*x)+(y*y))/sigma2));
                        filter -> data[ih][iw][ik] = val;

                }
            }
        }

        val = 0.0;

        for (ih = 0; ih < filter -> h; ih++){                           //inserimento valori nel filtro
            for (iw = 0; iw < filter -> w; iw++){
                for (ik = 0; ik < filter -> k; ik++){
                        val+=filter -> data[ih][iw][ik];

                }
            }
        }

        for (ih = 0; ih < filter -> h; ih++){                           //inserimento valori nel filtro
            for (iw = 0; iw < filter -> w; iw++){
                for (ik = 0; ik < filter -> k; ik++){

                        filter -> data[ih][iw][ik]/=val;

                }
            }
        }

        compute_stats(filter);                          //calcolo stats
        return filter;                                  //output filtro

    } else {
        printf("Errore create_gaussian_filter");
        exit(1);
    }
}

void rescale(ip_mat * t, float new_max) {
    unsigned int    i;
    unsigned int    j;
    unsigned int    k;
    float           minCanale;
    float           maxCanale;
    float           valoreScalato;
    float           valore;

    compute_stats(t);

    for(k=0; k < t->k; k++) {
        minCanale = t->stat[k].min;
        maxCanale = t->stat[k].max;
        for(i = 0;i < t->h;i++) {
            for(j = 0;j < t->w;j++) {
                valore = t->data[i][j][k];                
                t->data[i][j][k] = (valore-minCanale)/(maxCanale-minCanale);
            }
        }
    }    

    for(i = 0;i < t->h;i++) {
        for(j = 0;j < t->w;j++) {
            for(k = 0;k < t->k;k++) {
                valoreScalato = t->data[i][j][k];
                t->data[i][j][k] = valoreScalato*new_max;
            }
        }
    }

    compute_stats(t);
}

void clamp(ip_mat * t, float low, float high) {

    unsigned int      ih, iw, ik;
    int              edited = 0;

    for(ih = 0; ih < t->h; ih++) {
        for(iw = 0; iw < t->w; iw++) {
            for(ik = 0; ik < t->k; ik++) {

                if(t->data[ih][iw][ik] < low) {
                    t->data[ih][iw][ik] = low;
                    edited = 1;
                }

                if(t->data[ih][iw][ik] > high) {
                    t->data[ih][iw][ik] = high;
                    edited = 1;
                }
            }
        }
    }

    if(edited) {
        compute_stats(t);
    }

    return;
}
