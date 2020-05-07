int main(){

    unsigned int h, w, k;
    float v;
    ip_mat * matrice;


    h = 5;      // lunghezza righe (o n° colonne)
    w = 5;      // n° righe
    k = 3;      // n° di canali, non modificare!
    v = 2;   // numero a caso


    matrice = ip_mat_create(h, w, k, v);

    //printf("%f\n", matrice->data[0][0][0]);

    ip_mat_show(matrice);
    ip_mat * matrice2;
    matrice2 = ip_mat_create(h, w, k, 4.0);
    //ip_mat * matrice2 = ip_mat_copy(matrice);
    //ip_mat_init_random(matrice2, 10.0, 2.0);
    //ip_mat_show(matrice2);
    //ip_mat * matrice3 = ip_mat_subset(matrice, 2,4,2,4);
    //ip_mat_show(matrice3);
    //ip_mat * matrice4 = ip_mat_concat(matrice, matrice2, 2);
    //ip_mat_show(matrice4);
    //ip_mat * matrice5 = ip_mat_sum(matrice, matrice2);
    //ip_mat_show(matrice5);
    //ip_mat * matrice6 = ip_mat_sub(matrice, matrice2);
    //ip_mat_show(matrice6);
    //ip_mat * matrice7 = ip_mat_mul_scalar(matrice, 3.0);
    //ip_mat_show(matrice7);
    //ip_mat * matrice8 = ip_mat_add_scalar(matrice, 3.0);
    //ip_mat_show(matrice8);
    ip_mat * matrice9 = ip_mat_mean(matrice, matrice2);
    ip_mat_show(matrice9);


    //ip_mat_free(matrice);
    //ip_mat_init_random(matrice, 10.0, 2.0);
    //ip_mat_show(matrice);
    //printf("%f\n",matrice->stat[0].max);


    return 0;
}