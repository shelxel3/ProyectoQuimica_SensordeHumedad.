/*POSIBLES CASOS

  1 Nivel bajo de agua. Muestra temp/hum y humedad del suelo. Da aviso rellenar tanque
   1.1 Activo bomba. Muestra temp/hum y humedad del suelo. Da otro aviso: No se puede regar, rellenar tanque.
  2 Nivel agua OK. Muestra temp/hum y humedad del suelo. Aviso nivel OK
      2.1 Humedad suelo OK. Muestra temp/hum y humedad del suelo.
        2.1.1 Activo bomba. Comienza el riego
      2.2 Humedad suelo NO OK. Muestra temp/hum y humedad del suelo. Comienza riego automático.
        2.2.2 Activo bomba. Muestra aviso, no se puede activar riego manual. Riego automático activo.
*/

#include <DHT.h>//Incluyo la librería del sensor DHT11

////////////////////////////////Para la pantalla LCD
#include <LiquidCrystal_I2C.h> // Debe descargar la Libreria que controla el I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); //SI no va, se puede poner 0x20 para probar
//el modulo es de 16 por 2. el protocolo es 0x27. si no va, probar con 0x20

//Pines del módulo I2C
//VCC GND SON LA TENSIÓN Y LA TIERRA
//SDA VA AL A4
//SCL VA AL A5

//////////////////////////Configuro pines y variables
int sensor = 2;//Pin 2 para el valor del sensor DHT11
int temp, humedad;//Configuro las variables de temperatura y humedad del DHT11
int boton = 3;//Pin 3 para activar manualmente la bomba
const int nivel = 9; //Pin 9 para medir el nivel de agua
const int bomba = 13; //Pin 13 para la bomba
const int humedadsuelo = A0; //A0 para la humedad del suelo

DHT dht (sensor, DHT11);//Configuro el modelo de DHT. Es el DHT11
int botonest = 0; //Inicializamos a 0 el estado del boton

///////////////////////////////Parte de humedad suelo y riego

void setup()
{
  Serial.begin(9600);//Arrancamos el puerto serie a 9600
  dht.begin();//Arranco el sensor DHT11

  //Configuramos entradas y salidas
  pinMode(humedadsuelo, INPUT);//Configuro humedadsuelo como entrada
  pinMode(bomba, OUTPUT);//Configuro bomba como salida
  pinMode(nivel, INPUT);//Configuro en nivel de agua como entrada
  pinMode(boton, INPUT);//Configuro en boton como entrada

  //Para la pantalla LCD
  lcd.init(); //Inicializamos el LCD
  lcd.backlight(); //Activamos la luz de fondo
  lcd.clear(); //Limpiamos lo que haya en pantalla
  lcd.setCursor(0, 0); //Iniciamos el cursor en el punto 0,0
}

