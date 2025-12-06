
/*––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––*\
|                                                                            |
|  HOTELLIN VARAUSJÄRJESTELMÄ                                                |
|                                                                            |
|  Ohjelma hallinnoi hotellihuoneiden varauksia. Ominaisuuksiin kuuluu:      |
|  - Huoneiden arvonta ja alustus (1hh ja 2hh)                               |
|  - Uuden varauksen tekeminen (nimellä, öiden määrällä ja lisäpalveluilla)  |  
|  - Automaattinen alennusprosentin arvonta ja hinnanlaskenta                |
|  - Varauksen etsiminen nimellä tai varausnumerolla                         |
|  - Varauksen muokkaaminen (öiden muutos, palvelut) ja peruuttaminen        |
|  - Tietojen tallennus ja lataus tiedostosta (pysyvä muisti)                |
|                                                                            |
|  Tekijä: [Daniel Pitkänen]                                                 |
|  Pvm:    [3.12.2025]                                                       |
\*––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––*/

#include <iostream>  
#include <vector>    
#include <string>    
#include <cstdlib>   
#include <ctime>     
#include <limits>    
#include <iomanip>   
#include <fstream>   
#include <clocale>   

using namespace std;

// VÄRIMOODIT
const string V_RESET      = "\033[0m";  //Resettaa kaikki värimoodit 
// Tekstivärit
const string V_MUSTA      = "\033[30m";
const string V_PUNAINEN   = "\033[31m";
const string V_VIHREA     = "\033[32m";
const string V_KELTAINEN  = "\033[33m";
const string V_SININEN    = "\033[34m";
const string V_MAGENTA    = "\033[35m"; // Pinkki
const string V_SYANI      = "\033[36m"; // Turkoosi
const string V_VALKOINEN  = "\033[37m";
// Kirkkaat tekstivärit (näkyvät paremmin tummalla taustalla)
const string V_KIRKAS_PUN = "\033[91m";
const string V_KIRKAS_VIH = "\033[92m";
const string V_KIRKAS_VAL = "\033[97m"; // Kirkas valkoinen
// Taustavärit 
const string V_TAUSTA_SIN = "\033[44m";
const string V_TAUSTA_VIH = "\033[42m";
const string V_TAUSTA_PUN = "\033[41m";
// Tehosteet
const string V_LIHAVOITU  = "\033[1m";
const string V_ALLEVIIVAUS= "\033[4m";


//GLOBAALIT VAKIOT
const int HINTA_1HH = 100;      // 1 hengen huoneen perushinta / yö
const int HINTA_2HH = 150;      // 2 hengen huoneen perushinta / yö
const int HINTA_AAMUPALA = 30;  // Aamupalan hinta per yö
const int HINTA_SPA = 25;       // Spa-osaston kertamaksu per varaus
const int MIN_HUONEET = 40;     // Hotellin minimikoko
const int MAX_HUONEET = 300;    // Hotellin maksimikoko
const string TIEDOSTO_NIMI = "hotelli_data.txt"; // Tiedoston nimi tallennusta varten

//TIETUEET
struct Huone {
    int numero;           // Huoneen numero 
    int kapasiteetti;     // 1 tai 2 henkilöä
    bool onVarattu;       // true = varattu, false = vapaa
    int varausNumero;     // Yksilöllinen varauskoodi 
    string varaajanNimi;  // Asiakkaan nimi
    int yot;              // Varauksen kesto öinä
    bool aamupala;        // Onko aamupala valittu
    bool spa;             // Onko spa valittu
    int alennusProsentti; // Tallennettu alennus 0, 10 tai 20
};

//ALIOHJELMIEN ESITTELYT
void alustaHuoneet(vector<Huone>& huoneet);
int lueKokonaisluku(int min, int max, string kehote);
void tulostaOtsikko();
void tulostaValikko();
void odotaEnter();
void tyhjennaRuutu();
double laskeHinta(const Huone& h); 

// Päätoiminnot
void teeVaraus(vector<Huone>& huoneet);
void etsiVaraus(const vector<Huone>& huoneet);
void muokkaaVarausta(vector<Huone>& huoneet); 
void listaaHuoneet(const vector<Huone>& huoneet);

