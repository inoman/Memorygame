
#define VALINTA_OFF      0 
#define VALINTA_NONE     0 
#define VALINTA_RED  (1 << 0)
#define VALINTA_GREEN    (1 << 1)
#define VALINTA_BLUE (1 << 2)
#define VALINTA_YELLOW   (1 << 3)

#define LED_RED     10
#define LED_GREEN   3
#define LED_BLUE    13
#define LED_YELLOW  5


#define NAPPI_RED    9
#define NAPPI_GREEN  2
#define NAPPI_BLUE   12
#define NAPPI_YELLOW 6

#define BUZZER1  4
#define BUZZER2  7


#define KIERROKSET_VOITTO      13 //KIERROSTEN MÄÄRÄ, JONKA JÄLKEEN PELIN VOITTAA
#define AIKARAJA   3000 //AIKA PAINAA NAPPIA, JOS MENEE YLI 3 SEKUNTIA PELI LOPETETAAN

#define MUISTIPELI  0

byte pelimuoto = MUISTIPELI; //PELIMUOTO MÄÄRITELLÄÄN
byte jarjestus[32]; //TÄHÄN TALLETETAAN PELIN AIKANA SYNTYNYT JÄRJESTYS
byte kierrokset = 0; //KIERROKSET, JOILTA PÄÄSTY LÄPI

void setup()
{
  pinMode(NAPPI_RED, INPUT_PULLUP);
  pinMode(NAPPI_GREEN, INPUT_PULLUP);
  pinMode(NAPPI_BLUE, INPUT_PULLUP);
  pinMode(NAPPI_YELLOW, INPUT_PULLUP);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  pinMode(BUZZER1, OUTPUT);
  pinMode(BUZZER2, OUTPUT);

  // TARKISTAA PAINETAANKO JOTAIN NAPPIA
  if (tarkista_nappi() == VALINTA_GREEN | VALINTA_RED | VALINTA_BLUE | VALINTA_YELLOW)
  {
    pelimuoto = MUISTIPELI; //PELI PÄÄLLE

    setLEDs(VALINTA_RED | VALINTA_BLUE | VALINTA_YELLOW | VALINTA_GREEN); // KAIKKI LEDIT PÄÄLLE KUN PAINETAAN JOTAIN NAPPIA

    while(tarkista_nappi() != VALINTA_NONE) ; // ODOTTAA, ETTÄ PELAAJA LOPETTAA NAPIN PAINAMISEN

  }

  soita_voitto(); // KUN ALOITTAA PELIN, SOITTAA MUSIIKIN
}

void loop()
{
  idle(); // VILKUTTAA LEDEJÄ, KUN ODOTTAA PELAAJAN PAINAVAN NAPPIA

  // KUN PELI ALOITETAAN, LAITTAA HETKEKSI KAIKKI LEDIT PÄÄLLE
  setLEDs(VALINTA_RED | VALINTA_GREEN | VALINTA_BLUE | VALINTA_YELLOW); 
  delay(1000);
  setLEDs(VALINTA_OFF); // POIS PÄÄLTÄ
  delay(250);

  if (pelimuoto == MUISTIPELI)
  {
    //VOITETTIINKO VAI HÄVITTIIN
    if (peli() == true) 
      soita_voitto(); // PELAAJA VOITTI, SOITTAA VOITON MUSIIKIN
    else 
      havionmusa(); // PELAAJA HÄVISI, SOITTAA HÄVIÖN MUSIIKIN
  }
}


