Wokwi: https://wokwi.com/projects/402840506624528385

Admin Page: https://pokesp.onrender.com/




Modify:

```ino
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 16 column and 2 rows


  lcd.init(); // initialize the lcd
  lcd.backlight();
```