// Tiedostonkäsittely
void tallennaTiedot(const vector<Huone>& huoneet);
bool lataaTiedot(vector<Huone>& huoneet);

//PÄÄOHJELMA
int main() {
    setlocale(LC_ALL, ""); //ääkköset
    
    // Alustetaan satunnaislukugeneraattori nykyisellä ajalla 
    srand(static_cast<unsigned int>(time(0)));
    
    //vektori, joka toimii ohjelman päätietovarastona
    vector<Huone> huoneet;
    
    //ladata tiedot tiedostosta
    if (lataaTiedot(huoneet)) {
        cout << V_VIHREA << "Tiedot ladattiin onnistuneesti tiedostosta!" << V_RESET << endl;
        odotaEnter();
    } else {
        // Jos tiedostoa ei ole, luodaan huoneet
        alustaHuoneet(huoneet);
    }
    
    // Pääsilmukka, joka pitää ohjelman käynnissä kunnes käyttäjä haluaa lopettaa
    bool jatkuu = true;
    while (jatkuu) {
        tyhjennaRuutu();
        tulostaOtsikko();
        // Näytetään huoneiden kokonaismäärä informatiivisena tietona
        cout << "Hotellissa on tänä iltana " << V_SYANI << huoneet.size() << V_RESET << " huonetta." << endl;
        
        tulostaValikko();
        
        // Luetaan käyttäjän valinta virheenkestävällä funktiolla
        int valinta = lueKokonaisluku(0, 4, "Valitse toiminto > ");

        switch (valinta) {
            case 1: 
                teeVaraus(huoneet); 
                break;
            case 2: 
                etsiVaraus(huoneet); 
                odotaEnter(); // Odottaa, että käyttäjä ehtii lukea tulokset
                break;
            case 3: 
                listaaHuoneet(huoneet); 
                odotaEnter(); 
                break;
            case 4: 
                muokkaaVarausta(huoneet); 
                break;
            case 0:
                // Tallennetaan tiedot ennen poistumista, jotta ne säilyvät seuraavaan kertaan.
                tallennaTiedot(huoneet);
                cout << "\nKiitos käynnistä. " << V_VIHREA << "Näkemiin!" << V_RESET << endl;
                jatkuu = false; // Katkaisee while-silmukan
                break;
            default: 
                break;
        }
    }
    return 0; 
}

//APUFUNKTIOT

// Tyhjentää terminaalin näytön.
void tyhjennaRuutu() {
    // Käyttää esikääntäjäkomentoja (#ifdef) tunnistamaan käyttöjärjestelmän
    #ifdef _WIN32
        system("cls");   // Windows
    #else
        system("clear"); // Linux tai macOS
    #endif
}

// Pysäyttää ohjelman ja odottaa Enterin painallusta
void odotaEnter() {
    cout << "\n" << V_KELTAINEN << "Paina Enter jatkaaksesi..." << V_RESET;
    cin.get(); // Lukee yhden merkin puskurista
}

// Laskee huoneen kokonaishinnan huomioiden kaikki muuttujat
double laskeHinta(const Huone& h) {
    // 1. Perushinta huoneen tyyppi * yöt
    double huoneHinta = (h.kapasiteetti == 1) ? HINTA_1HH : HINTA_2HH;
    double yotHinta = (huoneHinta * h.yot);
    
    // 2. Lisätään aamupala jos valittu 
    if (h.aamupala) {
        yotHinta += (HINTA_AAMUPALA /* h.yot*/); //poistamalla kommentti voi tehdä aamupalan hinta per yö
    }
    
    // 3. Lasketaan alennus majoituksesta ja aamupalasta
    // Kaava: Hinta * ((100 - ale%) / 100)
    double alennettuHinta = yotHinta * ((100.0 - h.alennusProsentti) / 100.0);

    // 4. Lisätään Spa (kertamaksu, johon alennus ei tässä mallissa vaikuta)
    double spaHinta = h.spa ? HINTA_SPA : 0;
    
    return alennettuHinta + spaHinta;
}

// TIEDOSTOJEN KÄSITTELY

