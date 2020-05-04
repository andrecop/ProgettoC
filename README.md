# Università Ca' Foscari - Progetto C

## PROGRAMMAZIONE E LABORATORIO [CT0442] - Proff. A. Marin e S. Vascon

<p align="center">
  <a href="https://www.unive.it/"><img src="https://img.shields.io/badge/UniVe-Sito-red" alt="Home: Università Ca' Foscari Venezia" /></a>
  <a href="https://moodle.unive.it/"><img src="https://img.shields.io/badge/UniVe-Moodle-red" alt="Ca' Foscari" /></a>
  <a href="https://moodle.unive.it/course/view.php?id=2694"><img src="https://img.shields.io/badge/Moodle-Corso-brightgreen" alt="Corso: PROGRAMMAZIONE E LABORATORIO [CT0442] - Proff. A. Marin e S. Vascon" /></a>
  <a href="https://docs.google.com/document/d/1liD_FkU5L6NO-F1iHcYiUlTUN_CsGFXBbt6ayf4jleM/edit"><img src="https://img.shields.io/badge/Moodle-Progetto-green" alt="Progetto" /></a>
  <a href="https://docs.google.com/document/d/1liD_FkU5L6NO-F1iHcYiUlTUN_CsGFXBbt6ayf4jleM/edit"><img src="https://img.shields.io/badge/Moodle-Progetto-green" alt="Progetto" /></a>
</p>

# Image Processing Library (v.1.0)
### Scopo:
Realizzare una libreria in linguaggio C per effettuare operazioni di elaborazione di immagini
(image processing).
Il progetto è diviso in tre parti (consegnate in un’unica soluzione):
<ul>
<li>la prima parte in cui dovrete implementare un insieme di metodi per la gestione di matrici a 3 dimensioni, comprendente alcune operazioni matematiche e di gestione della memoria.</li>
<li>nella seconda parte dovrete implementare alcuni semplici metodi per l’elaborazione di immagini (conversione da colore a scala di grigi, corruzione di immagini etc…).</li>
<li>nell’ultima parte vi verrà richiesto di implementare l’algoritmo di convoluzione a cui applicherete filtri per ottenere interessanti effetti.</li>
</ul>
Per ottenere il massimo punteggio nel laboratorio dovrete implementare correttamente tutti i metodi presenti nel file ip_lib.h
<br>

