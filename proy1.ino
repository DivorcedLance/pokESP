/* ESP32 HTTP Client example

  https://wokwi.com/projects/384757860096897025

  This project makes an HTTP GET request to https://www.google.com/search?q=arduino every 5 seconds.

  If you want to request from a local server you can uncomment and configure getLocal() but you need the Wokwi IoT Gateway, as explained here:

  https://docs.wokwi.com/guides/esp32-wifi#the-private-gateway

  Note that the IoT Gateway requires a Wokwi Club subscription.
  To purchase a Wokwi Club subscription, go to https://wokwi.com/club
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <Keypad.h>
#include <Wire.h>               /*~ Librería I2C ~*/
#include <LiquidCrystal_I2C.h>  /*~ Librería LCD ~*/

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
// Defining the WiFi channel speeds up the connection:
#define WIFI_CHANNEL 6

int lastRequest = 0;

const uint8_t ROWS = 4;
const uint8_t COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

uint8_t colPins[COLS] = { 12, 13, 32, 33 }; // Pins connected to C1, C2, C3, C4
uint8_t rowPins[ROWS] = { 25, 26, 27, 14 }; // Pins connected to R1, R2, R3, R4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void getGoogle() {
  const char* url = "https://www.google.com";

  get(url);
}

void getLocal() {
  const char* url = "https://host.wokwi.internal:45455/weatherforecast"; // Change this url to request from your local server.

  get(url);
}

void getPoke() {
  const char* url = "https://pokeapi.co/api/v2/pokemon/ditto"; // Change this url to request from your local server.

  get(url);
}

void get(const char* url) {
  Serial.printf("Sending request to %s\n", url);

  HTTPClient http;

  http.begin(url);

  int res = http.GET();

  if (res > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(res);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error code: ");
    Serial.println(res);
  }
  // Free resources
  http.end();
}

/*~ Los siguientes arrays contienen un caracter, es decir se indica que pixel se enciendo o apaga dependiendo si es 1 o 0 ~*/
/*~ Icono o caracter 1 ~*/
byte caracter1 [ ] = {
  B01110,							/*~ ⬜⬛⬛⬛⬜~*/
  B01010,							/*~ ⬜⬛⬜⬛⬜~*/
  B01110,							/*~ ⬜⬛⬛⬛⬜~*/
  B11111,							/*~ ⬛⬛⬛⬛⬛~*/
  B00100,							/*~ ⬜⬜⬛⬜⬜~*/
  B00100,						  /*~ ⬜⬜⬛⬜⬜~*/
  B01010,							/*~ ⬜⬛⬛⬛⬜~*/
  B10001							/*~ ⬛⬜⬜⬜⬛~*/
};

/*~ Icono o caracter 2 ~*/
byte caracter2 [ ] = {
  B11111,							/*~ ⬛⬛⬛⬛⬛~*/
  B10001,							/*~ ⬛⬜⬜⬜⬛~*/
  B10001,							/*~ ⬛⬜⬜⬜⬛~*/
  B11111,							/*~ ⬛⬛⬛⬛⬛~*/
  B11111,							/*~ ⬛⬛⬛⬛⬛~*/
  B11111,							/*~ ⬛⬛⬛⬛⬛~*/
  B11111,							/*~ ⬛⬛⬛⬛⬛~*/
  B11011							/*~ ⬛⬛⬜⬛⬛~*/
};


LiquidCrystal_I2C lcd ( 0x27, 20, 4 );  /*~ Instancia de la clase para el manejo de la pantalla ( Dirección I2C, cantidad de columnas, cantidad de filas ) ~*/

int currentX = 0;
int currentY = 0;

void moveCursor(int x, int y) {
    lcd.setCursor(x, y);
    currentX = x;
    currentY = y;
}

void clearChar(int x, int y) {
    moveCursor(x, y);
    lcd.print(" ");
}

void writeChar(char c, int x, int y) {
    moveCursor(x, y);
    lcd.print(c);
}

void moveChar(char c, int x, int y) {
    Serial.print(x);
    Serial.print(" ");
    Serial.println(y);

    if (x < 0) {
        currentX = 19;
    }
    if (x >= 20) {
        currentX = 0;
    }
    if (y < 0) {
        currentY = 3;
    }
    if (y >= 4) {
        currentY = 0;
    }
    
    return;
}

void controlCursor(char key) {
    switch (key) {
        case 'D':
            lcd.clear();
            break;
        case '0':
            moveChar('o', 0, 0);
            break;
        case '2':
            moveChar('^', currentX, currentY - 1);
            break;
        case '8':
            moveChar('v', currentX, currentY + 1);  
            break;
        case '4':
            moveChar('<', currentX - 1, currentY);
            break;
        case '6':
            moveChar('>', currentX + 1, currentY);
            break;
        default:
            break;
    }
}


