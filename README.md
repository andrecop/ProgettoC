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
<img src="Immagini/01.jpg">