#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#include <EEPROM.h>
#include <Keypad.h>
#include "RTClib.h"
RTC_DS3231 RTC;

const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5}; 
byte colPins[COLS] = {6, 7, 8, 9}; 

int Movcontador, Movcontador2;
int SerialpH, SerialTemperatura;
int opcion, anio, mes, dia, hora, minuto, segundo, temperatura, ContandorSensores, ValSen, Dato, largo, led, valor, valorAlmacenar, BTN, ContadorRegado, SelectorRegador, HorarioRiego, Editar;
int TipoRiego, contHoras, contMinutos;
int SensorPH = A2, Po, RangoPH;
int SensorHumedad = A3, humedad, PorcentajeHumedad;
String cocatenarNumeros="";
const int Relay = 10;
char ValorBTN;

String Menu1[7]= {"Visual", "HumCal", "PHCal", "Reloj", "Riego", "BTNCod", "    "};
String Visual[5]= {"Modo Visual:", "Temper", "Humedad", "PH", "    "};
String HumCal[4]= {"Calibrar hum:", "Actual", "Nuevo", "    "};
String PHCal[4]= {"Calibrar PH:", "actual", "nuevo", "    "};
String Reloj[5]= {"Reloj actual:", "HMS", "MD", "A", "    "};
String Riego[7]= {"Sistema Riego:", "Riego1", "Riego2", "Riego3", "Riego4", "Auto", "    "};
String BTNCod[3]= {"Boton codigo:", "B: ", "    "};



byte CursorDerecha[8] = {
  B01000, //  *
  B01100, //  **
  B01110, //  ***
  B01111, //  ****
  B01110, //  ***
  B01100, //  **
  B01000, //  *
  B00000  //
};

byte CursorIzquierda[8] = {
  B00010, //   *
  B00110, //  **
  B01110, // ***
  B11110, //****
  B01110, // ***
  B00110, //  **
  B00010, //   *
  B00000  //
};

byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b11111, // *****
  0b01110, //  ***
  0b01110, //  ***
  0b01110, //  ***
  0b01110, //  ***
  0b01110  //  ***
};

byte downArrow[8] = {
  0b01110, //  ***
  0b01110, //  ***
  0b01110, //  ***
  0b01110, //  ***
  0b01110, //  *** 
  0b11111, // *****
  0b01110, //  ***
  0b00100  //   *
};

byte grados[] = {
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  Serial.begin(9600);
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__)); 
  pinMode(Relay,OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.createChar(0, CursorDerecha);
  lcd.createChar(1, CursorIzquierda);
  lcd.createChar(2, upArrow);
  lcd.createChar(3, downArrow);
  lcd.createChar(4, grados);
}

int agregar_numeros(int keypressed, int valormaximo)
{
      
  
      if (Dato ==0)
      { 
        Dato = keypressed;
      }
      else
      {
        Dato = (Dato*10) + keypressed;
      }

      
      if(keypressed == 10){
        cocatenarNumeros="";
        Dato = 0; 
        Editar = 0;       
      }else if(Dato <= valormaximo && Dato != 0){
        cocatenarNumeros += keypressed;
        lcd.print(cocatenarNumeros);
      }

      if(Dato > valormaximo)
      {
        Dato =(Dato - keypressed)/10;
      }    

      if(keypressed == 11)
      {

        if(TipoRiego == 0)
        {
          opcion = 0;
          EEPROM.put(valorAlmacenar, Dato);
          cocatenarNumeros="";
          Dato = 0;
          Editar = 0;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Dato almacenado");
          lcd.setCursor(0, 1);
          lcd.print("correctamente!!");
          delay(2000);
        }
        else
        {
          opcion = 0;
          Editar = 0;
          cocatenarNumeros="";
          contHoras = Dato;
          contMinutos = Dato;
          Dato = contHoras/100;
          EEPROM.put(valorAlmacenar, Dato);
          Dato = contMinutos - 100*(contHoras/100);       
          EEPROM.put(valorAlmacenar+5, Dato);
          Dato = 0;
          lcd.clear();
          lcd.print("Horario guardado");
          lcd.setCursor(0, 1);
          lcd.print("correctamente!!");
          delay(2000);
        }        
      }

}

