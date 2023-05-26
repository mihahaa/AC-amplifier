/*
 * U ovom programu ideja je da sa ulaza analognog ulaza Arduina, A7, čitamo vrednosti koje operacionim ampermetrom (OpAmp) uvećavamo 
 * tako da one pripadaju vrednostima napona između 0V i 5V. Te vrednosti nakon čitanja skaliramo nazad uz pomoć formule
 * V_out=gain*V_in+offset
 * gde: Vout-napon koji čitamo Arduinom, gain-mera uvećanja jačine signala, offset-vrednost napona za koliko ćemo signal "podići" ili "spustiti"
 * na osi, V_in-vrednost napona kojeg hoćemo da izmerimo (napon audio signala)
 * Vrednost gain se računa kao gain=-R2/R1, a offset kao offset=V_ref*(R4/(R3+R4)), gde su R1, R2, R3, R4 otpornosti označene kao u prilogu, a 
 * dobijene pomoću potenciometra, a napon V_ref je takođe napon označen kao u prilogu.
 * 
 * Vrednosti napona V_in mogu se očitavati na nečemu sličnom osciloskopu, Serial Plotter, ili numerički, pomoću Serial Monitora-a 
 */


//deklarišemo i definišemo gore navedene vrednosti
float V_ref=5,R1=,R2=,R3=,R4=,gain,offset,V_out,V_in;
int pin; //numeracija pina sa kog hoćemo da čitamo
int sensorValue; //vrednost koju čitamo sa analognog pina

//inicijalizujemo brzinu komunikacije Arduina sa serijskim izlazom
void setup() {
  gain=-(R2/R1);
  offset=V_ref*(R4/(R3+R4));
  Serial.begin(115200);
}

//u svakoj iteraciji petlje čitamo vrednost sa analognog izlaza, transformišemo je i na kraju ispisujemo
void loop() {
  sensorValue = analogRead(pin);
  V_out=sensorValue*(5.0/1023);
  V_in=(V_out-offset)*(1/gain);
  Serial.println(V_in);
}
