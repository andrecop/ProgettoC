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
ip_mat * ip_mat_create(unsigned int h, unsigned int w,unsigned  int k, float v){
    ip_mat * new_mat = (ip_mat*)malloc(sizeof(ip_mat));
    stats new_stats[k];
    float ***new_data;
    unsigned int ih, iw, ik;

    // inizializzo le varibili della matrice
    new_mat->h = h;     // altezza
    new_mat->w = w;     // lunghezza
    new_mat->k = k;     // n° canali

    // inizializzo stats (anche se non credo serva, qui verrà chiamata la funzione che lo fa)
    for(ik = 0; ik < k; ik++){
        new_stats[ik].max = v;
        new_stats[ik].min = v;
        new_stats[ik].mean = v;
    }

    new_data = (float ***)malloc(sizeof(float **) * h);             // creo matrice altezza
    for(ih = 0; ih < h; ih++){
        new_data[ih] = (float **)malloc(sizeof(float *) * w);       // aggiungo dimensione lunghezza
        for(iw = 0; iw < w; iw++){
            new_data[ih][iw] = (float *)malloc(sizeof(float) * k);  // aggiungo i canali
            for(ik = 0; ik < k; ik++){
                new_data[ih][iw][ik] = v;                           // setto il valore di default
            }
        }
    }

    new_mat->stat = new_stats;                                      // setto stat
    new_mat->data = new_data;                                       // e data

    return new_mat;                                                 // ritorno la matrice inizializzata
}

/* Libera la memoria (data, stat e la struttura) */
void ip_mat_free(ip_mat *a){
    //evitare di istanziare variabili inutili
    int ih, iw;                     // variabili per scorrere
    int h = a->h;                   // recupero l'altezza
    int w = a->w;                   // recupero la lunghezza
    int k = a->k;                   // recupero il numero di canali

    free(a->stat);                  // libero stat

    for(ih = 0; ih < h; ih++){
        for(iw = 0; iw < w; iw++){
            free(a->data[ih][iw]);  // libero i canali per ciascuna lunghezza
        }
        free(a->data[ih]);          // libero la lunghezza per ciascuna altezza
    }

    free(a->data);                  // libero l'altezza
    free(a);                        // libero la matrice che conteneva tutto

    return;                         // fine
}

/* Restituisce il valore in posizione i,j,k */
float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k){
    return a->data[i][j][k];
}

/* Setta il valore in posizione i,j,k a v*/
void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v){
    a->data[i][j][k] = v;
    return;
}

/* Calcola il valore minimo, il massimo e la media per ogni canale
 * e li salva dentro la struttura ip_mat stats
 * */
void compute_stats(ip_mat * t){
    //evitare di istanziare variabili inutili
    int ih, iw, ik;                         // variabili per scorrere
    float val;                              // variabile per salvare il valore in "data"
    int h = t->h;                           // recupero l'altezza
    int w = t->w;                           // recupero la lunghezza
    int k = t->k;                           // recupero il numero di canali
    float minimo = FLT_MAX;                 // setto il minimo al max float
    float massimo = FLT_MIN;                // setto in massimo al min float
    float media;                            // qui calcolerò la media

    for(ih = 0; ih < h; ih++){
        for(iw = 0; iw < w; iw++){
            for(ik = 0; ik < k; ik++){
                val = t->data[ih][iw][ik];  // estraggo il valore
                if(val < minimo)
                    minimo = val;           // salvo il minimo (di sicuro sarà più piccolo di max float)
                if(val > massimo)
                    massimo = val;          // salvo il massimo (il contrario di sopra ↑)
            }
        }
    }

    media = (massimo + minimo) / 2;         // calcolo la media
    t->stat->max = massimo;                 // setto il massimo
    t->stat->min = minimo;                  // il minimo
    t->stat->mean = media;                  // e la media

    return;                                 // fine
}

/*----------------------------------------------------------------------------------------------
                                       ↓ DA FARE ↓
-----------------------------------------------------------------------------------------------*/


/* Inizializza una ip_mat con dimensioni w h e k.
 * Ogni elemento è generato da una gaussiana con media mean e varianza var */
