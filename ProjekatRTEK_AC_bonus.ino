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

   Takođe, moguće je preko Serial Monitora-a učitati dve vrednosti, amplitudu signala kojeg puštamo preko Function Generatora (Ampl) i njegovu 
   frekvenciju u hercima (Freq), u razmaku od 3 sekunde kako bi Arduino mogao da ih pročita
   Funkcija F=Ampl*sin(2*pi*Freq*n*Ts) gde:n-broj tačke koju generišemo, Ts=1/Freq_Ardu gde Freq_Arduino-frekvenicija kojom Arduino izbacuje 
   vrednosti (cca 9600Hz) formira idealan signal. Efektivnu vrednost tako dobijenog signala (V_eff_real)upoređujemo sa onom očitanom sa analognog 
   ulaza uz pomoć formule Rel_Error=|V_eff_real-V_eff|/(V_eff)*100% i to konačno ispisujemo na Serial izlazu
*/

#include<LiquidCrystal.h>

//definišemo i deklarišemo gore navedene vrednosti
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define N 1000
#define pi 3.14
#define Freq_Ardu 9600
float R1=1.57,R2=8.63,R3=5.13,R4=5.09,gain,offset,V_ref=5;
int V_out_i[N]; // vrednosti napona izražene bitovski (ali u decimalnom obliku)
int V_max_i; //maksimalna vrednost napona izražena bitovski (takođe u decimalnom obliku)
float V_max,V_eff;
float sum; //promenljiva koja će služiti za skladištenje sume koju koristimo u formuli za efektivnu vrednost
float V; //vrednost napona koju dobijemo kada onu iz niza V_out_i skaliramo iz celog domena u decimalni, a nakon toga još jednom skaliramo uz 
         //pomoć formule iz gornjeg objašnjenja, na željeni opseg
float Ampl,Freq,V_eff_real,sum_real,V_real,Ts,Rel_Error;
int par=0; //promenljiva koja obezbeđuje da pratimo da li se učitava prva (Ampl) ili druga (Freq) vrednost sa Serial-a
int pin=7; //numeracija analognog pina sa kojeg čitamo vrednosti
int garbage_cnt=0; // primeljiva koja broji da li smo prešli prvih 1000 vrednosti; među njima može da se nađe neka neočekivana
int garbage=1000; //broj vrednosti koje čitamo sa analognog ulaza među kojima može da se pronađe neka neočekivana
int in_delay=3000; //dužina perioda u kojem moramo učitati vrednosti preko Seriala

//inicijalizujemo LCD i brzinu komunikacije Arduina sa serijskim izlazom
void setup() {
  gain=-(R2/R1);
  offset=V_ref*(R4/(R3+R4));
  Ts=1/Freq_Ardu;
  Serial.begin(115200);
  lcd.begin(16, 2);
}

//u svakoj iteraciji loop-a prvo učitamo jedan blok podataka veličine , a zatim računamo efektivnu vrednost napona koji se nalaze u njemu, V_eff,
//kao i njegovu maksimalnu vrednost, V_max; njihovu vrednost na kraju ispisujemo na LCD-u
//dodatno, u ovom kodu u dve uzastopne iteracije petlje loop čitamo amplitudu i frekvenciju generičkog signala, računamo njegovu efektivnu vrednost
//i ispisujemo relativnu grešku očitane efektivne vrednosti sa ovom, idealnom
void loop() {
    V_max_i=-1;
    for(int i=0;i<N;i++) V_out_i[i]=analogRead(A7);
    while (Serial.available()==0){}       
    if (par==0){
      Ampl = Serial.parseFloat(); 
      par=1;
      }  
    else 
    {
      Freq=Serial.parseFloat();
      par=0;
      sum_real=0;
      for (int i=1;i<=N;i++)
      {
        V_real=Ampl*sin(2*pi*Freq*i*Ts);
        sum_real+=V_real*V_real;
      }
      V_eff_real=sqrt(sum_real/N);
    }
    for(int i=0;i<N;i++) 
      if(V_out_i[i]>V_max_i) 
        V_max_i=V_out_i[i];
    sum = 0;
    for (int i = 0; i < N; i++)
    {
      V = ((V_out_i[i] * (5.0 / 1023.0)) - offset) * (1/gain);
      sum += V * V;
    }
    V_eff=sqrt(sum/N);
    if (par==0)
    {
      Rel_Error=abs(V_eff-V_eff_real)/V_eff_real*100;
      Serial.println(String("Efektivna vrednost generičkog signala: ")+String(V_eff_real));
      Serial.println(String("Efektivna vrednost očitanog signala: ")+String(V_eff));
      Serial.println(String("Relativna greška: ")+String(Rel_Error)+String("%"));
    }
    V_max=((V_max_i * (5.0 / 1023.0)) - offset) * (1/gain);
    lcd.setCursor(0, 0);
    lcd.print(V_eff);
    lcd.setCursor(0, 1);
    lcd.print(V_max);
    delay(in_delay);
  
}
