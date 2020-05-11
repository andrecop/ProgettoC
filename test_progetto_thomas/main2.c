#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bmp.h"
#include "ip_lib.h"


    Bitmap   *foto1;
    Bitmap   *foto2;
    Bitmap   *output;
    char     bitmap1[100];
    char     bitmap2[100];
    char     formato[]=".bmp";
    char     operazione[100];
    char     risp[1];
    ip_mat   *mat1 = NULL;
    ip_mat   *mat2 = NULL;
    ip_mat   *out;
    unsigned int h, w, k, i;
    int m;
    float v, mean, var;

void selection(void){
    printf("\033[1;31m");
    printf("\nAttenzione! Digitare ");
    printf("\033[0m");
    printf("help ");
    printf("\033[1;31m");
    printf("per la lista delle operazioni.");

    printf("\033[1;33m");
    printf("\n\nNome operazione da eseguire: ");
    printf("\033[0m");
    scanf("%s",operazione);

    if (strcmp(operazione, "help") == 0){
        printf("\033[1;33m");
        printf("\ncreate\nfree\nshow\nshow_stats\nget\nset\ncompute_stats\ninit_random\ncopy\nsubset\nconcat\nsum");
        printf("\nsub\nmul_scalar\nadd_scalar\nmean\nto_gray_scale\nblend\nbrighten\ncorrupt\n");
        printf("\033[0m");
    } else if (strcmp(operazione, "create") == 0){
        printf("\033[1;31m");
        printf("\nDimensioni");
        printf("\nAltezza: ");
        printf("\033[0m");
        scanf("%d",&h);
        printf("\033[1;31m");
        printf("Larghezza: ");
        printf("\033[0m");
        scanf("%d",&w);
        printf("\033[1;31m");
        printf("Canali (di default 3): ");
        printf("\033[0m");
        scanf("%d",&k);
        printf("\033[1;31m");
        printf("Valore di inizializzazione: ");
        printf("\033[0m");
        scanf("%f",&v);
        mat1 = ip_mat_create(h,w,k,v);
        printf("\nMatrice creata correttamente\n");
    } else if (strcmp(operazione, "free") == 0){
        printf("\033[1;31m");
        printf("\nLibero la memoria ");
        for(i=0; i< 4; i++){
            fflush(stdout);
            sleep(1);
            printf(".");
            fflush(stdout);
        }
        if(mat1){
            ip_mat_free(mat1);
        }
        if(mat2){
            ip_mat_free(mat2);
        }
        printf("\nMemoria liberata correttamente\n");
        printf("\033[0m");
    } else if (strcmp(operazione, "show") == 0){
        if(mat1){
            ip_mat_show(mat1);
        }
        printf("\n");
    } else if (strcmp(operazione, "show_stats") == 0){
        if(mat1){
            ip_mat_show_stats(mat1);
        }
        printf("\n");
    } else if (strcmp(operazione, "get") == 0){
        if(mat1){
            printf("\033[1;31m");
            printf("\nPosizione");
            printf("\nAltezza: ");
            printf("\033[0m");
            scanf("%d",&h);
            printf("\033[1;31m");
            printf("Larghezza: ");
            printf("\033[0m");
            scanf("%d",&w);
            printf("\033[1;31m");
            printf("Canale: ");
            printf("\033[0m");
            scanf("%d",&k);
            v = get_val(mat1,h,w,k);
            printf("%f", v);
            printf("\n");
        }
    } else if (strcmp(operazione, "set") == 0){
        if(mat1){
            printf("\033[1;31m");
            printf("\nPosizione");
            printf("\nAltezza: ");
            printf("\033[0m");
            scanf("%d",&h);
            printf("\033[1;31m");
            printf("Larghezza: ");
            printf("\033[0m");
            scanf("%d",&w);
            printf("\033[1;31m");
            printf("Canale: ");
            printf("\033[0m");
            scanf("%d",&k);
            printf("\033[1;31m");
            printf("Valore: ");
            printf("\033[0m");
            scanf("%e",&v);
            set_val(mat1,h,w,k,v);
            printf("\nValore settato correttamente\n");
        }
    } else if (strcmp(operazione, "compute_stats") == 0){
        if(mat1){
            compute_stats(mat1);
            printf("\nStatistiche computate correttamente\n");
        }
    } else if (strcmp(operazione, "init_random") == 0){
        if(mat1){
            printf("\033[1;31m");
            printf("\nParametri");
            printf("\nMedia: ");
            printf("\033[0m");
            scanf("%f",&mean);
            printf("\033[1;31m");
            printf("Varianza: ");
            printf("\033[0m");
            scanf("%f",&var);
            ip_mat_init_random(mat1, mean, var);
            printf("\nMatrice inizializzata correttamente\n");
        }
    } else if (strcmp(operazione, "copy") == 0){
        if(mat1){
            mat2 = ip_mat_copy(mat1);
            printf("\nMatrice copiata correttamente\n");
        }
    } else if (strcmp(operazione, "subset") == 0){
        if(mat1){
            printf("\033[1;31m");
            printf("\nPorzione");
            printf("\nInizio riga: ");
            printf("\033[0m");
            scanf("%d",&h);
            printf("\033[1;31m");
            printf("Fine riga: ");
            printf("\033[0m");
            scanf("%d",&w);
            printf("\033[1;31m");
            printf("Inizio colonna: ");
            printf("\033[0m");
            scanf("%d",&k);
            printf("\033[1;31m");
            printf("Fine colonna: ");
            printf("\033[0m");
            scanf("%d",&i);
            mat1 = ip_mat_subset(mat1,h,w,k,i);
            printf("\nMatrice tagliata correttamente\n");
        }
    } else if (strcmp(operazione, "concat") == 0){
        if(mat1){
            printf("\033[1;31m");
            printf("\nDimensione su cui effettuare la concatenazione (0,1,2): ");
            printf("\033[0m");
            scanf("%d",&m);
            mat1 = ip_mat_concat(mat1,mat2,m);
            printf("\nMatrici concatenate correttamente\n");
        }
    } else if (strcmp(operazione, "sum") == 0){
        if(mat1){
            mat1 = ip_mat_sum(mat1,mat2);
            printf("\nMatrici sommate correttamente\n");
        }
    } else if (strcmp(operazione, "sub") == 0){
        if(mat1){
            mat1 = ip_mat_sub(mat1,mat2);
            printf("\nMatrici sottratte correttamente\n");
        }
    } else if (strcmp(operazione, "mul_scalar") == 0){
        if(mat1){
            printf("\033[1;31m");
            printf("\nScalare da moltiplicare: ");
            printf("\033[0m");
            scanf("%e",&v);
            mat1 = ip_mat_mul_scalar(mat1,v);
            printf("\nScalare moltiplicato correttamente\n");
        }
    } else if (strcmp(operazione, "add_scalar") == 0){
        if(mat1){
            printf("\033[1;31m");
            printf("\nScalare da sommare: ");
            printf("\033[0m");
            scanf("%e",&v);
            mat1 = ip_mat_add_scalar(mat1,v);
            printf("\nScalare sommato correttamente\n");
        }
    } else if (strcmp(operazione, "mean") == 0){
        if(mat1){
            mat1 = ip_mat_mean(mat1,mat2);
            printf("\nMedia calcolata correttamente\n");
        }
    } else if (strcmp(operazione, "to_gray_scale") == 0){
        if(mat1){
            mat1 = ip_mat_to_gray_scale(mat1);
            printf("\nImmagine in scala di grigi creata correttamente\n");
        }
    } else if (strcmp(operazione, "blend") == 0){
        if(mat1){
            printf("\033[1;31m");
            printf("\nParametro alpha: ");
            printf("\033[0m");
            scanf("%e",&v);
            mat1 = ip_mat_blend(mat1, mat2, v);
            printf("\nImmagini mescolate correttamente\n");
        }
    } else if (strcmp(operazione, "brighten") == 0){
        if(mat1){
            printf("\033[1;31m");
            printf("\nLuminosità: ");
            printf("\033[0m");
            scanf("%e",&v);
            mat1 = ip_mat_brighten(mat1, v);
            printf("\nImmagine schiarita correttamente\n");
        }
    } else if (strcmp(operazione, "corrupt") == 0){
        if(mat1){
            printf("\033[1;31m");
            printf("\nQuantità: ");
            printf("\033[0m");
            scanf("%e",&v);
            mat1 = ip_mat_corrupt(mat1, v);
            printf("\nImmagine corrotta correttamente\n");
        }
    }

    printf("\033[1;31m");
    printf("\nVuoi eseguire altre operazioni? y/n\n");
    printf("\033[0m");
    scanf("%s",risp);
    if (strcmp(risp, "y") == 0){
        selection();
    }
}
int main(void)
{
    

    printf("\033[1;31m");
    printf("\nVuoi caricare delle immagini? y/n\n");
    printf("\033[0m");
    scanf("%s",risp);
    if (strcmp(risp, "y") == 0){
        printf("\033[1;31m");
        printf("\nAttenzione! Inserire i nomi dei file (escluso '.bmp')\n");

        printf("\033[1;32m");
        printf("\nNome prima bitmap: ");
        printf("\033[0m");
        scanf("%s",bitmap1);
        strcat(bitmap1, formato);
        foto1 = bm_load(bitmap1);
        mat1 = bitmap_to_ip_mat(foto1);
        bm_free(foto1);

        printf("\033[1;32m");
        printf("\nNome seconda bitmap: ");
        printf("\033[0m");
        scanf("%s",bitmap2);
        strcat(bitmap2, formato);
        foto2 = bm_load(bitmap2);
        mat2 = bitmap_to_ip_mat(foto2);
        bm_free(foto2);
        

    }
    
    selection();
    


    
    
/*
    out = ip_mat_blend(mat1,mat2,1);
    output = ip_mat_to_bitmap(out);
    bm_save(output,"blended.bmp");
    bm_free(output);
    ip_mat_free(mat1);
    ip_mat_free(mat2);
    ip_mat_free(out);
*/
    return 0;
    
}