void ip_mat_init_random(ip_mat * t, float mean, float var){
    //evitare di istanziare variabili inutili
    unsigned int h, w, k;                                               // variabili per salvare dimensioni matrice 3D
    unsigned int ih, iw, ik;                                            // variabili per scorrere
    h = t->h;                                                           // recupero l'altezza
    w = t->w;                                                           // recupero la lunghezza
    k = t->k;                                                           // recupero il numero di canali

    for(ih = 0; ih < h; ih++){
        for(iw = 0; iw < w; iw++){
            for(ik = 0; ik < k; ik++){
                t->data[ih][iw][ik] = get_normal_random() * var + mean; // creo i valori
            }
        }
    }

    compute_stats(t);
    return;
}


/* Crea una copia di una ip_mat e lo restituisce in output */
ip_mat * ip_mat_copy(ip_mat * in){
    //evitare di istanziare variabili inutili
    ip_mat * copia;                                             // nuova matrice (copia)
    unsigned int h, w, k;                                       // variabili per salvare dimensioni matrice 3D
    unsigned int ih, iw, ik;                                    // variabili per scorrere
    h = in->h;                                                  // recupero l'altezza
    w = in->w;                                                  // recupero la lunghezza
    k = in->k;                                                  // recupero il numero di canali

    copia = ip_mat_create(h, w, k, 0.0);                        // creo una nuova ip_mat

    for(ih = 0; ih < h; ih++){
        for(iw = 0; iw < w; iw++){
            for(ik = 0; ik < k; ik++){
                copia->data[ih][iw][ik] = in->data[ih][iw][ik]; // copio i valori di data
            }
        }
    }
    for(ik = 0; ik < k; ik++){
        copia->stat[ik].max = in->stat[ik].max;                 // copio in stat il massimo
        copia->stat[ik].min = in->stat[ik].min;                 // il minimo
        copia->stat[ik].mean = in->stat[ik].mean;               // e la media (tanto son gli stessi)
    }
    return copia;                                               // fine
}

/* Restituisce una sotto-matrice, ovvero la porzione individuata da:
 * t->data[row_start...row_end][col_start...col_end][0...k]
 * La terza dimensione la riportiamo per intero, stiamo in sostanza prendendo un sottoinsieme
 * delle righe e delle colonne.
 * */
ip_mat * ip_mat_subset(ip_mat * t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end){
    //manca verifica indici (range)
    //snellire evitando di istanziare variabili (che sarebbe inutile)

    ip_mat * sub;                                             // nuova matrice (copia)

    /*
    float *** data;
    float ** dataH;
    float * dataW;
    */

    unsigned int h, w, k;                        // variabili per dimensioni matrice 3D
    unsigned int ih, iw, ik;                                    // variabili per scorrere
    h = row_end;                                                  // recupero l'altezza
    w = col_end;                                                  // recupero la lunghezza
    k = t->k;                                                  // recupero il numero di canali
    
    sub = ip_mat_create(h - row_start, w - col_start, k, 0.0);                        // creo una nuova ip_mat
    
    for(ih = row_start; ih < h; ih++){
        for(iw = col_start; iw < w; iw++){
            for(ik = 0; ik < k; ik++){
                sub->data[ih-row_start][iw-col_start][ik] = t->data[ih][iw][ik]; // copio i valori di data
            }
        }
    }

    compute_stats(sub);
    
    /*
    QUANTO SCRITTO SOTTO E' SOLO UN'IDEA PER NON CREARE UNA NUOVA MATRICE, DA TESTARE!

    sub->h=row_end-row_start;
    sub->w=col_end-col_start;
    sub->k=t->k;
    
    dataW=&(t->data[row_start][col_start]);
    dataH=&(dataW[row_start]);
    data=&(dataH);

    sub->data=data;

    compute_stats(sub);
    */

    return sub;
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
    // mancano controlli dimensioni
    //evitare di istanziare variabili inutili
    //migliorare il codice (abbozzato)
    //soprattutto la parte degli if e dei for
    ip_mat * concat;                                             // nuova matrice (copia)
    unsigned int h, w, k, ha, wa, ka, hb, wb, kb;                                       // variabili per salvare dimensioni matrice 3D
    unsigned int ih, iw, ik;                                    // variabili per scorrere
    ha = a->h;                                                  // recupero l'altezza
    wa = a->w;                                                  // recupero la lunghezza
    ka = a->k;                                                  // recupero il numero di canali
    hb = b->h;                                                  // recupero l'altezza
    wb = b->w;                                                  // recupero la lunghezza
    kb = b->k;                                                  // recupero il numero di canali

    h=ha;
    w=wa;
    k=ka;
    
    if(dimensione == 0){
        h = ha + hb;
    } else if(dimensione == 1){
        w = wa + wb;
    } else if(dimensione == 2){
        k = ka + kb;
    }

    concat = ip_mat_create(h, w, k, 0.0);                        // creo una nuova ip_mat

    if(dimensione == 0){
        for(ih = 0; ih < ha; ih++){
            for(iw = 0; iw < w; iw++){
                for(ik = 0; ik < k; ik++){
                    concat->data[ih][iw][ik] = a->data[ih][iw][ik]; // copio i valori di data
                }
            }
        }
        for(ih = ha; ih < h; ih++){
            for(iw = 0; iw < w; iw++){
                for(ik = 0; ik < k; ik++){
                    concat->data[ih][iw][ik] = a->data[ih-ha][iw][ik]; // copio i valori di data
                }
            }
        }
    } else if(dimensione == 1){
        for(ih = 0; ih < h; ih++){
            for(iw = 0; iw < wa; iw++){
                for(ik = 0; ik < k; ik++){
                    concat->data[ih][iw][ik] = a->data[ih][iw][ik]; // copio i valori di data
                }
            }
            for(iw = wa; iw < w; iw++){
                for(ik = 0; ik < k; ik++){
                    concat->data[ih][iw][ik] = a->data[ih][iw-wa][ik]; // copio i valori di data
                }
            }
        }
    } else if(dimensione == 2){
        for(ih = 0; ih < h; ih++){
            for(iw = 0; iw < w; iw++){
                for(ik = 0; ik < ka; ik++){
                    concat->data[ih][iw][ik] = a->data[ih][iw][ik]; // copio i valori di data
                }
                for(ik = ka; ik < k; ik++){
                    concat->data[ih][iw][ik] = a->data[ih][iw][ik-ka]; // copio i valori di data
                }
            }
        }
    }
    compute_stats(concat);

    return concat;                                               // fine
}

