#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include "ip_lib.h"

int main(void)
{
    Bitmap    *caricaFile;
    Bitmap    *fileOutput;    
    char      nomeInput[100];
    char      nomeOutput[100];
    ip_mat    *strutturaImmagine;    ;    
    ip_mat    *scalaGray;

    printf("\n\nScrivi il nome del file .bmp che vuoi caricare: ");
    scanf("%s",nomeInput);
    printf("\n\nNome del file in output? ");
    scanf("%s",nomeOutput);
    caricaFile = bm_load(nomeInput);
    strutturaImmagine = bitmap_to_ip_mat(caricaFile);    
    bm_free(caricaFile);    
    scalaGray = ip_mat_to_gray_scale(strutturaImmagine);
    fileOutput = ip_mat_to_bitmap(scalaGray);    
    bm_save(fileOutput,nomeOutput);    
    bm_free(fileOutput);    
    ip_mat_free(strutturaImmagine);    
    ip_mat_free(scalaGray);
    return 0;
}