int Botones(int contador, int estado){
  char keypressed = keypad.getKey();
  if (keypressed != NO_KEY){

    if(BTN == 1){
      ValorBTN = keypressed;
    }
    
    switch (keypressed)
    {
    case 'A':
    if(Editar == 1)
    {
      opcion++;  
        if(opcion == 1 && estado == 0){
        select();
      }

        if(opcion > 3){
        opcion = 3;
      }
    }
    else
    {
      opcion++;  
        if(opcion == 1 && estado == 0){
        select();
      }

        if(opcion > 1){
        opcion = 1;
      }
    }
    break;

    case 'B':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(10, largo);
    }

    opcion--;
    if(opcion < 0)
    {
      opcion = 0;
    }
    
    if(opcion == 0){
      Movcontador2 = 0;
    }  

    lcd.clear();
    break;

    case 'C':
    if(opcion != 2)
    {
      contador--;
      lcd.clear();
    }
    break;

    case 'D':
     if(opcion != 2)
    {
      contador++;
      lcd.clear();
    }
    break;
     

    case '*':
    if(led == 0)
    {
    led = 1;
    lcd.noBacklight();
    }
    else if(led = 1)
    {
    led = 0;
    lcd.backlight();
    }
    break;

    case '#':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(11, largo);
    } 
    break;
    
    case '1':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(1, largo);
    }   
    break;

    case '2':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(2, largo);
    }   
    break;
    
    case '3':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(3, largo);
    }   
    break;
    
    case '4':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(4, largo);
    }   
    break;
    
    case '5':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(5, largo);
    }   
    break;
    
    case '6':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(6, largo);
    }   
    break;
    
    case '7':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(7, largo);
    }   
    break;

    case '8':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(8, largo);
    }   
    break;

    case '9':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(9, largo);
    }   
    break;

    case '0':
    if(opcion == 2 && estado == 2)
    {      
      agregar_numeros(0, largo);
    }   
    break;
    
    }
  }
  return contador;
}

int Cursores(int LimiteInf, int LimiteSup, int contador){
  
    if (contador == LimiteInf)
    {
      lcd.setCursor(15, 1);
      lcd.write(byte(3));
    }
    else if(contador > LimiteInf && contador < LimiteSup)
    {

      if(BTN == 0){
        lcd.setCursor(15, 0);
        lcd.write(byte(2));
      } 
      else
      {
        lcd.setCursor(15, 0);
        lcd.print(ValorBTN); 
      }
      lcd.setCursor(15, 1);
      lcd.write(byte(3));   
    }
    else if(contador == LimiteSup)
    {
      if(BTN == 0){
        lcd.setCursor(15, 0);
        lcd.write(byte(2));
      } 
    }
    
    if(contador < LimiteInf)
    {
      contador = LimiteInf;
    }
    else if(contador > LimiteSup)
    {
      contador = LimiteSup;
    }
    return contador;
}