// Tallentaa kaikki huoneet tekstitiedostoon ja käyttää '|' merkkiä tietokenttien erottimena
void tallennaTiedot(const vector<Huone>& huoneet) {
    ofstream tiedosto(TIEDOSTO_NIMI); // Avataan tiedosto kirjoittamista varten
    
    if (!tiedosto.is_open()) {
        cout << V_PUNAINEN << "Virhe: Tiedoston tallennus epäonnistui!" << V_RESET << endl;
        return;
    }
    
    for (const auto& h : huoneet) {
        // Kirjoitetaan tiedot määrätyssä järjestyksessä
        tiedosto << h.numero << "|" 
                 << h.kapasiteetti << "|" 
                 << h.onVarattu << "|" 
                 << h.varausNumero << "|" 
                 << h.yot << "|" 
                 << h.aamupala << "|"
                 << h.spa << "|" 
                 << h.alennusProsentti << "|" 
                 << h.varaajanNimi << endl;
    }
    
    tiedosto.close(); // Suljetaan tiedosto resurssien vapauttamiseksi
    cout << V_VIHREA << "Tiedot tallennettu tiedostoon " << TIEDOSTO_NIMI << V_RESET << endl;
}

// Lukee tiedot tiedostosta ja palauttaa ne vektoriin.
bool lataaTiedot(vector<Huone>& huoneet) {
    ifstream tiedosto(TIEDOSTO_NIMI); // Avataan tiedosto lukemista varten
    if (!tiedosto.is_open()) return false; // Jos tiedostoa ei ole, palautetaan false
    
    huoneet.clear(); // Varmistetaan, että vektori on tyhjä ennen latausta
    string rivi;
    
    // Luetaan rivi kerrallaan
    while (getline(tiedosto, rivi)) {
        if (rivi.empty()) continue; // Skipataan tyhjät rivit
        
        Huone h;
        size_t pos = 0, next_pos;

        // "Try-catch" -lohko estää ohjelman kaatumisen, jos tiedosto on viallinen
        try {
            // Erotellaan (parsitaan) tiedot |-merkkien välistä stringistä kokonaisluvuiksi (stoi)
            
            // 1. Numero
            next_pos = rivi.find('|', pos); 
            h.numero = stoi(rivi.substr(pos, next_pos - pos)); 
            pos = next_pos + 1;

            // 2. Kapasiteetti
            next_pos = rivi.find('|', pos); 
            h.kapasiteetti = stoi(rivi.substr(pos, next_pos - pos)); 
            pos = next_pos + 1;

            // 3. OnVarattu
            next_pos = rivi.find('|', pos); 
            h.onVarattu = stoi(rivi.substr(pos, next_pos - pos)); 
            pos = next_pos + 1;

            // 4. Varausnumero
            next_pos = rivi.find('|', pos); 
            h.varausNumero = stoi(rivi.substr(pos, next_pos - pos)); 
            pos = next_pos + 1;

            // 5. Yöt
            next_pos = rivi.find('|', pos); 
            h.yot = stoi(rivi.substr(pos, next_pos - pos)); 
            pos = next_pos + 1;
            
            // 6. Aamupala
            next_pos = rivi.find('|', pos); 
            h.aamupala = (next_pos != string::npos) ? stoi(rivi.substr(pos, next_pos - pos)) : 0; 
            pos = next_pos + 1;

            // 7. Spa
            next_pos = rivi.find('|', pos);
            h.spa = (next_pos != string::npos) ? stoi(rivi.substr(pos, next_pos - pos)) : 0; 
            pos = next_pos + 1;

            // 8. Alennusprosentti (Lisätty korjauksessa)
            next_pos = rivi.find('|', pos);
            if (next_pos != string::npos) {
                h.alennusProsentti = stoi(rivi.substr(pos, next_pos - pos)); 
                pos = next_pos + 1;
            } else {
                h.alennusProsentti = 0; // Oletusarvo vanhoille tiedostoille
            }

            // 9. Nimi 
            h.varaajanNimi = rivi.substr(pos);
            
            huoneet.push_back(h); // Lisätään valmis huone vektoriin
        } catch (...) { 
            continue; // Jos rivi on rikki, hypätään seuraavaan
        }
    }
    tiedosto.close();
    return true;
}

