#include <Keypad.h>
#include <Wire.h>               /*~ Librería I2C ~*/
#include <LiquidCrystal.h>  /*~ Librería LCD ~*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

void noop() {
  Serial.println("Noop");
}

// ================ Hardware ================

const uint8_t ROWS = 4;
const uint8_t COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '.', '0', '#', 'D' }
};

uint8_t colPins[COLS] = { 12, 13, 32, 33 }; // Pins connected to C1, C2, C3, C4
uint8_t rowPins[ROWS] = { 25, 26, 27, 14 }; // Pins connected to R1, R2, R3, R4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int rs = 22, en = 21, d4 = 5, d5 = 18, d6 = 19, d7 = 23;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setupLcd() {
  lcd.begin(20, 4);
  lcd.clear();
}

// ================ Wifi ================

void setupWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

String get(const char* url) {
  Serial.printf("Sending GET request to %s\n", url);

  HTTPClient http;

  http.begin(url);

  int res = http.GET();
  String rt;

  if (res > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(res);
    String payload = http.getString();
    rt = payload;
    Serial.println(payload);
  } else {
    Serial.print("Error code: ");
    Serial.println(res);
    rt = "";
  }
  http.end();
  return rt;
}

int post(const char* url, const char* payload) {
  Serial.printf("Sending POST request to %s\n", url);

  HTTPClient http;

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int res = http.POST(payload);

  if (res > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(res);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error code: ");
    Serial.println(res);
  }
  http.end();
  return res;
}

void put(const char* url, const char* payload) {
  Serial.printf("Sending PUT request to %s\n", url);

  HTTPClient http;

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int res = http.PUT(payload);

  if (res > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(res);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error code: ");
    Serial.println(res);
  }
  http.end();
}

void del(const char* url) {
  Serial.printf("Sending DELETE request to %s\n", url);

  HTTPClient http;

  http.begin(url);

  int res = http.sendRequest("DELETE");

  if (res > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(res);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error code: ");
    Serial.println(res);
  }
  http.end();
}

bool validateUser(const char* url, const char* userid, const char* password) {
  String payload = "{\"userid\":\"";
  payload += userid;
  payload += "\",\"password\":\"";
  payload += password;
  payload += "\"}";

  int res = post(url, payload.c_str());
  return res == 200;
}

void updatePrice(const char* url, const char* ean13, float price) {
  String endpoint = String(url) + "/price/" + ean13;
  String payload = "{\"price\":";
  payload += String(price);
  payload += "}";
  put(endpoint.c_str(), payload.c_str());
}

void updateAmount(const char* url, const char* ean13, int amount) {
  String endpoint = String(url) + "/amount/" + ean13;
  String payload = "{\"amount\":";
  payload += String(amount);
  payload += "}";
  put(endpoint.c_str(), payload.c_str());
}

void deleteProduct(const char* url, const char* ean13) {
  String endpoint = String(url) + "/" + ean13;
  del(endpoint.c_str());
}

// ================ Display Basics ================

// Cursor
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

void writeRow (int row, String text, int maxlength = 20) {
    lcd.setCursor(0, row);
    if (text.length() > maxlength) {
        text = text.substring(0, maxlength);
    }
    lcd.print(text);
}

void clearRow(int row) {
    writeRow(row, "                    ");
}

// ================ Display Utilities ================

void setFooter (String footerText) {
    writeRow(3, footerText);
}

void write3Rows (String* displayedRows, int maxlength = 20) {
    for (int i = 0; i < 3; i++) {
        clearRow(i);
        writeRow(i, displayedRows[i], maxlength);
    }
}

void write4Rows (String* displayedRows, int maxlength = 20) {
    for (int i = 0; i < 4; i++) {
        clearRow(i);
        writeRow(i, displayedRows[i], maxlength);
    }
}

void display3Options (String* options, int nOptions, int index, int maxlength = 20) {
  if (index + 2 < nOptions) {
    String displayedOptions[] = {options[index], options[index + 1], options[index + 2]};
    write3Rows(displayedOptions, maxlength);
  }
}

