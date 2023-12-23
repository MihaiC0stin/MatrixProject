# Despre ce este proiectul ?
Proiectul consta in programarea unui joc folosind placuta Arduino Uno.

## Cerintele proiectului sunt:

– Minimal components: an LCD, a joystick, a buzzer and the led
matrix.
– You must add basic sounds to the game (when ”eating” food, when
dying, when finishing the level etc). Extra: add theme songs.
– Each level / instance should work on 16x16 matrix. You can apply
the concept of visibility / fog of war (aka you only see 8x8 of the
total 16x16 matrix, and you discover more as you move around) or
you can use the concept of ”rooms”. Basically you will have 4 rooms
that you need to go through on each level.
– It must be intuitive and fun to play.
– It must make sense in the current setup.
– You should have a feeling of progression in difficulty. Depending
on the dynamic of the game, this is done in the same level or with
multiple levels. You can make them progress dynamically or have
a number of fixed levels with an endgame. Try to introduce some
randomness, though.
  
# Despre ce este jocul meu ?
La inceput am incercat sa fac mai multe tipuri de jocuri. Am inceput cu speranta de a face un rpg, dar am renuntat la aceasta idee din cauza multitudinii de text care se salveaza in RAM/flash, a complexitatii si a memoriei. Apoi am incercat sa fac un joc de tip zombie survival, unde playerul este alergat de zombi, si poate construi ziduri sa se apere. In cele din urma am ramas pe un joc de tip rogue, unde playerul doar este alergat de monstrii, iar scorul se obtine omorand monstrii, la dificultati cat mai mari, in acelasi timp avand grija sa nu moara.

# Detalii Joc
Meniu Interactic:
      -Play game: Apasa pentru a incepe jocul
      -Settings:
            -Mtrix Brightness: Seteaza luminozitatea matricii
            -Lcd Brightness: Seteaza luminozitatea matricii
            -Toggle Sound: Modifica statusul sunetului din On in Off si vice versa
      -Highscores: Afiseaza pe ecran top 5 scoruri
      -About: Afiseaza pe ecran nume jocului, al autorului, si numele de github al acestuia
      -How To Play: Afiseaza pe ecran o scurta explicatie a cum se joaca jocul.

Joc:
   Foloseste joystickul pentru a te misca si butonul cel mai din stanga pentru a ataca. Scopul jocului este de a ajunge pana la nivelul 7, unde monstrii au statusurile maxate, iar scorul pe care-l ofera cand e maxat, este si el maxim.
Pentru a ajunge la nivelul 7 playerul are 2 optiuni. Una dintre ele este sa omoare toti monstrii, care sunt in viata. Cealalta optiune este sa se fereasca de atacurile monstrilor si sa supravietuiasca 1 minut, dar trebuie sa aiba grija fiindca si statusurile monstrilor de la nivelurile anterioare vor creste impreuna cu dificultatea. In cele din urma daca playerul a reusit sa faca highscore, va fi afisat un ecran in care acesta poate sa-si setezele numele miscand din joystick in jos si in sus pentru a selecta litera, si stanga dreapta pentru a alege pozitia literei. Prin apasarea butonului cel mai din stanga, numele va fi salvat, si va aparea in highscore.

# Setup

<img src = "https://github.com/MihaiC0stin/MatrixProject/assets/103452863/4d66a8aa-5f58-4802-8fa2-e5b8becb4c6c" width=50% height=50%>

# Componente folosite

- Arduino Uno x1
- Matrix 8x8 x1
- Condensator x1
- Ecran LCD x1
- Butoane x2
- Potentiometru x1
- Rezistenta 100k Ohm x1
- Rezistenta 220 Ohm x1
- Joystick x1
  
# Video

Link: https://youtu.be/iqaTxT9OfF0

# Buguri

1. Pe video se poate sa apara ca apas uneori de 2 ori, la Toggle sound, am rezolvat, trebuia scos print ul din if-ul care verifica daca butonul a fost apasat.
2. Mai este un singur bug, care apare rar, am stat ore, daca nu zile sa inteleg din ce cauza apare, si sa-l recreez macar. Uneori sunt pe mapa niste monstrii "fantoma", nu poti trece prin ei, sau sa le dai damage si au blink, dar nu se misca spre player, nici ataca cand e in fata lui. Suspectez ca problema ar fi cand ii adaug, sau cand ii scot din array, dar totul pare in regula acolo.

# Mentiuni

Fata de cand am prezentat la laborator, am rezolvat majoritatea bugurilor, inclusiv highscore (fun fact: bugul acela era din cauza ca puneam score ul in highscore din momentul cand se termina jocul, iar acesta era deja sortat pana sa ajunga in functia de setare nume, de aceea numele ramanea mereu pe ultimul loc, iar scorul pe primul), am refacut setupul (e rezistent acm, nu mai scot firele alea nici de-as vrea :D ), am adaugat setarile in EEPROM (raman aceleasi setari dupa reset la arduino) si am adaugat animatii pentru meniu si startup.
N-am adaugat buzzer si nici n-am salvat highscore-urile in EEPROM.

PS. Am pus la 1 dimineata inca un commit fiindca mi-am amintit sa mentionez cateva buguri si am mai adaugat cateva comentarii unde cred ca trebuia. Promit sa fie ultimul, o sa ma bucur de sarbatori. :))



