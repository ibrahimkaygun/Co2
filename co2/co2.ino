#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <stdio.h>
#include "DHT.h"
#define DHT_pin A1
#define DHT_tip DHT11
#define sag     0
#define yukari  1
#define asagi   2
#define sol     3
#define sec     4
#define yok     5
//----------------
#define RLOAD 1.0
#define RZERO 203.232
#include "MQ135.h"
MQ135 gasSensor = MQ135(A2);
float ppm;
byte yuzde;
//----------------
const int motor =  3;//------------
const int fan =  A3;
const int rezistans =  A4;
DHT dht(DHT_pin, DHT_tip);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
char sicaklik[] = "Sckl: 00.0   C   ";
char nem[]      = "Nem : 00.0   %   ";
int okunan_deger  = 0;
int basilan_tus   = 0;
int k = 0, i = 0, j = 0, f = 0, y = 0, cnt = 0;
byte a = 0, b = 0, a1 = 0, b1 = 0, a2 = 0, b2 = 0, adr = 0;
byte x1 = 3, x2 = 2;
byte RH, Temp;
void setup() {
  //------------------------
  ///pinMode(sensorPin, INPUT);
  //------------------------
  pinMode(fan, OUTPUT);
  pinMode(rezistans, OUTPUT);
  digitalWrite(fan, LOW);
  lcd.begin(16, 2);
  dht.begin();
  pinMode(A3, OUTPUT);
  a = EEPROM.read(0);
  gecikme(5);
  b = EEPROM.read(1);
  gecikme(5);
  a1 = EEPROM.read(2);
  gecikme(5);
  b1 = EEPROM.read(3);
  gecikme(5);
  a2 = EEPROM.read(4);
  gecikme(5);
  b2 = EEPROM.read(5);
  gecikme(5);
  if (a == b || a < b) {
    b = a;
    a = a + 2;
  }
  if (a1 == b1 || a1 < b1) {
    b1 = a1;
    a1 = a1 + 3;
  }
  if (a2 == b2 || a2 < b2) {
    b2 = a2;
    a2 = a2 + 4;
  }
}
void gecikme(int gd) {
  delay(gd);
}
void buton_basili() {
  gecikme(50);
up:
  basilan_tus = buton_bul();
  if (basilan_tus == sag)goto up;
  if (basilan_tus == yukari)goto up;
  if (basilan_tus == asagi)goto up;
  if (basilan_tus == sol)goto up;
  if (basilan_tus == sec)goto up;
  gecikme(50);
}
void lcd_sil() {
  lcd.clear();
}

void sicaklik_nem_oku() {
  RH = dht.readHumidity();
  Temp = dht.readTemperature();
  if (isnan(RH) || isnan(Temp)) {
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("Hata");
    return;
  }
}


byte buton_bul() {
  gecikme(10);
  okunan_deger = analogRead(A0);
  if (okunan_deger > 1000) return yok;
  if (okunan_deger < 50)return sag;
  if (okunan_deger < 200)return yukari;
  if (okunan_deger < 360)return asagi;
  if (okunan_deger < 510)return sol;
  if (okunan_deger < 740)return sec;
}

//----------------------------------
int yuzdeye_cevir(float deger)
{
  int yuzde_degeri = 0;
  yuzde_degeri = map(deger, 0, 5000, 0, 100);
  return yuzde_degeri;
}