void displayList (String* listItems, int nlistItems, int index, int maxlength = 20) {
  if (index + 3 < nlistItems) {
    String displayedItems[] = {listItems[index], listItems[index + 1], listItems[index + 2], listItems[index + 3]};
    write4Rows(displayedItems, maxlength);
  }
}

// Globals
const char* apiBaseUrl = "https://pokesp.onrender.com";
int typeDisplay;

int displayIndex;
String inputFooter;
int selectorY;
int pag;
String inputText;

String currentUserID;
String currentEAN13;

// ================ Display 0 ================

// Variables Display 0
String* options;
int nOptions;
int selectedOption;
int optionSelectorY;

// Estados Display 0
int currentStateD0;
int nStatesD0;
int nextFCodeD0;

// Menu Principal
String menu_principal_options[] = {"Consultar", "Actualizar Precio", "Contar", "Eliminar", "Cerrar Sesion"};

// ================ Display 1 ================

// Variables Display 1
String label;

// Estados Display 1
String* d1_labels;
String d1_buffer[8];
int currentStateD1;
int nStatesD1;
int nextFCodeD1;


// Login
String login_labels[] = {"Inserte UserID", "Inserte Password"};
// Consultar
String consultar_labels[] = {"EAN13"};
// ActualizarPrecio
String actualizar_precio_labels[] = {"EAN13", "Nuevo Precio"};
// Contar 
String contar_labels[] = {"EAN13", "Nueva Cantidad"};
// Eliminar
String eliminar_labels[] = {"EAN13"};

// ================ Display 2 ================

// Estados Display 2
String d2_items[4];
int d2_n_items;
int nextFCodeD2;

// ================ CallBack Implementation ================

void callFunctionCode (int code) {
    Serial.println("Call Function Code");
    Serial.println(code);
    switch (code) {
        case -1:
            loginAction();
            break;
        case 0:
            login();
            break;
        case 1:
            menuPrincipal();
            break;
        case 10:
            consultar();
            break;
        case 100:
            consultarAction();
            break;
        case 11:
            actualizarPrecio();
            break;
        case 110:
            actualizarPrecioAction();
            break;
        case 12:
            contar();
            break;
        case 120:
            contarAction();
            break;
        case 13:
            eliminar();
            break;
        case 130:
            eliminarAction();
            break;
        case 14:
            cerrarSesion();
            break;        
        default:
            break;
    }
}

// ================ Handle Displays ================

void updateInputText (String newText) {
    clearRow(3);
    writeRow(3, newText);
}

void writeInInput (char c) {
  if (inputText.length() > 20) {
    return;
  }
  inputText = inputText.substring(0, inputText.length() - 1);
  inputText += c;
  inputText += '_';
  updateInputText(inputText);
}

void deleteInInput () {
  inputText = inputText.substring(0, inputText.length() - 2);
  inputText += '_';
  updateInputText(inputText);
}

// ================ Display 0 ================

void initDisplay0 (String* options_, int nOptions_) {
    typeDisplay = 0;
    displayIndex = 0;
    
    options = options_;
    nOptions = nOptions_;
    optionSelectorY = 0;
    selectedOption = 0;

    inputText = "_";
    updateDisplay0();
}

void updateDisplay0 () {
    display3Options(options, nOptions, displayIndex, 19);
    updateInputText(inputText);
    updateOptionSelectorY(optionSelectorY);
}

void updateOptionSelectorY (int optionSelectorY) {
  for (int i = 0; i < 3; i++) {
    if (i == optionSelectorY) {
      writeAsterisc(i);
    } else {
      deleteAsterisc(i);
    }
  }
}

void moveupD0() {
    if (selectedOption > 0) {
        selectedOption--;
    }

    if (optionSelectorY > 0) {
        optionSelectorY--;
    } else {
        if (displayIndex > 0) {
            displayIndex--;
        }
    }

    updateDisplay0();
}