// PALAUTTAA ARVON 1 JOS PELAAJA VOITTAA, 0 JOS PELAAJA HÄVIÄÄ
boolean peli(void)
{
  randomSeed(millis()); 

  kierrokset = 0; // NOLLAA KIERROKSEN

  while (kierrokset < KIERROKSET_VOITTO) 
  {
    lisaa_nappi(); // LISÄÄ YHDEN NAPIN LISÄÄ JA TOISTAA VANHAN JÄRJESTYKSEN

    toistanapit(); // TOISTAA PELIÄ

    // TARKISTAA TOISTAAKO PELAAJA JÄRJESTYKSEN OIKEIN
    for (byte nykyinen = 0 ; nykyinen < kierrokset ; nykyinen++)
    {
      byte choice = painallus(); // TARKISTAA MITÄ NAPPIA PELAAJA PAINAA

      if (choice == 0) return false; // PELI LOPPUU JOS EI PAINA MITÄÄN

      if (choice != jarjestus[nykyinen]) return false; // JOS JÄRJESTYS ON VÄÄRÄ PELIN HÄVIÄÄ
    }

    delay(1000); // SEKUNNIN VIIVE ENNEN SEURAAVAN KIERROKSEN ALKUA
  }

  return true; // PELATTIIN 13 KIERROSTA --> VOITTO
}

void toistanapit(void)
{
  for (byte nykyinen = 0 ; nykyinen < kierrokset ; nykyinen++) 
  {
    aani_painettaessa(jarjestus[nykyinen], 150);

    delay(150); // AIKA VALOJEN NÄYTTÄMISEN VÄLISSÄ
  }
}