/**** PARTE 1: OPERAZIONI MATEMATICHE FRA IP_MAT ****/
/* Esegue la somma di due ip_mat (tutte le dimensioni devono essere identiche)
 * e la restituisce in output. */
ip_mat * ip_mat_sum(ip_mat * a, ip_mat * b){
    //controllo dimensioni identiche in ingresso
    //evitare di istanziare variabili inutili
    ip_mat * somma;                                             // nuova matrice (copia)
    unsigned int h, w, k;                                       // variabili per salvare dimensioni matrice 3D
    unsigned int ih, iw, ik;                                    // variabili per scorrere
    h = a->h;                                                  // recupero l'altezza
    w = a->w;                                                  // recupero la lunghezza
    k = a->k;                                                  // recupero il numero di canali

    somma = ip_mat_create(h, w, k, 0.0);                        // creo una nuova ip_mat

    for(ih = 0; ih < h; ih++){
        for(iw = 0; iw < w; iw++){
            for(ik = 0; ik < k; ik++){
                somma->data[ih][iw][ik] = a->data[ih][iw][ik] + b->data[ih][iw][ik]; // copio i valori di data
            }
        }
    }
    compute_stats(somma);
    return somma;                                               // fine
}

/* Esegue la sottrazione di due ip_mat (tutte le dimensioni devono essere identiche)
 * e la restituisce in output.
 * */
ip_mat * ip_mat_sub(ip_mat * a, ip_mat * b){
    //controllo dimensioni identiche in ingresso
    //evitare di istanziare variabili inutili
    ip_mat * sottrazione;                                             // nuova matrice (copia)
    unsigned int h, w, k;                                       // variabili per salvare dimensioni matrice 3D
    unsigned int ih, iw, ik;                                    // variabili per scorrere
    h = a->h;                                                  // recupero l'altezza
    w = a->w;                                                  // recupero la lunghezza
    k = a->k;                                                  // recupero il numero di canali

    sottrazione = ip_mat_create(h, w, k, 0.0);                        // creo una nuova ip_mat

    for(ih = 0; ih < h; ih++){
        for(iw = 0; iw < w; iw++){
            for(ik = 0; ik < k; ik++){
                sottrazione->data[ih][iw][ik] = a->data[ih][iw][ik] - b->data[ih][iw][ik]; // copio i valori di data
            }
        }
    }
    compute_stats(sottrazione);
    return sottrazione;   
}

/* Moltiplica un ip_mat per uno scalare c. Si moltiplica c per tutti gli elementi di "a"
 * e si salva il risultato in un nuovo tensore in output. */
