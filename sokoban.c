#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ASCII 123   // Wielkość tablicy do potrzebnych kodów ASCII.
#define ZMIANA 32   // Różnica w kodach ASCII między wielką a małą literą.
// Oznaczenia symboli zgodnie z treścią zadania.
#define DOL 2
#define LEWO 4
#define PRAWO 6
#define GORA 8
#define SCIANA 35
#define POSTAC_N 64
#define POSTAC_D 42
#define DOCELOWE 43
#define NIEDOCELOWE 45
#define KONIEC '.'

// Struktura przechowująca współrzędne.
typedef struct lokalizacja {
	int x;
    int y;
} lok;

// Struktura przechowująca kluczowe dane jednego ruchu.
typedef struct ruch {
    lok postac;
    char symbolPostaci;
    lok skrzynia;
    char symbolSkrzyni;
    int kierunek;
} ruch;

// Element stosu, który przechowuje historię ruchów.
typedef struct node {
    ruch zmiany;
    struct node* next;
} node;

// Sprawdza, czy stos jest pusty.
bool czyPusty(node* top) {
    return top == NULL;
}

// Dodaje element na stos.
void push(node** top, ruch nowaPlansza) {
    node* nowy = (node*)malloc(sizeof(node));
    nowy->zmiany = nowaPlansza;
    nowy->next = *top;
    *top = nowy;
}

// Zdejmuje i zwraca element ze stosu.
ruch pop(node** top) {
    node* temp = *top;
    ruch zmiany = temp->zmiany;
    *top = temp->next;
    free(temp);
    return zmiany;
}

void wczytajPlansze(char ***plansza, int **rozmiary, int *ileWierszy) {
    // Liczba pól w wierszu.
    int kolumny = 0;
    int znak = 0;
    while (znak != '\n') {
        // Tablica znaków dla pojedynczego wiersza.
        char *wiersz = NULL;
        znak = getchar();
        ungetc(znak, stdin);
        if (znak == '\n') {
            continue;
        }    
        while (znak != '\n') {
            znak = getchar();
            // Zwiększanie pamięci, dopóki w danym wierszu są pola.
            if (znak != '\n') {
                kolumny++;
                wiersz = realloc(wiersz, (size_t)kolumny * sizeof(char *));
                wiersz[kolumny - 1] = (char)znak;
            }
            // Aktualizacja liczby wierszy i ich długości, gdy koniec wiersza.
            else {
                (*ileWierszy)++;
                *rozmiary = realloc(*rozmiary,
                                    ((size_t)(*ileWierszy) * sizeof(int)));
                (*rozmiary)[(*ileWierszy) - 1] = kolumny;
            }
        }
        *plansza = realloc(*plansza, ((size_t)(*ileWierszy)* sizeof(char *)));
        (*plansza)[(*ileWierszy) - 1] = malloc((size_t)kolumny * sizeof(char));
        for (int i = 0; i < kolumny; i++) {
                (*plansza)[(*ileWierszy) - 1][i] = wiersz[i];
        }
        kolumny = 0;
        free(wiersz);
        znak = 0;
    }
}

// Tworzy kopię kształtu planszy do tablicy odwiedzeń.
void kopiaPlanszy(bool ***odwiedzone, int *rozmiary, int ileWierszy) {
    *odwiedzone = malloc((size_t)ileWierszy * sizeof(bool *));
    for (int i = 0; i < ileWierszy; i++) {
        (*odwiedzone)[i] = malloc((size_t)rozmiary[i] * sizeof(bool));
    }
    // Wstępne wypełnienie planszy.
    for (int i = 0; i < ileWierszy; i++) {
        for (int j = 0; j < rozmiary[i]; j++) {
            (*odwiedzone)[i][j] = true;
        }
    }
}