// LISÄÄ UUDEN NAPIN JÄRJESTYKSEEN KÄYTTÄMÄLLÄ AJASTINTA 
void lisaa_nappi(void)
{
  byte uusiNappi = random(0, 4);

  //MUUTETAAN NUMEROT VALINNOIKSI
  if(uusiNappi == 0) uusiNappi = VALINTA_RED;
  else if(uusiNappi == 1) uusiNappi = VALINTA_GREEN;
  else if(uusiNappi == 2) uusiNappi = VALINTA_BLUE;
  else if(uusiNappi == 3) uusiNappi = VALINTA_YELLOW;

  jarjestus[kierrokset++] = uusiNappi; // LISÄÄ TÄMÄN UUDEN NAPIN LISTAAN, JOSTA PELIÄ TOISTETAAN
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// SYTYTTÄÄ ANNETUN LEDIN AIEMMIN VALITUN VALINNAN MUKAAN
void setLEDs(byte leds)
{
  if ((leds & VALINTA_RED) != 0)
    digitalWrite(LED_RED, HIGH);
  else
    digitalWrite(LED_RED, LOW);

  if ((leds & VALINTA_GREEN) != 0)
    digitalWrite(LED_GREEN, HIGH);
  else
    digitalWrite(LED_GREEN, LOW);

  if ((leds & VALINTA_BLUE) != 0)
    digitalWrite(LED_BLUE, HIGH);
  else
    digitalWrite(LED_BLUE, LOW);

  if ((leds & VALINTA_YELLOW) != 0)
    digitalWrite(LED_YELLOW, HIGH);
  else
    digitalWrite(LED_YELLOW, LOW);
}
 // palauttaa painetun napin
byte painallus(void)
{
  long alkuaika = millis(); // AJASTIN PÄÄLLE AINA NÄYTETYN JÄRJESTYKSEN JÄLKEEN

  while ( (millis() - alkuaika) < AIKARAJA) // ODOTTAA PELAAJAN VALINTAA KUNNES AIKAA KULUU LIIKAA (> 3 s)
  {
    byte nappi = tarkista_nappi();

    if (nappi != VALINTA_NONE)
    { 
      aani_painettaessa(nappi, 150); // ÄÄNI NAPPIA PAINETTAESSA

      while(tarkista_nappi() != VALINTA_NONE) ;

      delay(10); //

      return nappi;
    }

  }

  return VALINTA_NONE; // JOS AIKAA KULUU LIIKAA, PALAUTTAA TÄMÄN
}

// PALAUTTAA PAINETUN VALINNAN
byte tarkista_nappi(void)
{
  if (digitalRead(NAPPI_RED) == 0) return(VALINTA_RED); 
  else if (digitalRead(NAPPI_GREEN) == 0) return(VALINTA_GREEN); 
  else if (digitalRead(NAPPI_BLUE) == 0) return(VALINTA_BLUE); 
  else if (digitalRead(NAPPI_YELLOW) == 0) return(VALINTA_YELLOW);

  return(VALINTA_NONE); // PALAUTETAAN JOS MITÄÄN NAPPIA EI PAINETA 
}

void aani_painettaessa(byte y, int buzz_pituusms)
{
  setLEDs(y); 

  //SOITTAA ÄÄNEN NAPPIA PAINETTAESSA
  switch(y) 
  {
  case VALINTA_RED:
    buzz_aani(buzz_pituusms, 1136); 
    break;
  case VALINTA_GREEN:
    buzz_aani(buzz_pituusms, 568); 
    break;
  case VALINTA_BLUE:
    buzz_aani(buzz_pituusms, 851); 
    break;
  case VALINTA_YELLOW:
    buzz_aani(buzz_pituusms, 638); 
    break;
  }

  setLEDs(VALINTA_OFF);
}

void buzz_aani(int buzz_pituusms, int buzz_viive)
{
  long buzz_pituusus = buzz_pituusms * (long)1000;

  while (buzz_pituusus > (buzz_viive * 2))
  {
    buzz_pituusus -= buzz_viive * 2;

    digitalWrite(BUZZER1, LOW);
    digitalWrite(BUZZER2, HIGH);
    delayMicroseconds(buzz_viive);

    digitalWrite(BUZZER1, HIGH);
    digitalWrite(BUZZER2, LOW);
    delayMicroseconds(buzz_viive);
  }
}


void soita_voitto(void)
{
  setLEDs(VALINTA_GREEN | VALINTA_BLUE);
  voitonmusa();
  setLEDs(VALINTA_RED | VALINTA_YELLOW);
  voitonmusa();
  setLEDs(VALINTA_GREEN | VALINTA_BLUE);
  voitonmusa();
  setLEDs(VALINTA_RED | VALINTA_YELLOW);
  voitonmusa();
}

// VOITON MUSIIKKI!!!
void voitonmusa(void)
{
  for (byte x = 250 ; x > 40 ; x--)
  {
    for (byte y = 0 ; y < 2 ; y++)
    {
      digitalWrite(BUZZER2, HIGH);
      digitalWrite(BUZZER1, LOW);
      delayMicroseconds(x);

      digitalWrite(BUZZER2, LOW);
      digitalWrite(BUZZER1, HIGH);
      delayMicroseconds(x);

      digitalWrite(BUZZER2, LOW);
      digitalWrite(BUZZER1, HIGH);
      delayMicroseconds(x);

      digitalWrite(BUZZER2, LOW);
      digitalWrite(BUZZER1, HIGH);
      delayMicroseconds(x);
      
    }
  }
}

// HÄVIÖN MUSIIKKI!
void havionmusa(void)
{
  setLEDs(VALINTA_RED | VALINTA_GREEN);
  buzz_aani(255, 1500);

  setLEDs(VALINTA_BLUE | VALINTA_YELLOW);
  buzz_aani(255, 1500);

  setLEDs(VALINTA_RED | VALINTA_GREEN);
  buzz_aani(255, 1500);

  setLEDs(VALINTA_BLUE | VALINTA_YELLOW);
  buzz_aani(255, 1500);
}

// VÄLKYTTELEE LEDEJÄ; KUN ODOTETAAN PELIN ALKUA
void idle(void)
{
  while(1) 
  {
    setLEDs(VALINTA_RED);
    delay(100);
    if (tarkista_nappi() != VALINTA_NONE) return;

    setLEDs(VALINTA_BLUE);
    delay(100);
    if (tarkista_nappi() != VALINTA_NONE) return;

    setLEDs(VALINTA_GREEN);
    delay(100);
    if (tarkista_nappi() != VALINTA_NONE) return;

    setLEDs(VALINTA_YELLOW);
    delay(100);
    if (tarkista_nappi() != VALINTA_NONE) return;
  }
}