// Alustaa hotellin satunnaisella määrällä huoneita 40-300
void alustaHuoneet(vector<Huone>& huoneet) {
    // Kaava varmistaa, että luku on parillinen
    int n = (rand() % ((MAX_HUONEET / 2) - (MIN_HUONEET / 2) + 1) + (MIN_HUONEET / 2)) * 2;
    
    huoneet.clear();
    for (int i = 0; i < n; ++i) {
        Huone uusiHuone;
        uusiHuone.numero = i + 1;
        uusiHuone.onVarattu = false; // Aluksi vapaa
        uusiHuone.varausNumero = 0;
        uusiHuone.varaajanNimi = "";
        uusiHuone.yot = 0;
        uusiHuone.aamupala = false;
        uusiHuone.spa = false;
        uusiHuone.alennusProsentti = 0;
        
        // Puolet huoneista 1hh, puolet 2hh
        uusiHuone.kapasiteetti = (i < n / 2) ? 1 : 2;
        
        huoneet.push_back(uusiHuone);
    }
}

//UI LOGIIKKA 

// Lukee käyttäjältä kokonaisluvun ja varmistaa sen oikeellisuuden sekä estää ohjelman kaatumisen, jos käyttäjä syöttää kirjaimia
int lueKokonaisluku(int min, int max, string kehote) {
    int syote;
    while (true) {
        cout << kehote;
        // cin >> palauttaa true, jos luvun luku onnistui
        if (cin >> syote) {
            // Tarkistetaan onko luku sallitulla välillä
            if (syote >= min && syote <= max) {
                // Puhdistetaan syötepuskuri, jotta seuraava luku onnistuu
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return syote;
            } else {
                cout << V_PUNAINEN << "Virhe: Syötä luku väliltä " << min << "-" << max << "." << V_RESET << endl;
            }
        } else {
            // Jos käyttäjä syötti merkkejä, cin menee virhetilaan
            cout << V_PUNAINEN << "Virhe: Syötteen tulee olla numero." << V_RESET << endl;
            cin.clear(); // Nollataan virhetila
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Poistetaan virheellinen syöte
        }
    }
}

void tulostaOtsikko() {
    //Päämenun "logo"
    cout << V_SYANI << V_LIHAVOITU;
    cout << "  _   _       _       _ _ _ " << endl;
    cout << " | | | | ___ | |_ ___| | (_)" << endl;
    cout << " | |_| |/ _ \\| __/ _ \\ | | |" << endl;
    cout << " |  _  | (_) | ||  __/ | | |" << endl;
    cout << " |_| |_|\\___/ \\__\\___|_|_|_|" << endl;
    cout << V_RESET << endl;
}

void tulostaValikko() {
    cout << "-----------------------------" << endl;
    cout << " [" << V_SININEN << "1" << V_RESET << "]  " << V_LIHAVOITU << "Tee uusi varaus" << V_RESET << endl;
    cout << " [" << V_SININEN << "2" << V_RESET << "]  Etsi varausta" << endl;
    cout << " [" << V_SININEN << "3" << V_RESET << "]  Listaa huoneet" << endl;
    cout << " [" << V_SININEN << "4" << V_RESET << "]  " << V_LIHAVOITU << "Muokkaa varausta" << V_RESET << endl;
    cout << " [" << V_SYANI << "0" << V_RESET << "]  Lopeta (Tallennus)" << endl;
    cout << "-----------------------------" << endl;
}

// TOIMINNOT