void movedownD0() {
    if (selectedOption < nOptions - 1) {
        selectedOption++;
    }

    if (optionSelectorY < 2) {
        optionSelectorY++;
    } else {
        if (displayIndex + 2 < nOptions) {
            displayIndex++;
        }
    }
    updateDisplay0();
}

void selectOptionD0() {
    callFunctionCode(nextFCodeD0*10 + selectedOption);
}

void writeAsterisc(int row) {
    writeChar('*', 19, row);
}

void deleteAsterisc(int row) {
    clearChar(19, row);
}

// ================ Display 1 ================

void initDisplay1 (String labelText) {
    if (typeDisplay != 1) {
      clearRow(0);
      clearRow(2);
    }
    typeDisplay = 1;

    label = labelText;

    inputText = "_";
    updateDisplay1();
}

void updateDisplay1 () {
    updateLabelText(label);
    updateInputText(inputText);
}

void updateLabelText (String newText) {
    clearRow(1);
    if (newText.length() > 20) {
        newText = newText.substring(0, 20);
    }
    writeRow(1, newText);
}

void nextDisplay1(int jump) {
  if (currentStateD1 + jump < 0) {
    return;
  }
  d1_buffer[currentStateD1] = inputText.substring(0, inputText.length() - 1);
  currentStateD1 += jump;
  if (currentStateD1 < nStatesD1) {
    initDisplay1(d1_labels[currentStateD1]);
  } else {
    callFunctionCode(nextFCodeD1);
  }
}

// ================ Display 2 ================

void initDisplay2 () {
    if (typeDisplay != 2) {
      clearRow(0);
      clearRow(1);
      clearRow(2);
      clearRow(3);
    }
    typeDisplay = 2;
    displayIndex = 0;
    updateDisplay2();
}

void updateDisplay2 () {
    displayList(d2_items, d2_n_items, displayIndex);
}

void moveupD2() {
    if (displayIndex > 0) {
        displayIndex--;
    }
    updateDisplay2();
}

void movedownD2() {
    if (displayIndex + 3 < d2_n_items) {
        displayIndex++;
    }
    updateDisplay2();
}

void nextDisplay2() {
    callFunctionCode(nextFCodeD2);
}

void next(int jump = 1) {
  switch (typeDisplay)
  {
    case 1:
      nextDisplay1(jump);
      break;
    default:
      break;
  }
}

// ================ Display 0 ================

void menuPrincipal() {
  nStatesD0 = 1;
  nextFCodeD0 = 1;
  currentStateD0 = 0;
  initDisplay0(menu_principal_options, 5);
}

// ================ Display 1 ================

void login() {
  d1_labels = login_labels;
  nStatesD1 = 2;
  nextFCodeD1 = -1;
  currentStateD1 = 0;
  initDisplay1(d1_labels[currentStateD1]);
}

void loginAction() {
  Serial.print("Login ");
  currentUserID = d1_buffer[0];
  String password = d1_buffer[1];
  Serial.println(currentUserID);
  Serial.println(password);

  bool isValid = validateUser((String(apiBaseUrl) + "/users/validate").c_str(), currentUserID.c_str(), password.c_str());
  
  if (isValid) {
    menuPrincipal();
  } else {
    login();
  }
}

void consultar() {
  d1_labels = consultar_labels;
  nStatesD1 = 1;
  nextFCodeD1 = 100;
  currentStateD1 = 0;
  initDisplay1(d1_labels[currentStateD1]);
}

void consultarAction() {
  Serial.print("Consultar ");
  currentEAN13 = d1_buffer[0];
  Serial.println(currentEAN13);
  mostrarProducto(currentEAN13);
}

void actualizarPrecio() {
  d1_labels = actualizar_precio_labels;
  nStatesD1 = 2;
  nextFCodeD1 = 110;
  currentStateD1 = 0;
  initDisplay1(d1_labels[currentStateD1]);
}