// Aktualizuje tablicy odwiedzonych pól tak, aby nieodwiedzone były tylko pola,
// które mogą stanowić ścieżke postaci na odpowiadających im polach planszy.
void aktualizacjaOdwiedzonych(bool ***odwiedzone, char **plansza,
                              int *rozmiary, int ileWierszy) {
    char znak;
    for (int i = 0; i < ileWierszy; i++) {
        for (int j = 0; j < rozmiary[i]; j++) {
            znak = plansza[i][j];
            if (znak == DOCELOWE || znak == NIEDOCELOWE ||
                znak == POSTAC_D || znak == POSTAC_N) {
                (*odwiedzone)[i][j] = false;
            }
            else {
                (*odwiedzone)[i][j] = true;
            }
        }
    }
}

bool czySkrzynia(int znak) {
    return ((znak >= 'A' && znak <= 'Z') || (znak >= 'a' && znak <= 'z'));
}

bool czyMala(char litera) {
    return(litera >= 'a');
}

// Zapamiętuje pozycję postaci i każdej skrzyni na planszy.
void przeszukajPlansze(char **plansza, int *rozmiary, int ileWierszy,
                       lok skrzynie[ASCII], lok *postac) {
    for (int i = 0; i < ASCII; i++) {
        skrzynie[i].x = -1;
        skrzynie[i].y = -1;
    }
    for (int i = 0; i < ileWierszy; i++) {
        for (int j = 0; j < rozmiary[i]; j++) {
            int znak = plansza[i][j]; 
            if (czySkrzynia(znak)) {
                // Zapamiętanie pozycji skrzyni dla jej nazwy zarówno wielką,
                // jak i małą literą.
                if (czyMala((char)znak)) {
                    skrzynie[znak - ZMIANA].x = i;
                    skrzynie[znak - ZMIANA].y = j;
                }
                else {
                    skrzynie[znak + ZMIANA].x = i;
                    skrzynie[znak + ZMIANA].y = j;
                }
                skrzynie[znak].x = i;
                skrzynie[znak].y = j;
            }
            else if (znak == POSTAC_D || znak == POSTAC_N) {
                (*postac).x = i;
                (*postac).y = j;
            }
        }
    }
}

void wypiszPlansze(char **plansza, int *rozmiary, int ileWierszy) {
    for (int i = 0; i < ileWierszy; i++) {
        for (int j = 0; j < rozmiary[i]; j++) {
            printf("%c", plansza[i][j]);
        }
        printf("\n");
    }
}

bool czyWolne (char pole) {
    if (pole == DOCELOWE || pole == NIEDOCELOWE ||
        pole == POSTAC_D || pole == POSTAC_N) {
        return true;
    }
    return false;
}

// Sprawdza, czy pole przed i za skrzynią w podanym kierunku jest wolne
// i nie wychodzi poza zakres planszy.
bool sprawdzPchniecie(int kierunek, char skrzynia, lok skrzynie[ASCII],
                      char **plansza, int *rozmiary, int ileWierszy) {
    int x = skrzynie[(int)skrzynia].x;
    int y = skrzynie[(int)skrzynia].y;
    if (kierunek == DOL || kierunek == GORA) {
        if (x < ileWierszy - 1 && x > 0) {
            if (rozmiary[x - 1] <= y || rozmiary [x + 1] <= y) {
                return false;
            }
            else if (!(czyWolne(plansza[x - 1][y]) &&
                       czyWolne(plansza[x + 1][y]))) {
                return false;
            }
            return true;
        }
        return false;
    }
    else if (kierunek == LEWO || kierunek == PRAWO) {
        if (y < rozmiary[x] - 1 && y > 0) {
            if (!(czyWolne(plansza[x][y + 1]) && czyWolne(plansza[x][y - 1]))) {
                return false;
            }
            return true;
        }
        return false;
    }
    return false;
}

