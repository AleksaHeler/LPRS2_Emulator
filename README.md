# LPRS2 Emulator
Grupni projekat iz predmeta LPRS2. 

**Autori**: *Marko Đorđević, Radomir Zlatković, Aleksa Heler*

## Uvod
Ideja je bila napraviti igricu za emulator koja je slična [Wolfenstein 3D](https://en.wikipedia.org/wiki/Wolfenstein_3D). Kako bi mogli napraviti 3D igru u 2D emulatoru, koristimo Raycasting algoritam (ne raytracing). Kako bi pojednostavili sebi problem, i kako nije neophodno imati vertikalnost u igri (spratovi, stepenice, skokovi), možemo mapu predstaviti 2D matricom celih brojeva, gde broj označava tip zida, i ako je 0 znači prazno polje po kojem se igrač može kretati.

Kako bismo dobili 3D efekat, potrebno je zidove nacrtati tako da im je visina obrnuto proporcionalna udaljenosti od kamere. To radimo tako što crtamo jednu po jednu vertikalnu liniju. Za svaku vertikalnu liniju bacimo zrak u tom smeru i gledamo kad ce udariti u prvi zid i time dobijemo udaljenost, i za to koristimo relativno jednostavan "**Digital Differential Analysis**" ([DDA](https://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm))) algoritam.

DDA je relativno brz i koristi se za pretragu koje kvadrate zrak (ray) pogađa. Mi ga koristimo da nađemo koji kvadrat naše mape je zrak pogodio i zaustavimo algoritam u tom trenutku i nacrtamo vertikalnu liniju čija visina odgovara udaljenosti zida tj. dužini zraka. Ideju za ovaj algoritam nam je dao [Lode Vandevenne](https://lodev.org/), i deo implementacije je odrađen po njegovom [tutorijalu](https://lodev.org/cgtutor/raycasting.html).

TODO: dodati objašnjenje za celu igricu, ne samo raycasting.

## Timeline
- [ ] Proof of concept
  - [X] Render bez tekstura sa dve boje (1bit color indexing)
  - [X] Render bez tekstura sa vise boja (4bit color indexing)
  - [ ] Render sa teksturama (učitavanje iz fajla)
  - [ ] Učitavanje mape iz fajla (idealno slike, ali može bilo koji fajl)
- [ ] Igra
  - [ ] Do kraja implementiran renderer
    - [ ] Naše funkcije sin() i cos() umesto *math.h*
    - [ ] Fixed point aritmetika umesto floating point
  - [ ] Modularne mape i teksture
  - [ ] Napredne kontrole igrača (drugačije kretanje, konstantna brzina nezavisno od FPS, pucanje)
  - [ ] UI (životi, municija, rezultat...)
  - [ ] Items (bure sa: municijom, životima... powerup?)
  - [ ] AI (neprijatelji, kretanje neprijatelja, interakcija sa njima)


## Pokretanje

``` sudo ./waf prerequisites ``` - install waf prerequisites

``` sudo ./waf configure ``` - configure project

``` sudo ./waf build run --app=intro ``` - run the app

## Proof of concept
### proof_of_concept1.c
Koristi 1bit indeksiranje boja, dakle postoje dve boje (u našem slučaju plava 0 i crvena 1). Implementira jednostavno kretanje igrača (napred nazad, okretanje levo desno). Kod iscrtavanja na ekran, nakon što sačeka vSync signal, sve piksele postavi na plavo (pozadina = 0), i zatim prolazi kroz širinu ekrana, od levo ka desno i iscrtava linije odgovarajuće visine koristeći DDA algoritam i svaki zid oboji istom bojom. Mapa je zabeležena u kodu (hard coded) kao dvodimenzionalni niz tipa *int*.

![proof of concept 1](poc/images/proof_of_concept1.png)

### proof_of_concept2.c
Slično kao i prvi p.o.c. samo koristi 4bit indeksiranje za boje, i paletu definišemo na početku koda. Onda se kod iscrtavanja linije gleda koji je broj (tip) kvadrata na mapi pogođen i u zavisnosti od toga oboji liniju. Ostatak je u suštini isti. Može se primetitit da je u drugom p.o.c. rezolucija manja. To je zato što za indeksiranje koristimo više bitova, i sa istim ograničenjem memorije može da stane manje piksela, u ovom slučaju 4x manje, što je rezolucija 320x240 umesto 640x480.

![proof of concept 2](poc/images/proof_of_concept2.png)

