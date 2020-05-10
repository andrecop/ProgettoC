#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include "ip_lib.h"

/*
1) bm_load(char *input);
2) bm_free(Bitmap *pointer);
3) bitmap_to_ip_mat(Bitmap *pointer)
4) clamp(ip_mat *pointer,0,255)
5) ip_mat_to_bitmap(ip_mat *pointer);
6) bm_save(Bitmap *pointer,char *nomeFileOutput);*/


int main(void)
{
    Bitmap   *foto1;
    Bitmap   *foto2;
    Bitmap   *output;
    char     nomeInput1[100];
    char     nomeInput2[100];
    ip_mat   *struttura1;
    ip_mat   *struttura2;
    ip_mat   *risultante;

    printf("\nNome prima .bmp da importare: ");
    scanf("%s",nomeInput1);
    foto1 = bm_load(nomeInput1);
    printf("\nNome seconda .bmp da importare: ");
    scanf("%s",nomeInput2);
    foto2 = bm_load(nomeInput2);
    struttura1 = bitmap_to_ip_mat(foto1);
    bm_free(foto1);
    struttura2 = bitmap_to_ip_mat(foto2);
    bm_free(foto2);
    risultante = ip_mat_blend(struttura1,struttura2,0.75);
    clamp(risultante,0,255);
    output = ip_mat_to_bitmap(risultante);
    bm_save(output,"blending");
    bm_free(output);
    ip_mat_free(struttura1);
    ip_mat_free(struttura2);
    ip_mat_free(risultante);
    return 0;
    
}