// Modyfikuje planszę zgodnie z wykonaniem ruchu i informacje o aktualnych
// pozycjach skrzyni i postaci.
void pchniecieSkrzyni(int kierunek, char skrzynia, char ***plansza,
                      lok skrzynie[ASCII], lok *postac) {
    lok pozycjaSkrzyni = skrzynie[(int)skrzynia];
    // Pozycja pola, na które przesuwamy skrzynię.
    lok cel = pozycjaSkrzyni;
    // W poleceniu zawsze jest mała litera, ale na planszy może być wielka.
    if (!(czyMala((*plansza)[pozycjaSkrzyni.x][pozycjaSkrzyni.y]))) {
        skrzynia -= ZMIANA;
    }
    // Modyfikacja pola cel, w zależności od kierunku ruchu.
    if (kierunek == DOL) {
        cel.x++;
    }
    else if (kierunek == GORA) {
        cel.x--;
    }
    else if (kierunek == LEWO) {
        cel.y--;
    }
    else if (kierunek == PRAWO) {
        cel.y++;
    }
    // Modyfikacja pola, na którym przed ruchem znajdowała się postać.
    if ((*plansza)[(*postac).x][(*postac).y] == POSTAC_N) {
        (*plansza)[(*postac).x][(*postac).y] = NIEDOCELOWE;
    }
    else if ((*plansza)[(*postac).x][(*postac).y] == POSTAC_D) {
        (*plansza)[(*postac).x][(*postac).y] = DOCELOWE;
    }
    // Modyfikacja pozycji postaci.
    (*postac).x = pozycjaSkrzyni.x;
    (*postac).y = pozycjaSkrzyni.y;
    // Modyfikacja pola, na którym przed ruchem znajdowała się skrzynia,
    // a po ruchu postać.
    if (!(czyMala(skrzynia))) {
        (*plansza)[pozycjaSkrzyni.x][pozycjaSkrzyni.y] = POSTAC_D;
    }
    else if (czyMala(skrzynia)) {
        (*plansza)[pozycjaSkrzyni.x][pozycjaSkrzyni.y] = POSTAC_N;
    }
    // Modyfikacja pozycji skrzyni, zapamiętana pod indeksem zarówno wielkiej
    // jak i małej litery.
    if (czyMala(skrzynia)) {
        skrzynie[(int)skrzynia] = cel;
        skrzynie[skrzynia - ZMIANA] = cel;
    }
    else {
        skrzynie[(int)skrzynia] = cel;
        skrzynie[skrzynia + ZMIANA] = cel;
    }
    // Modyfikacja pola celu na przesuniętą tam skrzynię.
    if ((*plansza)[cel.x][cel.y] == DOCELOWE) {
        if (!(czyMala(skrzynia))) {
            (*plansza)[cel.x][cel.y] = skrzynia;
        }
        else {
            (*plansza)[cel.x][cel.y] = skrzynia - ZMIANA;
        }
    }
    else if ((*plansza)[cel.x][cel.y] == NIEDOCELOWE) {
        if (!czyMala(skrzynia)) {
            (*plansza)[cel.x][cel.y] = skrzynia + ZMIANA;
        }
        else {
            (*plansza)[cel.x][cel.y] = skrzynia;
        }
    }
}

// Rekurencyjnie sprawdza, czy istnieje ścieżka po wolnych polach od miejsca
// na pozycji x, y do celu.
bool znajdzSciezke(int x, int y, lok cel, bool ***odwiedzone,
                   int *rozmiary, int ileWierszy) {
    // Pole celu znalezione.
    if (x == cel.x && y == cel.y) {
        return true;
    }
    (*odwiedzone)[x][y] = true;
    // Idzie w górę.
    if (x - 1 >= 0 && rozmiary[x - 1] > y && !(*odwiedzone)[x - 1][y]) {
        bool zwrot = znajdzSciezke(x - 1, y, cel, odwiedzone,
                                   rozmiary, ileWierszy);
        if (zwrot) {
            return true;
        }
    }
    // Idzie w lewo.
    if (y - 1 >= 0 && !(*odwiedzone)[x][y - 1]) {
        bool zwrot = znajdzSciezke(x, y - 1, cel, odwiedzone,
                                   rozmiary, ileWierszy);
        if (zwrot) {
            return true;
        }
    }
    // Idzie w dół.
    if (x + 1 < ileWierszy && rozmiary[x + 1] > y && !(*odwiedzone)[x + 1][y]) {
        bool zwrot = znajdzSciezke(x + 1, y, cel, odwiedzone,
                                   rozmiary, ileWierszy);
        if (zwrot) {
            return true;
        }
    }
    // Idzie w prawo.
    if (y + 1 < rozmiary[x] && !(*odwiedzone)[x][y + 1]) {
        bool zwrot = znajdzSciezke(x, y + 1, cel, odwiedzone,
                                   rozmiary, ileWierszy);
        if (zwrot) {
            return true;
        }
    }
    return false;
}