ip_mat * ip_mat_mul_scalar(ip_mat *a, float c){
    //controllo dimensioni identiche in ingresso
    //evitare di istanziare variabili inutili
    ip_mat * moltiplicaScalare;                                             // nuova matrice (copia)
    unsigned int h, w, k;                                       // variabili per salvare dimensioni matrice 3D
    unsigned int ih, iw, ik;                                    // variabili per scorrere
    h = a->h;                                                  // recupero l'altezza
    w = a->w;                                                  // recupero la lunghezza
    k = a->k;                                                  // recupero il numero di canali

    moltiplicaScalare = ip_mat_create(h, w, k, 0.0);                        // creo una nuova ip_mat

    for(ih = 0; ih < h; ih++){
        for(iw = 0; iw < w; iw++){
            for(ik = 0; ik < k; ik++){
                moltiplicaScalare->data[ih][iw][ik] = a->data[ih][iw][ik] * c; // copio i valori di data
            }
        }
    }
    compute_stats(moltiplicaScalare);
    return moltiplicaScalare;   
}

/* Aggiunge ad un ip_mat uno scalare c e lo restituisce in un nuovo tensore in output. */
ip_mat *  ip_mat_add_scalar(ip_mat *a, float c){
    //controllo dimensioni identiche in ingresso
    //evitare di istanziare variabili inutili
    ip_mat * sommaScalare;                                             // nuova matrice (copia)
    unsigned int h, w, k;                                       // variabili per salvare dimensioni matrice 3D
    unsigned int ih, iw, ik;                                    // variabili per scorrere
    h = a->h;                                                  // recupero l'altezza
    w = a->w;                                                  // recupero la lunghezza
    k = a->k;                                                  // recupero il numero di canali

    sommaScalare = ip_mat_create(h, w, k, 0.0);                        // creo una nuova ip_mat

    for(ih = 0; ih < h; ih++){
        for(iw = 0; iw < w; iw++){
            for(ik = 0; ik < k; ik++){
                sommaScalare->data[ih][iw][ik] = a->data[ih][iw][ik] + c; // copio i valori di data
            }
        }
    }
    compute_stats(sommaScalare);
    return sommaScalare;   
}

/* Calcola la media di due ip_mat a e b e la restituisce in output.*/
ip_mat * ip_mat_mean(ip_mat * a, ip_mat * b){
    //controllo dimensioni identiche in ingresso
    //evitare di istanziare variabili inutili
    ip_mat * media;                                             // nuova matrice (copia)
    unsigned int h, w, k;                                       // variabili per salvare dimensioni matrice 3D
    unsigned int ih, iw, ik;                                    // variabili per scorrere
    h = a->h;                                                  // recupero l'altezza
    w = a->w;                                                  // recupero la lunghezza
    k = a->k;                                                  // recupero il numero di canali

    media = ip_mat_create(h, w, k, 0.0);                        // creo una nuova ip_mat

    for(ih = 0; ih < h; ih++){
        for(iw = 0; iw < w; iw++){
            for(ik = 0; ik < k; ik++){
                media->data[ih][iw][ik] = (a->data[ih][iw][ik] + b->data[ih][iw][ik]) / 2; // copio i valori di data
            }
        }
    }
    compute_stats(media);
    return media;   
}

/**** PARTE 2: SEMPLICI OPERAZIONI SU IMMAGINI ****/
/* Converte un'immagine RGB ad una immagine a scala di grigio.
 * Quest'operazione viene fatta calcolando la media per ogni pixel sui 3 canali
 * e creando una nuova immagine avente per valore di un pixel su ogni canale la media appena calcolata.
 * Avremo quindi che tutti i canali saranno uguali.
 * */
ip_mat * ip_mat_to_gray_scale(ip_mat * in);

/* Effettua la fusione (combinazione convessa) di due immagini */
ip_mat * ip_mat_blend(ip_mat * a, ip_mat * b, float alpha);

/* Operazione di brightening: aumenta la luminosità dell'immagine
 * aggiunge ad ogni pixel un certo valore*/
ip_mat * ip_mat_brighten(ip_mat * a, float bright);

/* Operazione di corruzione con rumore gaussiano:
 * Aggiunge del rumore gaussiano all'immagine, il rumore viene enfatizzato
 * per mezzo della variabile amount.
 * out = a + gauss_noise*amount
 * */
ip_mat * ip_mat_corrupt(ip_mat * a, float amount);

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