void CO2_oku() {
  ///val = analogRead(A6);
  float zero = gasSensor.getRZero();
  float getCorrectionFactor(Temp, RH);
  float correctedRZero = gasSensor.getCorrectedRZero(Temp, RH);
  float getCorrectedResistance(Temp, RH);
  //float ppm_1 = gasSensor.getPPM();
  float ppm_1 = gasSensor.getPPM();
  //float ppm_1 = gasSensor.getCorrectedPPM(Temp, RH);
  ppm = ppm_1;
  yuzde = yuzdeye_cevir(ppm_1);
  //lcd.setCursor(0, 0);
  //lcd.print("CO2:");
  //lcd.print(ppm);
  //lcd.setCursor(11, 0);
  //lcd.print("  ppm");
  //delay(1000);
}
//------------------------------------
void lcd_goruntule() {
  switch (k) {
    case 0: {
        sicaklik[6]     = Temp / 10 + 48;
        sicaklik[7]     = Temp % 10 + 48;
        sicaklik[14]    = 223;
        nem[6]        = RH / 10 + 48;
        nem[7]        = RH % 10 + 48;
        lcd.setCursor(0, 0);
        lcd.print(sicaklik);
        lcd.setCursor(0, 1);
        lcd.print(nem);
        break;
      }
    case 1: {
        nem[6]        = RH / 10 + 48;
        nem[7]        = RH % 10 + 48;
        lcd.setCursor(0, 0);
        lcd.print(yuzde);//-----------------------------------------------------------
        lcd.setCursor(0, 1);
        lcd.print("CO2 : ");
        lcd.print(ppm);
        lcd.setCursor(13, 1);
        lcd.print("ppm");
        break;
      }

    case 2: {
        lcd.setCursor(0, 0);
        lcd.print("      Ayar      ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        break;
      }

  }
}
void sinir_degistir(byte ust, byte alt, int ust_sinir, int alt_sinir) {
  buton_basili();
  lcd.setCursor(0, 0);
  lcd.print("Ust sinir: ");
  lcd.setCursor(0, 1);
  lcd.print("Alt sinir: ");
  do {
    lcd.setCursor(11, 0);
    lcd.print(ust / 10);
    lcd.print(ust % 10);
    lcd.setCursor(11, 1);
    lcd.print(alt / 10);
    lcd.print(alt % 10);
    basilan_tus = buton_bul();
    if (basilan_tus == yukari) {
      if (ust > alt && ust < ust_sinir)ust++;
      gecikme(100);
    }
    if (basilan_tus == asagi) {
      if (ust > alt && ust > alt_sinir && ust - alt != 1)ust--;
      gecikme(100);
    }
    if (basilan_tus == sag) {
      if (alt < ust && alt < ust_sinir && ust - alt != 1)alt++;
      gecikme(100);
    }
    if (basilan_tus == sol) {
      if (alt < ust && alt > alt_sinir)alt--;
      gecikme(100);
    }

  } while (basilan_tus != sec);
  x1 = ust;
  x2 = alt;
  buton_basili();

}
void lcd_ayar_goruntule() {
  switch (i) {
    case 1: {
        lcd.setCursor(0, 0);
        lcd.print("Sicaklik       ");
        lcd.setCursor(0, 1);
        lcd.print("Sinir Ayari    ");
        basilan_tus = buton_bul();
        if (basilan_tus == sec) {
          sinir_degistir(a, b, 50, 0 );
          lcd.setCursor(0, 0);
          lcd.print("Sicaklik        ");
          lcd.setCursor(0, 1);
          lcd.print("Sinir Ayari     ");
          a = x1;
          b = x2;
          EEPROM.write(0, a);
          gecikme(5);
          EEPROM.write(1, b);
          gecikme(5);
        }
        break;
      }
    case 2: {
        lcd.setCursor(0, 0);
        lcd.print("Nem              ");
        lcd.setCursor(0, 1);
        lcd.print("Sinir Ayari     ");
        basilan_tus = buton_bul();
        if (basilan_tus == sec) {
          sinir_degistir(a1, b1, 80, 20 );
          lcd.setCursor(0, 0);
          lcd.print("Nem              ");
          lcd.setCursor(0, 1);
          lcd.print("Sinir Ayari     ");
          a1 = x1;
          b1 = x2;
          EEPROM.write(2, a1);
          gecikme(5);
          EEPROM.write(3, b1);
          gecikme(5);
        }
        break;
      }
    case 3: {
        lcd.setCursor(0, 0);
        lcd.print("CO2             ");
        lcd.setCursor(0, 1);
        lcd.print("Sinir Ayari     ");
        basilan_tus = buton_bul();
        if (basilan_tus == sec) {
          sinir_degistir(a2, b2, 99, 0 );
          lcd.setCursor(0, 0);
          lcd.print("CO2        ");
          lcd.setCursor(0, 1);
          lcd.print("Sinir Ayari     ");
          a2 = x1;
          b2 = x2;
          EEPROM.write(4, a2);
          gecikme(5);
          EEPROM.write(5, b2);
          gecikme(5);
        }
        break;
      }

    case 4: {
        lcd.setCursor(0, 0);
        lcd.print("      Geri      ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        break;
      }
  }
}
void menu() {
  if (basilan_tus == yukari) {
    if (k < 2) k++;
    lcd_sil();
    lcd_goruntule();
    buton_basili();
  }
  if (basilan_tus == asagi) {
    if (k > 0) k--;
    lcd_sil();
    lcd_goruntule();
    buton_basili();
  }
  if (basilan_tus == sec && k == 2) {
    buton_basili();
    i = 1;
    lcd_ayar_goruntule();
    do {
      basilan_tus = buton_bul();
      lcd_ayar_goruntule();
      if (basilan_tus == yukari) {
        if (i < 4) i++;
        lcd_ayar_goruntule();
        buton_basili();
      }
      if (basilan_tus == asagi) {
        if (i > 1) i--;
        lcd_ayar_goruntule();
        buton_basili();
      }

    } while (basilan_tus != sec && k != 4);
    buton_basili();
    k = 0;
  }

}


void loop() {
  if (f == 150) {
    sicaklik_nem_oku();
    CO2_oku();
    lcd_goruntule();
    f = 0;
  }
  gecikme(10);
  f++;
  if (yuzde <= b2) digitalWrite(fan, LOW);
  if (yuzde >= a2) digitalWrite(fan, HIGH);  
  if (Temp < b) digitalWrite(rezistans, HIGH);
  if (Temp > a) digitalWrite(rezistans, LOW);
  basilan_tus = buton_bul();
  if (basilan_tus != yok) menu();
}