// Modyfikuje planszę zgodnie z cofnięciem ruchu.
void cofnijRuch(ruch zdjety, char ***plansza, lok skrzynie[ASCII],
                lok *postac) {
    // Pozycja, na której była skrzynia w poprzednim ruchu.
    lok lokPostaci = zdjety.postac;
    // Pozycja, na której była skrzynia w poprzednim ruchu.
    lok lokSkrzyni = zdjety.skrzynia;
    // Pozycja, na której jest obecnie skrzynia przed cofnięciem.
    lok dodatkowe = lokSkrzyni;
    // Ustawienie pozycji pola dodatkowe w zależności od kierunku ruchu.
    if (zdjety.kierunek == GORA) {
        dodatkowe.x--;
    }
    else if (zdjety.kierunek == DOL) {
         dodatkowe.x++;
    }
    else if (zdjety.kierunek == LEWO) {
        dodatkowe.y--;
    }
    else if (zdjety.kierunek == PRAWO) {
         dodatkowe.y++;
    }
    // Modyfikacja pola ze skrzynią na pole wolne.
    if (czyMala((*plansza)[dodatkowe.x][dodatkowe.y])) {
        (*plansza)[dodatkowe.x][dodatkowe.y] = NIEDOCELOWE;
    }
    else {
        (*plansza)[dodatkowe.x][dodatkowe.y] = DOCELOWE;
    }
    // Modyfikacja pola, na którym w poprzednim ruchu była postać.
    (*plansza)[lokPostaci.x][lokPostaci.y] = zdjety.symbolPostaci;
    // Modyfikacja pola postaci na pole skrzyni.
    if ((*plansza)[lokSkrzyni.x][lokSkrzyni.y] == POSTAC_D) {
        if (czyMala(zdjety.symbolSkrzyni)) {
            (*plansza)[lokSkrzyni.x][lokSkrzyni.y] =
                zdjety.symbolSkrzyni - ZMIANA;
        }
        else {
            (*plansza)[lokSkrzyni.x][lokSkrzyni.y] = zdjety.symbolSkrzyni;
        }
    }
    else {
        if (czyMala(zdjety.symbolSkrzyni)) {
            (*plansza)[lokSkrzyni.x][lokSkrzyni.y] = zdjety.symbolSkrzyni;
        }
        else {
            (*plansza)[lokSkrzyni.x][lokSkrzyni.y] =
                zdjety.symbolSkrzyni + ZMIANA;
        }
    }
    // Aktualizacja pozycji skrzyni.
    if (czyMala(zdjety.symbolSkrzyni)) {
        skrzynie[zdjety.symbolSkrzyni - ZMIANA] = lokSkrzyni;
    }
    else {
        skrzynie[zdjety.symbolSkrzyni + ZMIANA] = lokSkrzyni;
    }
    skrzynie[(int)zdjety.symbolSkrzyni] = lokSkrzyni;
    // Aktualizacja pozycji postaci.
    *postac = lokPostaci;
}