void loop()
{
  int SensorValue = analogRead(humedadsuelo);//Leo el valor de la humedad y lo meto en SensorValue
  int SensorNivel = digitalRead(nivel); //Leo lo que marca el nivel de agua
  int botonest = digitalRead(boton);//Leo lo que marca el boton

  //Leo y meto los valores de temperatura y humedad en las variables temp y humedad
  humedad = dht.readHumidity();
  temp = dht.readTemperature();

  /////Puerto serie
  //Imprimo por el puerto serie los valores de temperatura y humedad del DHT11
  Serial.print("Temperatura: "); Serial.print(temp);
  Serial.print("ºC Humedad: "); Serial.print(humedad); Serial.println("%");

  //Imprimo por el puerto serie el valor de la humedad del suelo
  Serial.print("Humedad del suelo: "); Serial.print(SensorValue); Serial.println("%");
  delay(500);

  /////Para mostrarlo en el LCD

  lcd.init(); //Inicializamos el LCD
  lcd.backlight(); //Activamos la luz de fondo
  lcd.clear();//blanqueamos la pantalla
  lcd.setCursor(0, 0); //Posicion: columna cero fila cero
  lcd.print("DHT:");
  lcd.print(temp);
  lcd.print("C/");
  lcd.print(humedad);
  lcd.print("%");
  lcd.setCursor(0, 1); //Posicion: columna cero fila uno
  lcd.print("HS:");
  lcd.print(SensorValue);
  lcd.print("%");

  /*
    //esto si quiero que la pantalla vaya de izquierda a derecha.
    for(int c=0;c<12;c++){ //hace una secuencia hacia la izquierda y luego hacia la derecha, por 12 pasos
    lcd.scrollDisplayLeft();
    delay(600);   }
    for(int c=0; c<12;c++){
    lcd.scrollDisplayRight();
    delay(600);   }
    lcd.clear();//blanqueamos la pantalla
  */

  if (SensorNivel == 0 && botonest == 1)
  { Serial.println("Nivel bajo de Agua. Rellenar el tanque");

    lcd.setCursor(0, 0); //Posicion: columna cero fila cero
    lcd.print("Nivel bajo agua");
    lcd.setCursor(0, 1); //Posicion: columna cero fila cero
    lcd.print("Rellenar tanque");
    delay (2000);
    lcd.clear();//blanqueamos la pantalla
    lcd.setCursor(0, 0); //Posicion: columna cero fila cero
    lcd.print("DHT:");
    lcd.print(temp);
    lcd.print("C/");
    lcd.print(humedad);
    lcd.print("%");
    lcd.setCursor(0, 1); //Posicion: columna cero fila uno
    lcd.print("HS:");
    lcd.print(SensorValue);
    lcd.print("%");
    delay (1000);

  }

  if (SensorNivel == 0 && botonest == 0)
  { Serial.println("Imposible regar, nivel bajo de agua. Debe rellenar el tanque");
    lcd.clear();//blanqueamos la pantalla
    lcd.setCursor(0, 0); //Posicion: columna cero fila cero
    lcd.print("IMPOSIBLE REGAR");
    lcd.setCursor(0, 1); //Posicion: columna cero fila cero
    lcd.print("Nivel bajo");
  }


  if (SensorNivel == 1)
  {
    Serial.println("Nivel de agua correcto, se puede regar");

    if (SensorValue >= 700) // el valor que considero seco y hay que regar es de 700
    {
      // Si la tierra está seca, comenzara a regar
      // Riega durante 2 segundoS y espera a comprobar la humedad otro segundo
      Serial.println("La tierra está seca, comienza el riego automático");
      lcd.clear();//blanqueamos la pantalla
      lcd.setCursor(0, 0); //Posicion: columna cero fila cero
      lcd.print("Riego automatico");
      lcd.setCursor(0, 1); //Posicion: columna cero fila uno
      lcd.print("HS:");
      lcd.print(SensorValue);
      lcd.print("%");
      digitalWrite(bomba, HIGH);
      delay(2000);
      digitalWrite(bomba, LOW);
      delay(1000);

      if (botonest == 0)
      { Serial.println("No se puede activar riego manual. Riego automático activo");
        lcd.clear();//blanqueamos la pantalla
        lcd.setCursor(0, 0); //Posicion: columna cero fila cero
        lcd.print("Regando automatico");
        lcd.setCursor(0, 1); //Posicion: columna cero fila cero
        lcd.print("NO MANUAL");
        delay(1000);

      }
    }

    if (SensorValue < 700) //No es necesario el riego automático, pero se puede regar de forma manual
    {
      if (botonest == 0) {
        Serial.println("Regando de forma manual");
        lcd.clear();//blanqueamos la pantalla
        lcd.setCursor(0, 0); //Posicion: columna cero fila cero
        lcd.print("Riego manual");
        lcd.setCursor(0, 1); //Posicion: columna cero fila uno
        lcd.print("HS:");
        lcd.print(SensorValue);
        lcd.print("%");
        digitalWrite(bomba, HIGH);
      }
      if (botonest == 1) {
        digitalWrite(bomba, LOW);
      }
    }
  }
}