// Hoitaa uuden varauksen luomisen alusta loppuun
void teeVaraus(vector<Huone>& huoneet) {
    tyhjennaRuutu();
    cout << V_LIHAVOITU << "  -~- " << V_SYANI << "UUSI VARAUS" << V_RESET << V_LIHAVOITU <<  " -~-   " << V_RESET << endl;
    cout << "[" << V_SININEN << "0" << V_RESET << "]  Palaa alkuvalikkoon" << endl;
    cout << "[" << V_SININEN << "1" << V_RESET << "]  Yhden hengen huone (1hh) - " << V_KELTAINEN << HINTA_1HH << " eur/yö" << V_RESET << endl;
    cout << "[" << V_SININEN << "2" << V_RESET << "]  Kahden hengen huone (2hh) - " << V_KELTAINEN << HINTA_2HH << " eur/yö" << V_RESET << endl;
    
    int valinta = lueKokonaisluku(0, 2, "Valinta: ");
    if (valinta == 0) return; // Paluu

    int huoneKoko = valinta;
    
    // Tarkistetaan onko yhtään vapaata huonetta jäljellä tässä kategoriassa
    bool vapaana = false;
    for (const auto& h : huoneet) {
        if (h.kapasiteetti == huoneKoko && !h.onVarattu) {
            vapaana = true; break;
        }
    }
    if (!vapaana) {
        cout << V_PUNAINEN << "Ei vapaita huoneita." << V_RESET << endl;
        odotaEnter(); return;
    }

    // Kysytään, haluaako käyttäjä valita itse vai annetaanko koneen arpoa
    cout << "\n[" << V_SININEN << "1" << V_RESET << "] Valitsen itse\n[" << V_SININEN << "2" << V_RESET << "] Arvo huone\n[" << V_PUNAINEN << "0" << V_RESET << "] Peruuta\n";
    int valintaTapa = lueKokonaisluku(0, 2, "Valinta: ");
    if (valintaTapa == 0) return;

    int valittuHuone = -1;
    if (valintaTapa == 1) {
        // Listataan vapaat huoneet (max 15 kpl, ettei ruutu täyty)
        cout << "Vapaat: " << V_VIHREA;
        int count = 0;
        for (const auto& h : huoneet) {
            if (h.kapasiteetti == huoneKoko && !h.onVarattu) {
                cout << h.numero << " ";
                if (++count > 15) { cout << "..."; break; }
            }
        }
        cout << V_RESET << endl;
        
        // Luetaan huonevalinta ja tarkistetaan, että se on vapaa ja oikeaa tyyppiä
        while(true) {
            int toive = lueKokonaisluku(1, huoneet.size(), "Anna huoneen numero: ");
            if (huoneet[toive-1].kapasiteetti == huoneKoko && !huoneet[toive-1].onVarattu) {
                valittuHuone = toive; break;
            }
            cout << V_PUNAINEN << "Ei vapaa tai väärä tyyppi. \n Yritä uudestaan" << V_RESET << endl;
        }
    } else {
        // Kone arpoo huoneen vapaiden listasta
        vector<int> vapaat;
        for (const auto& h : huoneet) if (h.kapasiteetti == huoneKoko && !h.onVarattu) vapaat.push_back(h.numero);
        valittuHuone = vapaat[rand() % vapaat.size()];
        cout << "Arvottu huone: " << V_LIHAVOITU << V_VIHREA << valittuHuone << V_RESET << endl;
    }
    
    // Nimen kysyminen ja validointi
    string nimi;
    while (true) {
        cout << "Anna varaajan nimi: ";
        getline(cin, nimi);
        // Tarkistetaan, ettei nimi ole tyhjä eikä sisällä '|' merkkiä, joka rikkoisi tiedoston
        if (nimi.length() > 0 && nimi.find('|') == string::npos) break;
        cout << V_PUNAINEN << "Virheellinen nimi (ei saa olla tyhjä, ei '|' merkkiä)." << V_RESET << endl;
    }
    
    int yot = lueKokonaisluku(1, 365, "Öiden määrä: ");
    
    // Lisäpalveluiden kysyminen
    cout << V_LIHAVOITU << "\n -~- " << V_SYANI << "Lisäpalvelut" << V_RESET << V_LIHAVOITU << " -~-" << V_RESET << endl;
    cout << " Aamupala (" << HINTA_AAMUPALA << "€)? \n [" << V_SININEN << "1" << V_RESET << "] Kyllä \n [" << V_SININEN << "0" << V_RESET << "] Ei " << endl;
    bool aamupala = (lueKokonaisluku(0, 1, "> ") == 1);
    cout << " Spa (" << HINTA_SPA << "€)? \n [" << V_SININEN << "1" << V_RESET << "] Kyllä \n [" << V_SININEN << "0" << V_RESET << "] Ei " << endl;
    bool spa = (lueKokonaisluku(0, 1, "> ") == 1);

    // Arvotaan uniikki varausnumero (10000-99999)
    int varausID;
    do {
        varausID = 10000 + (rand() % 90000);
        bool kaytossa = false;
        // Varmistetaan loopilla, ettei sama ID ole jo käytössä
        for (const auto& h : huoneet) if (h.onVarattu && h.varausNumero == varausID) kaytossa = true;
        if (!kaytossa) break;
    } while (true);
    
    // Arvotaan alennus (0%, 10% tai 20%)
    int arpa = rand() % 3;
    int alePros = (arpa == 1) ? 10 : (arpa == 2) ? 20 : 0;

    // --- TALLENNUS MUISTIIN ---
    // Päivitetään valitun huoneen tiedot vektorissa
    huoneet[valittuHuone - 1].onVarattu = true;
    huoneet[valittuHuone - 1].varaajanNimi = nimi;
    huoneet[valittuHuone - 1].varausNumero = varausID;
    huoneet[valittuHuone - 1].yot = yot;
    huoneet[valittuHuone - 1].aamupala = aamupala;
    huoneet[valittuHuone - 1].spa = spa;
    huoneet[valittuHuone - 1].alennusProsentti = alePros; //tallennetaan arvottu alennus jotta varauksen muokkaamisessa olisi myös mukana

    // Lasketaan lopullinen hinta
    double loppuSumma = laskeHinta(huoneet[valittuHuone - 1]);

    // KUITIN TULOSTUS
    cout << "\n" << V_VIHREA << "==================================" << endl;
    cout << "       VARAUKSEN KUITTI           " << endl;
    cout << "==================================" << V_RESET << endl;
    
    // Asiakkaan tiedot
    cout << " Varausnumero:          " << V_MAGENTA << V_LIHAVOITU << varausID << V_RESET << endl;
    cout << " Asiakas:           " << right << setw(5) << V_SYANI << nimi << V_RESET << endl;
    cout << " Huone:                 " << valittuHuone << " (" << huoneet[valittuHuone - 1].kapasiteetti << "hh)" << endl;
    cout << " Kesto:                 " << yot << " yötä" << endl;
    cout << V_ALLEVIIVAUS << "                                  " << V_RESET << endl;

    // Asetetaan tulostus näyttämään aina 2 desimaalia 
    cout << fixed << setprecision(2);
    // 1. Majoitus (Hinta keltaisella)
    double huoneHinta = (huoneet[valittuHuone - 1].kapasiteetti == 1) ? HINTA_1HH : HINTA_2HH;
    cout << left << setw(20) << "Majoitus" << right << setw(9) << V_KELTAINEN << (huoneHinta * yot) << " €" << V_RESET << endl;

    // 2. Aamupala
    if (aamupala) {
        cout << left << setw(20) << "Aamupala" << right << setw(9) << V_KELTAINEN << (HINTA_AAMUPALA * yot) << " €" << V_RESET << endl;
    }

    // 3. Spa
    if (spa) {
        cout << left << setw(20) << "Spa-osasto" << right << setw(9) << V_KELTAINEN << (double)HINTA_SPA << " €" << V_RESET << endl;
    }

    // 4. Alennus (Punaisella, jotta erottuu vähennyksenä)
    if (alePros > 0) {
        double normiHinta = (huoneHinta * yot) + (aamupala ? HINTA_AAMUPALA * yot : 0) + (spa ? HINTA_SPA : 0);
        double alennusEuroina = normiHinta - loppuSumma;
        
        cout << left << setw(20) << "Alennus" << right << setw(9) << V_PUNAINEN << "-" << alennusEuroina << " €" << V_RESET << endl;
        cout << V_MAGENTA << " (Kampanja-alennus -" << alePros << "%)" << V_RESET << endl;
    }

    cout << "----------------------------------" << endl;
    
    // Loppusumma
    cout << left << setw(20) << "YHTEENSÄ" << right << setw(9) << V_KIRKAS_VIH << V_LIHAVOITU << loppuSumma << " €" << V_RESET << endl;
    
    // Alatunniste 
    cout << V_KIRKAS_VAL << "==================================" << V_RESET << endl;

    odotaEnter();
}

