/*
 Created by Sebastiano Vascon on 23/03/20.
*/

#include <stdio.h>
#include <math.h>
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

float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k){
    if(i<a->h && j<a->w &&k<a->k){
        return a->data[i][j][k];
    }else{
        printf("Errore get_val!!!");
        exit(1);
    }
}

void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v){
    if(i<a->h && j<a->w &&k<a->k){
        a->data[i][j][k]=v;        
    }else{
        printf("Errore set_val!!!");
        exit(1);
    }
}

float get_normal_random(float media, float std){

    float y1 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float y2 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float num = cos(2*PI*y2)*sqrt(-2.*log(y1));

    return media + num*std;
}



ip_mat * ip_mat_create(unsigned int h, unsigned int w, unsigned int k, float v) {

    if(h > 0 && w > 0 && k > 0) {                                    
        ip_mat * new_mat = (ip_mat *)malloc(sizeof(ip_mat));            
        float *** new_data;                                             
        unsigned int ih, iw, ik;                                        
        stats * new_stats;                                              


                                                                        
        new_mat->h = h;                                                 
        new_mat->w = w;                                                 
        new_mat->k = k;                                                 

        new_stats = (stats *)malloc(sizeof(stats) * k);                 



        for(ik = 0; ik < k; ik++) {                                     
            new_stats[ik].max = v;                                      
            new_stats[ik].min = v;                                      
            new_stats[ik].mean = v;                                     
        }
                                                                        
        new_data = (float ***)malloc(sizeof(float **) * h);             



        for(ih = 0; ih < h; ih++) {
            new_data[ih] = (float **)malloc(sizeof(float *) * w);       



            for(iw = 0; iw < w; iw++) {
                new_data[ih][iw] = (float *)malloc(sizeof(float) * k);  



                for(ik = 0; ik < k; ik++) {
                    new_data[ih][iw][ik] = v;                           
                }
            }
        }

        new_mat->stat = new_stats;                                      
        new_mat->data = new_data;                                       

        return new_mat;                                                 
    } else {
        printf("Errore ip_mat_create");
        exit(1);
    }
}


void ip_mat_free(ip_mat *a)
{    
    unsigned int  x, y;

    if(a)
    {        
        free(a->stat);        
        for(x = 0;x < a->h;x++)
        {
            for(y = 0;y < a->w;y++)
            {
                free(a->data[x][y]);
            }
            free(a->data[x]);
        }
        free(a->data);        
        free(a);
    }        
}


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


void compute_stats(ip_mat * t)
{
    unsigned int    i;
    

    for(i = 0;i < t->k;i++)
    {
        Aux_calcola_canale(t,i);
    }

}


void ip_mat_init_random(ip_mat * t, float mean, float std)
{
    unsigned int  i, j, z;

    if(t)
    {
        for(i = 0;i < t->h;i++)
        {
            for(j = 0;j < t->w;j++)
            {
                for(z = 0;z < t->k;z++)
                {
                    t->data[i][j][z] = get_normal_random(mean,std);
                }
            }
        }        
        compute_stats(t);
    }
    else
    {
        printf("\nErrore ip_mat_init_random");
        exit(1);
    }
}


ip_mat * ip_mat_copy(ip_mat * in){
    
    if(in) {                                                            
        ip_mat * copia;                                                 
        unsigned int ih, iw, ik;                                        

        copia = ip_mat_create(in->h, in->w, in->k, 0.0);                            

        for(ih = 0; ih < in->h; ih++){
            for(iw = 0; iw < in->w; iw++){
                for(ik = 0; ik < in->k; ik++){
                    copia->data[ih][iw][ik] = in->data[ih][iw][ik];     
                }
            }
        }
        for(ik = 0; ik < in->k; ik++){
            copia->stat[ik].max = in->stat[ik].max;                     
            copia->stat[ik].min = in->stat[ik].min;                     
            copia->stat[ik].mean = in->stat[ik].mean;                   
        }
        return copia;                                                   
    } else {
        printf("Errore ip_mat_copy");
        exit(1);
    }
}