void setup ( void ) {
  
  lcd.init ( ); 					           	/*~ Inicializar la pantalla LCD ~*/
  
  lcd.createChar ( 0, caracter1 );   	/*~ Indicar al programa que genere un caracter a partir del array de bits. ~*/
  lcd.createChar ( 1, caracter2 );   	/*~ Indicar al programa que genere otro un caracter. ~*/
  
  lcd.backlight ( );     		          /*~ Encender la luz de fondo. ~*/
      delay ( 1000 );      	          	/*~ Esperar 1 segundo. ~*/
  
  lcd.noBacklight ( );   		          /*~ Apagar la luz de fondo. ~*/
      delay ( 1000 );      		          /*~ Esperar 1 segundo. ~*/
  lcd.backlight ( );     		          /*~ Encender la luz de fondo. ~*/

  lcd.setCursor ( 0, 0 );
  writeChar('o', 0, 0);

  Serial.begin(115200); //UART
}

void loop ( void ) {

    char key = keypad.getKey();
    if (key != NO_KEY) {
        Serial.println(key);
        controlCursor(key);
    }
}

// void loop ( void ) {
  
//   lcd.backlight ( );     		          /*~ Encender la luz de fondo. ~*/
//   	delay ( 1000 );      	          	/*~ Esperar 1 segundo. ~*/
  
//   lcd.noBacklight ( );   		          /*~ Apagar la luz de fondo. ~*/
//   	delay ( 1000 );      		          /*~ Esperar 1 segundo. ~*/
//   lcd.backlight ( );     		          /*~ Encender la luz de fondo. ~*/
  
   
//    lcd.setCursor ( 0, 0 );            /*~ ( columnas, filas) Ubicamos el cursor en la primera posición(columna:0) de la primera línea(fila:0) ~*/
//    lcd.write ( 0 );		  		          /*~ Mostramos nuestro primer icono o caracter ~*/
//    	delay ( 1000 );				            /*~ Esperar 1 segundo ~*/
//    lcd.clear ( );				              /*~ Limpiar pantalla ~*/
//    lcd.setCursor ( 0, 1 );            /*~ ( columnas, filas) Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1) ~*/
//    lcd.write ( 1 );                   /*~ Mostramos nuestro segundo icono o caracter ~*/
//     delay ( 1000 );				            /*~ Esperar 1 segundo ~*/
//    lcd.clear ( );				              /*~ Limpiar pantalla ~*/
  
//    lcd.setCursor ( 0, 0 );   	        /*~ ( columnas, filas) Ubicamos el cursor en la primera posición(columna:0) de la primera línea(fila:0) ~*/
//    lcd.print ( "Bienvenido" );     /*~ Mostrar una cadena de texto (no exceder 16 caracteres por línea)~*/
//    delay ( 1000 );				            /*~ Esperar 1 segundo ~*/   			
  	
//   for ( uint8_t i = 0; i < ( 40 ); i++ ) {  /*~ Este ciclo es para que se vea como se recorren los caracteres, si no es colocado se vería muy rápido ~*/
    
//     lcd.scrollDisplayRight ( );       /*~ Recorrer caracteres de derecha a izquierda ~*/
//     delay ( 100 );				            /*~ Esperar 100 milisegundos ~*/
    
//   }
  
//   for ( uint8_t i = 0; i < ( 40 ); i++ ) {  /*~ Este ciclo es para que se vea como se recorren los caracteres, si no es colocado se vería muy rápido ~*/
    
//     lcd.scrollDisplayLeft ( );        /*~ Recorrer caracteres de izquierda a derecha ~*/
//     delay ( 100 );				            /*~ Esperar 100 milisegundos ~*/
    
//   }
  
//   lcd.clear ( );				              /*~ Limpiar pantalla ~*/
  
  
// }

// void setup(void) {
//   Serial.begin(115200);

//   // WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
//   // Serial.print("Connecting to WiFi ");
//   // Serial.print(WIFI_SSID);
//   // // Wait for connection
//   // while (WiFi.status() != WL_CONNECTED) {
//   //   delay(100);
//   //   Serial.print(".");
//   // }
//   // Serial.println(" Connected!");

//   // Serial.print("IP address: ");
//   // Serial.println(WiFi.localIP());


// }

// void loop(void) {
  
//   char key = keypad.getKey();

//   if (key != NO_KEY) {
//     Serial.println(key);
//   }

//   // if (millis() - lastRequest > 5000) {
//   //   if (WiFi.status() == WL_CONNECTED) {
//   //     // getGoogle();
//   //     getPoke();
//   //     lastRequest = millis();
//   //   } else {
//   //     Serial.println("WiFi not connected");
//   //   }
//   // }
// }