// Hakee varauksia joko ID:n tai nimen perusteella
void etsiVaraus(const vector<Huone>& huoneet) {
    tyhjennaRuutu();
    cout << " -~- " << V_SYANI << "ETSI VARAUS" << V_RESET << V_LIHAVOITU << " -~- " << endl;
    cout << "[" << V_SININEN << "1" << V_RESET << "] Etsi varausnumerolla\n[" << V_SININEN << "2" << V_RESET << "] Etsi nimellä\n[" << V_SININEN << "0" << V_RESET << "] Palaa\n";
    int tapa = lueKokonaisluku(0, 2, "Valinta: ");
    if (tapa == 0) return;
    
    bool loytyi = false;
    if (tapa == 1) {
        // Haku varausnumerolla
        int id = lueKokonaisluku(10000, 99999, "Anna varausnumero: ");
        for (const auto& h : huoneet) {
            if (h.onVarattu && h.varausNumero == id) {
                cout << V_VIHREA << "\nLöytyi!" << V_RESET << " Huone " << h.numero << ", Varaaja: " << V_KELTAINEN << h.varaajanNimi << V_RESET << endl;
                loytyi = true;
            }
        }
    } else {
        // Haku nimellä 
        cout << "Anna nimi: ";
        string haku;
        getline(cin, haku);
        for (const auto& h : huoneet) {
            if (h.onVarattu && h.varaajanNimi == haku) {
                cout << V_VIHREA << "\nLöytyi!" << V_RESET << " Huone " << h.numero << " (ID: " << V_KELTAINEN << h.varausNumero <<  V_RESET <<")" << endl;
                loytyi = true;
            }
        }
    }
    if (!loytyi) cout << V_PUNAINEN << "\nEi varausta." << V_RESET << endl;
}