void wczytajPolecenia(bool ***odwiedzone, char ***plansza, int ileWierszy,
                      lok skrzynie[ASCII], int *rozmiary, lok postac,
                      node **stos) { 
    int znak = -1;
    int kierunek;
    lok cel;
    // Ostatni wykonany ruch.
    ruch ostatni;
    // Zdjęty ze stosu ruch.
    ruch zdjety;
    while (znak != KONIEC) {
        znak = getchar();
        if (znak == KONIEC) {
            continue;
        }
        else if (znak == '\n') {
            wypiszPlansze(*plansza, rozmiary, ileWierszy);
            znak = -1;
            continue;
        }
        // Konwertowanie na cyfrę, aby sprawdzić czy znak jest zerem.
        znak -= '0';
        if (znak != 0) {
            // Dodanie z powrotem odjętej wartości, aby mieć poprawny kod ASCII
            // znaku, który jest literą.
            znak += '0';
            kierunek = getchar();
            kierunek -= '0';
            // Niewykonywanie dalsze ruchu, gdy nie jest to możliwe ze względu
            // na samą pozycję skrzyni.
            if (!sprawdzPchniecie(kierunek, (char)znak, skrzynie, *plansza,
                                  rozmiary, ileWierszy)) {
                continue;
            }
            aktualizacjaOdwiedzonych(odwiedzone, *plansza,
                                     rozmiary, ileWierszy);
            // Pole na które musi dostać się postać, aby możliwe było pchnięcie.
            cel = skrzynie[znak];
            if (kierunek == DOL) {
                cel.x--;
            }
            else if (kierunek == GORA) {
                cel.x++;
            }
            else if (kierunek == LEWO) {
                cel.y++;
            }
            else if (kierunek == PRAWO) {
                cel.y--;
            }
            // Niewykonywanie dalsze ruchu, gdy nie istnieje poprawna ścieżka.
            if (!znajdzSciezke(postac.x, postac.y, cel, odwiedzone,
                               rozmiary, ileWierszy)) {
                continue;
            }
            // Włożenie na stos stanu planszy sprzed wykonania ruchu, ale po
            // upewnieniu, że ruch zajdzie.
            ostatni.postac = postac;
            ostatni.symbolPostaci = (*plansza)[postac.x][postac.y];
            ostatni.skrzynia = skrzynie[znak];
            lok lokSkrzyni = ostatni.skrzynia;
            ostatni.symbolSkrzyni = (*plansza)[lokSkrzyni.x][lokSkrzyni.y];
            ostatni.kierunek = kierunek;
            push(stos, ostatni);
            // Wykonanie ruchu.
            pchniecieSkrzyni(kierunek, (char)znak, plansza, skrzynie, &postac);
        }
        // Cofanie ruchu.
        if (znak == 0) {
            // Gdy istnieje ruch do cofnięcia.
            if (!czyPusty(*stos)) {
                zdjety = pop(stos);
                cofnijRuch(zdjety, plansza, skrzynie, &postac);
            }
        }
    }
}

// Zwalnia pamięć, którą zajmował stos.
void zwolnijStos(node** top) {
    while (*top != NULL) {
        node* temp = *top;
        *top = (*top)->next;
        free(temp);
    }
}

int main() {
    char **plansza = NULL;
    bool **odwiedzone = NULL;
    // Przechowuje rozmiary wszystkich wierszy planszy.
    int *rozmiary = NULL;
    node *stosRuchow = NULL;
    int ileWierszy = 0;
    lok postac;
    // Przechowuje pozycje skrzyń pod indeksami zgodnymi z kodem ASCII ich nazw.
    lok skrzynie[ASCII];
    wczytajPlansze(&plansza, &rozmiary, &ileWierszy);
    kopiaPlanszy(&odwiedzone, rozmiary, ileWierszy);
    przeszukajPlansze(plansza, rozmiary, ileWierszy, skrzynie, &postac);
    wczytajPolecenia(&odwiedzone, &plansza, ileWierszy, skrzynie, rozmiary,
                     postac, &stosRuchow);
    // Zwalnianie pamięci
    for (int i = 0; i < ileWierszy; i++) {
        free(plansza[i]);
        free(odwiedzone[i]);
    }
    free(plansza);
    free(odwiedzone);
    free(rozmiary);
    zwolnijStos(&stosRuchow);
    return 0;
}