void actualizarPrecioAction() {
  Serial.print("Actualizar Precio ");
  currentEAN13 = d1_buffer[0];
  String nuevoPrecio = d1_buffer[1];
  Serial.println(currentEAN13);
  Serial.println(nuevoPrecio);
  updatePrice((String(apiBaseUrl) + "/products").c_str(), currentEAN13.c_str(), nuevoPrecio.toFloat());
  menuPrincipal();
}

void contar() {
  d1_labels = contar_labels;
  nStatesD1 = 2;
  nextFCodeD1 = 120;
  currentStateD1 = 0;
  initDisplay1(d1_labels[currentStateD1]);
}

void contarAction() {
  Serial.print("Contar ");
  currentEAN13 = d1_buffer[0];
  String nuevaCantidad = d1_buffer[1];
  Serial.println(currentEAN13);
  Serial.println(nuevaCantidad);
  updateAmount((String(apiBaseUrl) + "/products").c_str(), currentEAN13.c_str(), nuevaCantidad.toInt());
  menuPrincipal();
} 

void eliminar() {
  d1_labels = eliminar_labels;
  nStatesD1 = 1;
  nextFCodeD1 = 130;
  currentStateD1 = 0;
  initDisplay1(d1_labels[currentStateD1]);
}

void eliminarAction() {
  Serial.print("Eliminar ");
  currentEAN13 = d1_buffer[0];
  Serial.println(currentEAN13);
  deleteProduct((String(apiBaseUrl) + "/products").c_str(), currentEAN13.c_str());
  menuPrincipal();
}

void cerrarSesion() {
  currentUserID = "";
  login();
}

// ================ Display 2 ================

void mostrarProducto(String ean13) {
  currentEAN13 = ean13;
  String json = get((String(apiBaseUrl) + "/products/" + ean13).c_str());
  int startIndex, endIndex;

  // Parse ean13
  startIndex = json.indexOf("\"ean13\":") + 8;
  endIndex = json.indexOf(",", startIndex);
  
  d2_items[0] = (json.substring(startIndex, endIndex)).c_str();

  // Parse name
  startIndex = json.indexOf("\"name\":\"") + 8;
  endIndex = json.indexOf("\"", startIndex);
  d2_items[1] = (json.substring(startIndex, endIndex)).c_str();

  // Parse price
  startIndex = json.indexOf("\"price\":") + 8;
  endIndex = json.indexOf(",", startIndex);
  d2_items[2] = (json.substring(startIndex, endIndex)).c_str();

  // Parse amount
  startIndex = json.indexOf("\"amount\":") + 9;
  endIndex = json.indexOf("}", startIndex);
  d2_items[3] = (json.substring(startIndex, endIndex)).c_str();

  
  d2_n_items = 4;
  nextFCodeD2 = 1;
  initDisplay2();
}


// ================ Keypad Handler ================

void onKeyPress ( char key ) {
    switch ( typeDisplay ) {
        case 0:
          if (key == 'A') {
            noop();
          } else if (key == 'B') {
            moveupD0();
          } else if (key == 'C') {
            movedownD0();
          } else if (key == 'D') {
            selectOptionD0();
          } else {
            noop();
          }
        break;
        case 1:
          if (key == 'A') {
            if ((inputText.length() == 1) && (currentStateD1 != 0)) {
              next(-1);
            } else {
              deleteInInput();
            }
          } else if ((key == 'B')||(key == 'C')) {
            noop();
          } else if (key == 'D') {
            next(1);
          } else {
            writeInInput(key);
          }
          break;
        case 2:
          if (key == 'A') {
            noop();
          } else if (key == 'B') {
            moveupD2();
          } else if (key == 'C') {
            movedownD2();
          } else if (key == 'D') {
            nextDisplay2();
          } else {
            noop();
          }
          break;
        default:
          break;
    }
}

void setup ( void ) {
  setupLcd();
  Serial.begin(115200);
  setupWifi();
  login();
} 

void loop ( void ) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
        onKeyPress(key);
    }
}