// Varauksen muokkaus ja peruutus
void muokkaaVarausta(vector<Huone>& huoneet) {
    tyhjennaRuutu();
    cout << V_LIHAVOITU << " -~- " << V_SYANI << "MUOKKAA VARAUSTA" << V_RESET << V_LIHAVOITU << " -~- " << V_RESET << endl;
    cout << "[" << V_SININEN << "1" << V_RESET << "] Etsi Varausnumerolla" << endl;
    cout << "[" << V_SININEN << "2" << V_RESET << "] Etsi Nimellä" << endl;
    cout << "[" << V_SININEN << "0" << V_RESET << "] Palaa" << endl;
    
    int hakutapa = lueKokonaisluku(0, 2, "Valinta: ");
    if (hakutapa == 0) return;

    // Käytetään osoitinta huoneeseen, jotta voimme muokata alkuperäistä tietoa
    Huone* kohde = nullptr;

    if (hakutapa == 1) {
        // Haku numerolla
        int id = lueKokonaisluku(10000, 99999, "Anna varausnumero: ");
        for (auto& h : huoneet) {
            if (h.onVarattu && h.varausNumero == id) {
                kohde = &h;
                break;
            }
        }
        if (!kohde) {
            cout << V_PUNAINEN << "Varausta ei löytynyt." << V_RESET << endl;
            odotaEnter(); return;
        }
    } else {
        // Haku nimellä - Ottaa huomioon samannimiset
        cout << "Anna nimi: ";
        string nimi;
        getline(cin, nimi);
        
        // Kerätään kaikki samalla nimellä olevat osoittimet vektoriin
        vector<Huone*> loydetyt;
        for (auto& h : huoneet) {
            if (h.onVarattu && h.varaajanNimi == nimi) {
                loydetyt.push_back(&h);
            }
        }
        
        if (loydetyt.empty()) {
            cout << V_PUNAINEN << "Ei varauksia nimellä " << nimi << V_RESET << endl;
            odotaEnter(); return;
        } else if (loydetyt.size() == 1) {
            kohde = loydetyt[0]; // Jos vain yksi, valitaan se suoraan
        } else {
            // Jos useita, pyydetään käyttäjää valitsemaan listasta
            cout << "\nLöytyi useampi varaus nimellä " << nimi << ":" << endl;
            for (size_t i = 0; i < loydetyt.size(); ++i) {
                cout << i + 1 << ". Huone " << loydetyt[i]->numero 
                     << " (ID: " << loydetyt[i]->varausNumero << ")" << endl;
            }
            int valinta = lueKokonaisluku(1, loydetyt.size(), "Valitse muokattava varaus (numero): ");
            kohde = loydetyt[valinta - 1];
        }
    }

    // Muokkausvalikko loopissa, kunnes käyttäjä poistuu
    while (true) {
        tyhjennaRuutu();
        cout << V_VIHREA << "Muokataan: " << kohde->varaajanNimi << " (Huone " << kohde->numero << ")" << V_RESET << endl;
        cout << "Tiedot: " << kohde->yot << " yötä. ";
        
        // Näytetään aktiivinen alennusprosentti
        if (kohde->alennusProsentti > 0) {
            cout << V_SYANI << "(ALENNUS: -" << kohde->alennusProsentti << "%)" << V_RESET;
        } else {
            cout << "(Ei alennusta)";
        }
        
        cout << "\nPalvelut: " << (kohde->aamupala ? "[Aamupala] " : "") << (kohde->spa ? "[Spa]" : "") << endl;
        
        // Lasketaan ja näytetään hinta reaaliajassa
        double lopullinen = laskeHinta(*kohde);
        if (kohde->alennusProsentti > 0) {
            // Lasketaan teoreettinen normaalihinta vertailun vuoksi
            double ovh = lopullinen / ((100.0 - kohde->alennusProsentti) / 100.0);
            if (!kohde->spa) ovh = (kohde->yot * ((kohde->kapasiteetti==1?HINTA_1HH:HINTA_2HH) + (kohde->aamupala?HINTA_AAMUPALA:0)));
            
            cout << "Normaalihinta: " << ovh << " €" << endl;
            cout << "Alennettu hinta: " << V_LIHAVOITU << lopullinen << " €" << V_RESET << endl;
        } else {
            cout << "Hinta yhteensä: " << V_LIHAVOITU << lopullinen << " €" << V_RESET << endl;
        }

        cout << "-------------------------" << endl;
        cout << "[" << V_SININEN << "1" << V_RESET << "] Muuta öiden määrää" << endl;
        // Dynaaminen teksti riippuen nykytilasta (Lisää/Poista)
        cout << "[" << V_SININEN << "2" << V_RESET << "] " << (kohde->aamupala ? "Poista aamupala" : "Lisää aamupala") << endl;
        cout << "[" << V_SININEN << "3" << V_RESET << "] " << (kohde->spa ? "Poista Spa" : "Lisää Spa") << endl;
        cout << "[" << V_SININEN << "5" << V_RESET << "] " << V_PUNAINEN << "PERUUTA varaus" << V_RESET << endl;
        cout << "[" << V_SININEN << "0" << V_RESET << "] Palaa" << endl;

        int mValinta = lueKokonaisluku(0, 4, "> ");

        if (mValinta == 0) break; // Poistutaan muokkauksesta
        
        if (mValinta == 1) kohde->yot = lueKokonaisluku(1, 365, "Anna uusi öiden määrä: ");
        else if (mValinta == 2) kohde->aamupala = !kohde->aamupala; // Vaihdetaan arvoa true tai false
        else if (mValinta == 3) kohde->spa = !kohde->spa;
        else if (mValinta == 4) {
            // Peruutuksen varmistus
            cout << "Vahvista peruutus (1=Kyllä, 0=Ei): ";
            if (lueKokonaisluku(0, 1, "> ") == 1) {
                // Nollataan huoneen tiedot
                kohde->onVarattu = false;
                kohde->varaajanNimi = "";
                kohde->varausNumero = 0;
                kohde->yot = 0;
                kohde->aamupala = false;
                kohde->spa = false;
                kohde->alennusProsentti = 0;
                cout << V_VIHREA << "Varaus peruttu." << V_RESET << endl;
                odotaEnter();
                return;
            }
        }
    }
}

// Listaa huoneiden statuksen
void listaaHuoneet(const vector<Huone>& huoneet) {
    tyhjennaRuutu();
    cout << "--- HUONETILANNE ---" << endl;
    int vapaana = 0;
    for (const auto& h : huoneet) {
        if (!h.onVarattu) vapaana++;
        cout << "Huone " << left << setw(3) << h.numero << " (" << h.kapasiteetti << "hh): ";
        if (h.onVarattu) {
            cout << V_PUNAINEN << "VARATTU" << V_RESET << " [" << h.varaajanNimi << "]";
            if (h.aamupala) cout << " +Aamu";
            if (h.spa) cout << " +Spa";
        } else {
            cout << V_VIHREA << "Vapaa" << V_RESET;
        }
        cout << endl;
    }
    cout << "\nVapaita: " << vapaana << " / " << huoneet.size() << endl;
}