int Sensores(int sensor){
  int valor;
  DateTime now = RTC.now();
  anio= now.year(); // año
  mes = now.month(); // Mes
  dia = now.day(); // Dia
  hora = now.hour(); // Hora
  minuto = now.minute(); // Minuto
  segundo = now.second(); // Segundo
  
  switch (sensor){
    
    case 0:
     valor = 0;
    break;
    
    case 1:
     lcd.setCursor(10, 0);
     valor = temperatura;
     lcd.print(valor);
     lcd.write(byte(4));
     lcd.print("C");
    break;

    case 2:
     lcd.setCursor(10, 0);
     valor = humedad;
     if(valor < 0)
     {
        lcd.print("N/A");
     }
     else
     {
        lcd.print(valor);
        lcd.print("%");
     }
     
    break;

    case 3:
     lcd.setCursor(10, 0);
     valor = Po;
     lcd.print(valor);
    break;


    case 4:
    valorAlmacenar = 10; 
    lcd.setCursor(10, 0);
    lcd.print(EEPROM.get(valorAlmacenar, Dato));
    largo = 1023;
    Dato = 0;   
    while(opcion == 2){
      Botones(Movcontador2, 2);
      if(opcion == 2){
        lcd.setCursor(9, 1);
        lcd.write(byte(0));
      }
      lcd.setCursor(10, 1);    
    } 
    break;


    case 5:
    valorAlmacenar = 20;
    lcd.setCursor(10, 0);
    lcd.print(EEPROM.get(valorAlmacenar, Dato));
    largo = 14;
    Dato = 0;
    while(opcion == 2){
      Botones(Movcontador2, 2);    
      if(opcion == 2){
        lcd.setCursor(9, 1);
        lcd.write(byte(0));
      }
      lcd.setCursor(10, 1);    
    } 
    break;

    case 6:
    lcd.setCursor(6,0);
    lcd.print(hora);
    lcd.print(":");
    lcd.print(minuto);
    lcd.print(":");
    lcd.print(segundo);
    break;

    case 7:
    lcd.setCursor(6,0);
    lcd.print(dia);
    lcd.print("/");
    lcd.print(mes);
    break;

    case 8:
    lcd.setCursor(6,0);
    lcd.print(anio);
    break;

    case 9:
    while(opcion == 2){  
      if(BTN == 0){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Activando");
        lcd.setCursor(0,1);
        lcd.print("visor de cod");
        delay(2000);
        BTN = 1;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Desactivando");
        lcd.setCursor(0,1);
        lcd.print("visor de cod");
        delay(2000);
        BTN = 0;
      }     
      opcion = 0;    
      }        
    break;

    case 10:
    largo = 2355;
    Dato = 0;
    TipoRiego = 1;
    while(opcion == 2){
      Botones(Movcontador2, 2);
      if(opcion == 2){
        lcd.setCursor(7, 0);
        lcd.write(byte(1));
      }
      lcd.setCursor(9, 1);    
    } 
    break;

    case 11:
    while(opcion == 2){
      Botones(Movcontador2, 2);
      if(EEPROM.get(valorAlmacenar, Dato) == 2)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Desactivando");
        lcd.setCursor(0, 1);
        lcd.print("Modo auto");
        delay(2000);
        TipoRiego = 0;
        Dato = TipoRiego;
        EEPROM.put(valorAlmacenar, Dato);
        lcd.clear();
        opcion = 1;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Activando");
        lcd.setCursor(0, 1);
        lcd.print("Modo auto");
        delay(2000);
        TipoRiego = 2;
        Dato = TipoRiego;
        EEPROM.put(valorAlmacenar, Dato);
        lcd.clear();
        opcion = 1;
      } 
    } 
    break;
    
    
  }

  if(ContandorSensores > 100)
  {   
    PorcentajeHumedad = analogRead(SensorHumedad);
    PorcentajeHumedad = PorcentajeHumedad - EEPROM.get(10, Dato);
    RangoPH = analogRead(SensorPH);
    RangoPH = RangoPH - EEPROM.get(20, Dato);
    
    if(PorcentajeHumedad > 1023)
    {
      PorcentajeHumedad = 1023;
    }
    else if(PorcentajeHumedad < 0)
    {
      PorcentajeHumedad = 0;
    }

    if(RangoPH > 1023)
    {
      RangoPH = 1023;
    }
    else if(RangoPH < 0)
    {
      RangoPH = 0;
    }
  
    humedad = map(PorcentajeHumedad,1023,0,0,100);
    Po = map(RangoPH,0,1023,0,14);
    SerialpH = Po + 500;
    temperatura = RTC.getTemperature();
    SerialTemperatura = temperatura + 1000;
    Serial.println(humedad); 
    Serial.println(SerialTemperatura);
    Serial.println(SerialpH);
    ContandorSensores = 0;   
  }


    ContadorRegado = 30;
    while(ContadorRegado <= 60){  
    if(hora == EEPROM.get(ContadorRegado, Dato) && minuto == EEPROM.get(ContadorRegado+5, Dato))
    {
      SelectorRegador=250;
    }
      ContadorRegado= ContadorRegado + 10;
    }  
    

    if(EEPROM.get(valorAlmacenar, Dato) == 2)
    {
      if(humedad > -1)
      {
        if(humedad < 75)
        {
          SelectorRegador=250;
        }
      }
    }

    if(SelectorRegador > 0){
      digitalWrite(Relay, HIGH);
      SelectorRegador--;
    }
    else
    {
      digitalWrite(Relay, LOW);
    }
 
  ContandorSensores++;
}

    
void select(){
  lcd.clear();
  Movcontador2=0;
  switch (Movcontador)
  {
    case 0:
    while(opcion == 1){
      
      Movcontador2 = Botones(Movcontador2, 0);
      Movcontador2 = Cursores(0, 3, Movcontador2);
      Sensores(Movcontador2);
    
      lcd.setCursor(0, 0);
      lcd.write(byte(0));   
      lcd.setCursor(1, 0);
      lcd.print(Visual[Movcontador2]);
      lcd.setCursor(1, 1);
      lcd.print(Visual[Movcontador2+1]);           
    }
    lcd.clear();
    break;

    case 1:
    while(opcion >= 1){

      Movcontador2 = Botones(Movcontador2, 0);
      Movcontador2 = Cursores(0, 2, Movcontador2);
      if(Movcontador2 == 0)
      {
        Editar = 0;
        Sensores(0);
      }
      else if(Movcontador2 == 1)
      {
        Editar = 0;
        Sensores(2);
      }
      else if(Movcontador2 == 2)
      {
        Editar = 1;
        Sensores(4);
      } 
      
      lcd.setCursor(0, 0);
      lcd.write(byte(0));   
      lcd.setCursor(1, 0);
      lcd.print(HumCal[Movcontador2]);
      lcd.setCursor(1, 1);
      lcd.print(HumCal[Movcontador2+1]);
    }
    lcd.clear();
    break;

    case 2:

    while(opcion >= 1){

      Movcontador2 = Botones(Movcontador2, 0);
      Movcontador2 = Cursores(0, 2, Movcontador2);
      if(Movcontador2 == 0)
      {
        Editar = 0;
        Sensores(0);
      }
      else if(Movcontador2 == 1)
      {
        Editar = 0;
        Sensores(3);
      }
      else if(Movcontador2 == 2)
      {
        Editar = 1;
        Sensores(5);
      } 
      
      lcd.setCursor(0, 0);
      lcd.write(byte(0));   
      lcd.setCursor(1, 0);
      lcd.print(PHCal[Movcontador2]);
      lcd.setCursor(1, 1);
      lcd.print(PHCal[Movcontador2+1]);      
    }
    lcd.clear();    
    break;

    case 3:
    while(opcion >= 1){

      Movcontador2 = Botones(Movcontador2, 0);
      Movcontador2 = Cursores(0, 3, Movcontador2);
      if(Movcontador2 == 0)
      {
        Editar = 0;
        Sensores(0);
      }
      else if(Movcontador2 == 1)
      {
        Editar = 0;
        Sensores(6);
      }
      else if(Movcontador2 == 2)
      {
        Editar = 0;
        Sensores(7);
      } 
      else if(Movcontador2 == 3)
      {
        Editar = 0;
        Sensores(8);
      } 
      
      lcd.setCursor(0, 0);
      lcd.write(byte(0));   
      lcd.setCursor(1, 0);
      lcd.print(Reloj[Movcontador2]);
      lcd.setCursor(1, 1);
      lcd.print(Reloj[Movcontador2+1]);      
    }
    lcd.clear(); 
    break; 

    case 4:
    while(opcion >= 1){

      Movcontador2 = Botones(Movcontador2, 0);
      Movcontador2 = Cursores(0, 5, Movcontador2);
      if(Movcontador2 == 0)
      {
        Sensores(0);
        Editar = 0;
      }
      else if(Movcontador2 == 1)
      {
        Dato = 0;
        Editar = 1;
        TipoRiego = 1;
        valorAlmacenar = 30;
        Sensores(10);
      }
      else if(Movcontador2 == 2)
      {
        valorAlmacenar = 40;
        Editar = 1;
        Sensores(10);
      } 
      else if(Movcontador2 == 3)
      {
        valorAlmacenar = 50;
        Editar = 1;
        Sensores(10);
      } 
      else if(Movcontador2 == 4)
      {
        valorAlmacenar = 60;
        Editar = 1;
        Sensores(10);
      } 

      else if(Movcontador2 == 5)
      {
        Editar = 1;
        valorAlmacenar = 70;
        if(EEPROM.get(valorAlmacenar, Dato) == 2)
        {
          lcd.setCursor(9,0);          
          lcd.print("Activ");    
        }
        else
        {
          lcd.setCursor(9,0);
          lcd.print("Desac"); 
        }
        Sensores(11);
      } 
      
      lcd.setCursor(0, 0);
      lcd.write(byte(0));   
      lcd.setCursor(1, 0);
      lcd.print(Riego[Movcontador2]);
      lcd.setCursor(1, 1);
      lcd.print(Riego[Movcontador2+1]);    

        if(Movcontador2 > 0 && Movcontador2 < 5)
        {
          lcd.setCursor(9, 0);
          lcd.print(EEPROM.get(valorAlmacenar, Dato));
          lcd.print(":");
          lcd.print(EEPROM.get(valorAlmacenar+5, Dato));
        }
    }
    lcd.clear();
    break;  

    case 5:
    while(opcion >= 1){

      Movcontador2 = Botones(Movcontador2, 0);
      Movcontador2 = Cursores(0, 1, Movcontador2);
      if(Movcontador2 == 0)
      {
        Sensores(0);
        Editar = 0;
      }
      else if(Movcontador2 == 1)
      {
        Editar = 1;
        if(BTN == 0)
        {
          lcd.setCursor(4, 0);
          lcd.print("'desac'");
        }
        else
        {
          lcd.setCursor(4, 0);
          lcd.print("'activ'");
        }
        Sensores(9);
      }
      
      lcd.setCursor(0, 0);
      lcd.write(byte(0));   
      lcd.setCursor(1, 0);
      lcd.print(BTNCod[Movcontador2]);
      lcd.setCursor(1, 1);
      lcd.print(BTNCod[Movcontador2+1]);      
    }
    lcd.clear();
    break;
  }       
}

void menu(){
    
    Movcontador = Botones(Movcontador, 0);
    Movcontador = Cursores(0, 5, Movcontador);
    Sensores(0);
    
    lcd.setCursor(0, 0);
    lcd.print("Menu:");
    lcd.setCursor(7, 0);
    lcd.write(byte(0));   
    lcd.setCursor(8, 0);
    lcd.print(Menu1[Movcontador]);
    lcd.setCursor(8, 1);
    lcd.print(Menu1[Movcontador+1]);
    lcd.setCursor(0,1);
    lcd.print(hora);
    lcd.print(":");
    lcd.print(minuto);        
}

void loop(){
  menu();
}