ip_mat *Aux_concat0(ip_mat *a,ip_mat *b)
{
    ip_mat             *result;
    unsigned int       i;
    unsigned int       j;
    unsigned int       terzaDim;    
    unsigned int       iResult = 0;

    if((a->w == b->w)&&(a->k == b->k))
    {
        result = ip_mat_create((a->h + b->h),a->w,a->k,0.0);        
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


ip_mat *Aux_concat1(ip_mat *a,ip_mat *b)
{
    ip_mat                *result;
    unsigned int          i;
    unsigned int          j;
    unsigned int          terzaDim;    
    unsigned int          jResult = 0;

    if((a->h == b->h)&&(a->k == b->k))
    {
        result = ip_mat_create(a->h,(a->w + b->w),a->k,0.0);        
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


ip_mat *Aux_concat2(ip_mat *a,ip_mat *b)
{
    ip_mat                 *result;
    unsigned int           i;
    unsigned int           j;
    unsigned int           terzaDim;    
    unsigned int           terzaResult = 0;

    if((a->h == b->h)&&(a->w == b->w))
    {
        result = ip_mat_create(a->h,a->w,(a->k + b->k),0.0);        
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


ip_mat * ip_mat_concat(ip_mat * a, ip_mat * b, int dimensione)
{
    ip_mat     *result = NULL;

    if(a && b && (dimensione>= 0 && dimensione <= 2))
    {
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
    else
    {
        printf("Errore nell'inserimento dei parametri");
        exit(1);
    }
}



ip_mat * ip_mat_subset(ip_mat * t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end)
{
    ip_mat                   *result;
    unsigned int             i;
    unsigned int             j;
    unsigned int             terzaDim;    
    unsigned int             hResult;
    unsigned int             wResult;    
    unsigned int             iR;
    unsigned int             jR;

    if((row_start < t->h)&&(row_end >= row_start && row_end < t->h))
    {
        if((col_start < t->w)&&(col_end >= col_start && col_end < t->w))
        {
            hResult = (row_end+1)-row_start;
            wResult = (col_end+1)-col_start;
            result = ip_mat_create(hResult,wResult,t->k,0.0);            
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


ip_mat * ip_mat_sum(ip_mat * a, ip_mat * b){

    if(a && b && (a->h == b->h) && (a->w == b->w) && (a->k == b->k)) {
        ip_mat * somma;                                                                     
        unsigned int ih, iw, ik;                                                            

        somma = ip_mat_create(a->h, a->w, a->k, 0.0);                                       

        for(ih = 0; ih < somma->h; ih++){
            for(iw = 0; iw < somma->w; iw++){
                for(ik = 0; ik < somma->k; ik++){
                    somma->data[ih][iw][ik] = a->data[ih][iw][ik] + b->data[ih][iw][ik];    
                }
            }
        }
        compute_stats(somma);                                                               
        return somma;                                                                       
    } else {
        printf("Errore ip_mat_sum");
        exit(1);
    }
}


ip_mat * ip_mat_sub(ip_mat * a, ip_mat * b){

    if(a && b && (a->h == b->h) && (a->w == b->w) && (a->k == b->k)) {
        ip_mat * sottrazione;                                                                   
        unsigned int ih, iw, ik;                                                                

        sottrazione = ip_mat_create(a->h, a->w, a->k, 0.0);                                     

        for(ih = 0; ih < sottrazione->h; ih++){
            for(iw = 0; iw < sottrazione->w; iw++){
                for(ik = 0; ik < sottrazione->k; ik++){
                    sottrazione->data[ih][iw][ik] = a->data[ih][iw][ik] - b->data[ih][iw][ik];  
                }
            }
        }
        compute_stats(sottrazione);                                                             
        return sottrazione;                                                                     
    } else {
        printf("Errore ip_mat_sub");
        exit(1);
    }
}



ip_mat * ip_mat_mul_scalar(ip_mat *a, float c){

    if(a) {
        ip_mat * moltiplicaScalare;                                                 
        unsigned int ih, iw, ik;                                                    

        moltiplicaScalare = ip_mat_create(a->h, a->w, a->k, 0.0);                   

        for(ih = 0; ih < moltiplicaScalare->h; ih++){
            for(iw = 0; iw < moltiplicaScalare->w; iw++){
                for(ik = 0; ik < moltiplicaScalare->k; ik++){
                    moltiplicaScalare->data[ih][iw][ik] = a->data[ih][iw][ik] * c;  
                }                                                                   
            }
        }
        compute_stats(moltiplicaScalare);                                           
        return moltiplicaScalare;                                                   
    } else {
        printf("Errore ip_mat_mul_scalar");
        exit(1);
    }
}


ip_mat *  ip_mat_add_scalar(ip_mat *a, float c){

    if(a) {
        ip_mat * sommaScalare;                                                  
        unsigned int ih, iw, ik;                                                

        sommaScalare = ip_mat_create(a->h, a->w, a->k, 0.0);                    

        for(ih = 0; ih < sommaScalare->h; ih++){
            for(iw = 0; iw < sommaScalare->w; iw++){
                for(ik = 0; ik < sommaScalare->k; ik++){
                    sommaScalare->data[ih][iw][ik] = a->data[ih][iw][ik] + c;   
                }                                                               
            }
        }
        compute_stats(sommaScalare);                                            
        return sommaScalare;                                                    
    } else {
        printf("Errore ip_mat_add_scalar");
        exit(1);
    }
}


ip_mat * ip_mat_mean(ip_mat * a, ip_mat * b)
{
    unsigned int      i;
    unsigned int      j;
    unsigned int      terzaDim;
    ip_mat            *result;


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
        compute_stats(result);
        return result;
    }
    else
    {
        exit(1);
    }
    
}


ip_mat * ip_mat_brighten(ip_mat * a, float bright)
{
    unsigned int        i;
    unsigned int        j;
    unsigned int        terzaDim;
    ip_mat              *result;

    if(a)
    {
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
    else
    {
        printf("Errore in input");
        exit(1);
    }
}


ip_mat * ip_mat_to_gray_scale(ip_mat * in) {
    if(in) {
        ip_mat * scalaGrigi;                                                                    
        unsigned int ih, iw, ik;                                                                
        float media = 0.0;                                                                      
        float nCanali = in->k;                                                                   

        scalaGrigi = ip_mat_create(in->h, in->w, in->k, 0.0);                                   

        for(ih = 0; ih < scalaGrigi->h; ih++){
            for(iw = 0; iw < scalaGrigi->w; iw++){
                for(ik = 0; ik < scalaGrigi->k; ik++){
                    media += in->data[ih][iw][ik];  
                }
                media /= nCanali;
                for(ik = 0; ik < scalaGrigi->k; ik++){
                   scalaGrigi->data[ih][iw][ik] = media;  
                }
                media = 0.0;
            }                                                                                   
        }
        compute_stats(scalaGrigi);                                                                   
        return scalaGrigi;                                                                           
    } else {
        printf("Errore ip_mat_to_gray_scale");
        exit(1);
    }
}



ip_mat * ip_mat_corrupt(ip_mat * a, float amount)
{
    unsigned int        i;
    unsigned int        j;
    unsigned int        terzaDim;
    ip_mat              *result;

    if(a)
    {
        result = ip_mat_create(a->h,a->w,a->k,0.0);
        if(result)
        {
            for(i = 0;i < a->h;i++)
            {
                for(j = 0;j < a->w;j++)
                {
                    for(terzaDim = 0;terzaDim < a->k;terzaDim++)
                    {                    
                        result->data[i][j][terzaDim] = get_val(a,i,j,terzaDim) + get_normal_random(0,(amount/2.0));                    
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
    else
    {
        printf("Errore in input");
        exit(1);
    }
}


void clamp(ip_mat * t, float low, float high)
{
    unsigned int      i;
    unsigned int      j;
    unsigned int      terzaDim;    
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


ip_mat * ip_mat_blend(ip_mat * a, ip_mat * b, float alpha){
    if((a && b)&&(a->h == b->h && a->w == b->w && a->k == b->k)&&((alpha >= 0.0 && alpha <= 1.0))) {
        ip_mat * bleded;                                                                         
        unsigned int ih, iw, ik;                                                                

        bleded = ip_mat_create(a->h, a->w, a->k, 0.0);                                           

        for(ih = 0; ih < bleded->h; ih++){
            for(iw = 0; iw < bleded->w; iw++){
                for(ik = 0; ik < bleded->k; ik++){
                    bleded->data[ih][iw][ik] = alpha*(a->data[ih][iw][ik]) + (1 - alpha)*(b->data[ih][iw][ik]);  
                }                                                                               
            }                                                                                   
        }
        compute_stats(bleded);                                                                   
        return bleded;                                                                           
    } else {
        printf("Errore ip_mat_blend");
        exit(1);
    }
}


ip_mat * ip_mat_padding(ip_mat * a, unsigned int pad_h, unsigned int pad_w){  
    if(a){
        ip_mat * padded;                                       
        unsigned int ih, iw, ik;                               
        unsigned int ph, pw, pk;                               

        ph = a->h + 2*pad_h;                                   
        pw = a->w + 2*pad_w;
        pk = a->k;
        padded = ip_mat_create(ph, pw, pk, 0.0);                       
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


float Aux_valore_cella(ip_mat *submatrix,ip_mat *filter,unsigned int canale)
{
    unsigned int     i;
    unsigned int     j;    
    float            valoreDiRitorno = 0;

    for(i = 0;i < submatrix->h;i++)
    {
        for(j = 0;j < submatrix->w;j++)
        {
            valoreDiRitorno += get_val(submatrix,i,j,canale)*get_val(filter,i,j,canale);
        }
    }
    return valoreDiRitorno;
}


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

    if(a)
    {
        if(a->k == f->k)
        {
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
        else
        {
            printf("\nDimensione di k diversa fra le due ip_mat");
            exit(1);
        }
    }
    else
    {
        printf("Errore in input");
        exit(1);
    }
}



ip_mat * create_sharpen_filter(){                    

        ip_mat * filter;                                                
        float values[] = {0, -1, 0, -1, 5, -1, 0, -1, 0};               
        unsigned int ih, iw, ik, dim;                                   
        dim = 0;

        filter = ip_mat_create(3, 3, 3, 0.0);                           
        for (ih = 0; ih < filter -> h; ih++){                           
            for (iw = 0; iw < filter -> w; iw++){
                for(ik = 0; ik < filter -> k; ik++){
                    filter -> data[ih][iw][ik] = values[dim];
                }
                dim += 1;
            }
        }
        compute_stats(filter);
        return filter;                                                  
}



ip_mat * create_edge_filter(){                     

        ip_mat * filter;                                                
        float values[] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};           
        unsigned int ih, iw, ik, dim;                                   
        dim = 0;

        filter = ip_mat_create(3, 3, 3, 0.0);                           
        for (ih = 0; ih < filter -> h; ih++){                           
            for (iw = 0; iw < filter -> w; iw++){
                for(ik = 0; ik < filter -> k; ik++){
                    filter -> data[ih][iw][ik] = values[dim];
                }
                dim += 1;
            }
        }
        compute_stats(filter);
        return filter;                                                  
}


ip_mat * create_emboss_filter()
{
    ip_mat   *result;
    int      k = 3;
    int      i;

    result = ip_mat_create(3,3,k,0.0);
    for(i = 0;i < k;i++)
    {
        set_val(result,0,0,i,-2);
        set_val(result,0,1,i,-1);
        set_val(result,0,2,i,0);
        set_val(result,1,0,i,-1);
        set_val(result,1,1,i,1);
        set_val(result,1,2,i,1);
        set_val(result,2,0,i,0);
        set_val(result,2,1,i,1);
        set_val(result,2,2,i,2);
    }
    compute_stats(result);
    return result;
}


ip_mat * create_average_filter(unsigned int h, unsigned int w, unsigned int k){

    if(w>0 && h>0 && k>0){                                                        
        ip_mat * filter;                                
        float c = 1.0 / ((w*1.0) * (h*1.0));
        
        filter = ip_mat_create(h, w, k, c);             
        compute_stats(filter);                          
        return filter;                                  
    } else {
        printf("Errore create_average_filter");
        exit(1);
    }
}


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
    

    if(w > 0 && h > 0 && k > 0)
    {
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
            for(i = 0;i < h;i++)
            {
                for(j = 0;j < w;j++)
                {
                    for(terzaDim = 0;terzaDim < 1;terzaDim++)
                    {
                        sommaFiltro += result->data[i][j][terzaDim];                    
                    }
                }
            }                
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
    else
    {
        printf("Errore in input");
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