## Introduzione al progetto
### Le immagini e il computer
Le immagini sono codificate nel computer tramite griglie ordinate di pixel. Un pixel è l’entità più piccola per descrivere il colore di un’immagine in un determinato punto.<br>
I pixel di un’immagine tipicamente hanno valori compresi tra 0 e 255, pertanto saranno necessari 8 bit per rappresentare un pixel.<br>
Le immagini sono rappresentate in memoria tramite matrici (l’ordine dei pixel è fondamentale). Nel caso di immagini in scala di grigi abbiamo solo un canale (il canale di luminosità).
<br>
<br>
<p align="center"><img src="Immagini/01.jpg"></p>
<br>
<br>
Nel caso di immagini a colori (il nostro caso), per rappresentare i colori visibili vengono utilizzati 3 canali (rosso, verde e blu). Ogni pixel quindi è composto da 3 valori (rosso, verde e blu). Pertanto la matrice avrà tre dimensioni: larghezza x altezza x 3. Tipicamente si parla di immagini a 24 bit (8 bit * 3 canali = 24 bit).
<br>
<br>
<p align="center"><img src="Immagini/02.jpg"></p>
<br>
<br>
La libreria in C per leggere e scrivere immagini in formato BITMAP vi viene fornita nello zip del progetto e contiene un riadattamento di quella disponibile a questo indirizzo: https://github.com/wernsey/bitmap<br>
Trovate maggiori dettagli sulla libreria a questa pagina http://wstoop.co.za/bitmap.php<br><br>
Per il primo passo vi serviranno i file: test_bmp.c, bmp.c e bmp.h<br><br>
Compilate bmp.c creando un file oggetto bmp.o che userete successivamente. In questo caso potete compilare solo col parametro -Wall.<br><br>
Per testare che tutto funzioni, compilate il file test_bmp.c che trovate nello zip del progetto linkando la libreria bmp.o ed eseguite il file.<br><br>
Al termine dell’esecuzione dovreste ottenere un file bitmap (che potete aprire con un qualsiasi visualizzatore di immagini) chiamato “mandelbrot.bmp” contenente una delle immagini frattali più famose al mondo (se siete curiosi... https://it.wikipedia.org/wiki/Frattale ).
<br>
<br>
<p align="center"><img src="Immagini/03.jpg"></p>
<br>
<br>
Bene, siete riusciti a creare delle immagini col vostro PC! (altrimenti bussate sul forum in Moodle).<br>
Attenzione: Nella libreria bmp.h sono presenti svariati metodi, quelli che potete usare sono:<br>
<ul>
<li>bm_create(...) crea una variabile di tipo Bitmap specificando larghezza (width) e altezza (height)</li>
<li>bm_load(...) Carica un’immagine salvata in formato bitmap su una variabile di tipo Bitmap.</li>
<li>bm_save(...) salva una variabile di tipo Bitmap su file. Bisogna specificare il nome del file.bmp.</li>
<li>bm_free(...) libera la memoria allocata dalla variabile Bitmap.</li>
</ul>
…gli altri sono proibiti, ma li lascio per vostra curiosità.

# Il progetto
## Image Processing 
L’image processing, o elaborazione delle immagini, è una branca della computer science nata negli anni ‘60 con il fine di applicare delle trasformazioni alle immagini più o meno in modo automatico. Applicazioni tipiche sono la rimozione del rumore, l’equalizzazione, la conversione di formati, scalatura, ritaglio o il rilevamento di bordi. In sostanza le operazioni basilari che trovate all’interno di Photoshop o GIMP. 

### PARTE 1: strutture dati, operazioni matematiche e gestione memoria
Per le operazioni successive dobbiamo realizzare una nuovo tipo di dato chiamato ip_mat (image processing matrix). Il tipo ip_mat altro non è che una matrice a 3 dimensioni (altezza x larghezza x canali, vedi figura sotto) in cui saranno memorizzati i pixel dell’immagine e sui quali applicheremo varie trasformazioni. Nella conversione da Bitmap a ip_mat, ai pixel viene fatto un casting a float in quanto le operazioni di elaborazione avranno bisogno di lavorare con dati in virgola, positivi e negativi. Dovremo quindi abbandonare i pixel [0,255] per un po’. 
<br>
<br>
<p align="center"><img src="Immagini/04.jpg"></p>
<br>
<br>
Ci appoggeremo alla seguente struttura che dovrete implementare:
<br>

<pre style="color:#000000;background:#ffffff;"><span style="color:#800000; font-weight:bold; ">typedef</span> <span style="color:#800000; font-weight:bold; ">struct</span> <span style="color:#800080; ">{</span>
    <span style="color:#800000; font-weight:bold; ">unsigned</span> <span style="color:#800000; font-weight:bold; ">int</span> w<span style="color:#800080; ">;</span> <span style="color:#696969; ">/* &lt;- larghezza */</span>
    <span style="color:#800000; font-weight:bold; ">unsigned</span> <span style="color:#800000; font-weight:bold; ">int</span> h<span style="color:#800080; ">;</span> <span style="color:#696969; ">/* &lt;- altezza */</span>
    <span style="color:#800000; font-weight:bold; ">unsigned</span> <span style="color:#800000; font-weight:bold; ">int</span> k<span style="color:#800080; ">;</span> <span style="color:#696969; ">/* &lt;- canali (3 nel nostro caso)*/</span>
    stats <span style="color:#808030; ">*</span> stat<span style="color:#800080; ">;</span>   <span style="color:#696969; ">/* &lt;- statistiche per canale. */</span>
    <span style="color:#800000; font-weight:bold; ">float</span> <span style="color:#808030; ">*</span><span style="color:#808030; ">*</span><span style="color:#808030; ">*</span> data<span style="color:#800080; ">;</span> <span style="color:#696969; ">/* &lt;- matrice 3D di valori */</span>
<span style="color:#800080; ">}</span>ip_mat<span style="color:#800080; ">;</span>
</pre>

<br>
La matrice a tre dimensioni sarà memorizzata nella variabile data.<br>
<br>
Vi viene lasciata libertà sulla struttura di memoria (la variabile data della struttura di cui sopra). Per praticità vi consiglio di utilizzare gli indici frastagliati, però se preferite linearizzare la matrice 3D fate pure (ricordatevi di modificare anche i metodi set_val e get_val).
<p align="center"><u>IMPORTANTE: La matrice data avrà dimensioni h x w x k.</u></p>
<br>
Abbiamo inoltre un tipo di dato stats:
<br>
<pre class="EnlighterJSRAW" data-enlighter-language="c" data-enlighter-linenumbers="true">
typedef struct{
    float min;
    float max;
    float mean;
}stats;
</pre>
<br>
che conterrà alcune statistiche (min, max, mean) sui diversi canali dell’immagine. In sostanza la variabile stat in ip_mat sarà un puntatore ad un vettore di stats di lunghezza k.<br>
<br>
Dovrete implementare i seguenti metodi (per i dettagli vedete il file ip_lib.h):<br>
<pre class="EnlighterJSRAW" data-enlighter-language="c" data-enlighter-linenumbers="true">
ip_mat * ip_mat_create(unsigned int h, unsigned int w, unsigned int k, float v);
void ip_mat_free(ip_mat *a);
float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k);
void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k,  float v);
void ip_mat_init_random(ip_mat * t, float mean, float var);
ip_mat * ip_mat_copy(ip_mat * in);
ip_mat * ip_mat_concat(ip_mat * a, ip_mat * b, int dimensione);
ip_mat * ip_mat_subset(ip_mat * t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end);
</pre>
<br>
alcune operazioni matematiche tra tipi ip_mat:<br>
<pre class="EnlighterJSRAW" data-enlighter-language="c" data-enlighter-linenumbers="true">
ip_mat * ip_mat_sum(ip_mat * a, ip_mat * b);
ip_mat * ip_mat_sub(ip_mat * a, ip_mat * b);
ip_mat * ip_mat_mul_scalar(ip_mat *a, float c);
ip_mat * ip_mat_add_scalar(ip_mat *a, float c);
ip_mat * ip_mat_mean(ip_mat * a, ip_mat * b);
void compute_stats(ip_mat * t);
</pre>
<br>
Trovate invece <u>già implementati</u> i seguenti metodi:<br>
<ul>
<li>Convertire un'immagine da mappa di pixel ad ip_mat e viceversa:<br>
<pre class="EnlighterJSRAW" data-enlighter-language="c" data-enlighter-linenumbers="true">
ip_mat * bitmap_to_ip_mat(Bitmap * img);
Bitmap * ip_mat_to_bitmap(ip_mat * t);
</pre></li>

<li>generazione di numeri da una distribuzione normale:<br>
<pre class="EnlighterJSRAW" data-enlighter-language="c" data-enlighter-linenumbers="true">
float get_normal_random();
</pre></li>

<li>visualizzazione di tipi ip_mat:<br>
<pre class="EnlighterJSRAW" data-enlighter-language="c" data-enlighter-linenumbers="true">
void ip_mat_show_stats(ip_mat * t);
void ip_mat_show(ip_mat * t);
</pre></li>
</ul>