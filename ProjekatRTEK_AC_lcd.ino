/*
   U ovom programu ideja je da sa ulaza analognog ulaza Arduina, A7, čitamo vrednosti koje operacionim ampermetrom (OpAmp) uvećavamo
   tako da one pripadaju vrednostima napona između 0V i 5V. Te vrednosti nakon čitanja skaliramo nazad uz pomoć formule
   V_out=gain*V_in+offset
   gde: Vout-napon koji čitamo Arduinom, gain-mera uvećanja jačine signala, offset-vrednost napona za koliko ćemo signal "podići" ili "spustiti"
   na osi, V_in-vrednost napona kojeg hoćemo da izmerimo (napon audio signala)
   Vrednost gain se računa kao gain=-R2/R1, a offset kao offset=V_ref*(R4/(R3+R4)), gde su R1, R2, R3, R4 otpornosti označene kao u prilogu, a
   dobijene pomoću potenciometra, a napon V_ref je takođe napon označen kao u prilogu.
   Ovako dobijene vrednosti V_in delimo u blokove veličine  i računamo efektivnu (V_eff=sqrt((1/n)*sum((x_i)^2)), gde: n-veličina bloka, x_i-
   vrednosti napona u bloku) i maksimalnu vrednost napona koji se nalaze u tom bloku.

   Na LCD-u (lcd) ispisujemo efektivnu i maksimalnu vrednost dobijene na način prikazan u prošlom pasusu.
*/

#include<LiquidCrystal.h>

//definišemo i deklarišemo gore navedene vrednosti
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define N ;
float R1=,R2=,R3=,R4=,gain,offset,V_ref=5;
int V_out_i[N]; // vrednosti napona izražene bitovski (ali u decimalnom obliku)
int V_max_i; //maksimalna vrednost napona izražena bitovski (takođe u decimalnom obliku)
float V_max,V_eff;
float sum; //promenljiva koja će služiti za skladištenje sume koju koristimo u formuli za efektivnu vrednost
float V; //vrednost napona koju dobijemo kada onu iz niza V_out_i skaliramo iz celog domena u decimalni, a nakon toga još jednom skaliramo uz 
         //pomoć formule iz gornjeg objašnjenja, na željeni opseg

//inicijalizujemo LCD i brzinu komunikacije Arduina sa serijskim izlazom
void setup() {
  gain=-(R2/R1);
  offset=V_ref*(R4/(R3+R4));
  Serial.begin(115200);
  lcd.begin(16, 2);
}

//u svakoj iteraciji loop-a prvo učitamo jedan blok podataka veličine , a zatim računamo efektivnu vrednost napona koji se nalaze u njemu, V_eff,
//kao i njegovu maksimalnu vrednost, V_max; njihovu vrednost na kraju ispisujemo na LCD-u
void loop() {
  V_max_i=-1;
  for(int i=0;i<N;i++) V_out[i]=analogRead(A7);
  for(int i=0;i<N;i++) 
    if(V_out[i]>V_max_i) 
      V_max_i=V_out[i];
  sum = 0;
  for (int i = 0; i <= i; t++)
  {
    V = ((V_out_i[i] * (5.0 / 1023.0)) - offset) * (1/gain);
    sum += V * V;
  }
  V_eff=sqrt(sum/N);
  V_max=((V_max_i * (5.0 / 1023.0)) - offset) * (1/gain);
  lcd.setCursor(0, 0);
  lcd.print("V_eff="+V_eff);
  lcd.setCursor(0, 1);
  lcd.print("V_max="+V_max);
}
