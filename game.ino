#include "IRremote.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif
LiquidCrystal_I2C lcd(0x27,20,4); 
IRrecv irrecv(3); 
decode_results results;

byte customS[8][8] = {                          //Кстомные символы человека, ключа и окружения
  {0x1f,0x1f,0x1f,0x1f,0x7,0x3,0x3,0x1},
  {0x1f,0x1f,0x1e,0x1c,0x18,0x18,0x10,0x10},
  {0x1,0x3,0x7,0x1f,0x1f,0x1f,0x1f,0x1f},
  {0x10,0x10,0x18,0x18,0x1c,0x1f,0x1f,0x1f},
  {0x1f,0x1f,0x7,0x3,0x1,0x1,0x0,0x0},
  {0x0,0xc,0x1e,0x1f,0x1f,0x1f,0x1f,0x1f},
  {0x1f,0x1f,0xf,0xf,0x7,0x7,0x7,0x7},
  {0x6,0x6,0x14,0x1f,0x4,0xa,0xa,0x1b}
  };

int pos = 2; //начальная позиция экрана
int x = 1;    //Начальное положение человека
int y = 9;
bool key = 0;  //Изначально ключ не найден
unsigned long a = 0;


//карта
byte list [48][20] = {
{1, 1, 1, 1, 3, 2, 1, 1, 1, 1, 3, 6, 2, 3, 6, 1, 1, 1, 2, 1},
{1, 1, 3, 0, 0, 0, 0, 8, 3, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0},
{1, 0, 0, 0, 7, 4, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0, 4, 1},
{3, 0, 8, 1, 1, 1, 0, 4, 1, 5, 4, 1, 1, 1, 5, 7, 4, 1, 1, 1},
{3, 0, 6, 1, 1, 1, 0, 8, 1, 3, 6, 1, 1, 1, 1, 1, 3, 6, 2, 1},
{5, 0, 0, 8, 1, 3, 0, 0, 0, 0, 0, 0, 6, 3, 0, 0, 0, 0, 0, 2},
{1, 0, 0, 4, 1, 5, 0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0},
{1, 5, 4, 1, 1, 1, 5, 4, 1, 1, 5, 4, 1, 5, 0, 0, 8, 1, 1, 1},
{3, 2, 1, 1, 1, 3, 6, 1, 3, 6, 2, 1, 1, 3, 0, 0, 6, 1, 1, 1},
{5, 0, 0, 6, 3, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 8, 1},
{1, 0, 0, 0, 0, 0, 0, 4, 7, 0, 0, 0, 8, 1, 7, 0, 0, 0, 0, 8},
{1, 5, 0, 4, 5, 7, 4, 1, 1, 5, 0, 0, 8, 1, 1, 1, 4, 5, 0, 4},
{1, 3, 0, 1, 1, 3, 2, 3, 6, 1, 0, 0, 6, 1, 3, 2, 1, 1, 0, 1},
{3, 0, 0, 8, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 8, 3, 0, 8},
{5, 0, 0, 0, 0, 0, 4, 7, 0, 4, 7, 0, 0, 4, 5, 0, 0, 0, 0, 4},
{1, 7, 7, 4, 1, 1, 1, 1, 0, 2, 1, 1, 1, 1, 1, 1, 5, 4, 1, 1},
{1, 1, 3, 6, 8, 1, 1, 3, 0, 6, 3, 6, 2, 1, 1, 1, 1, 1, 1, 1},
{1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 8, 3, 0, 6, 2, 3, 2},
{3, 0, 7, 0, 0, 0, 0, 8, 5, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{5, 0, 1, 1, 7, 4, 1, 1, 1, 1, 1, 1, 5, 0, 7, 4, 1, 5, 7, 4},
{3, 0, 8, 1, 1, 1, 3, 6, 2, 1, 1, 1, 3, 0, 8, 1, 3, 2, 1, 1},
{5, 0, 0, 2, 3, 0, 0, 0, 0, 0, 6, 3, 0, 0, 4, 0, 0, 0, 0, 0},
{1, 0, 0, 0, 0, 0, 8, 7, 0, 0, 0, 0, 0, 0, 8, 0, 0, 7, 0, 0},
{1, 5, 7, 4, 1, 1, 1, 1, 1, 5, 4, 1, 7, 4, 1, 5, 4, 1, 1, 5},
{3, 2, 1, 3, 6, 2, 1, 1, 1, 3, 6, 3, 6, 2, 1, 3, 2, 1, 1, 1},
{0, 0, 0, 0, 0, 0, 2, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 6, 2, 1},
{5, 7, 0, 0, 0, 0, 8, 1, 5, 0, 0, 4, 5, 7, 4, 5, 0, 0, 0, 2},
{1, 1, 7, 4, 5, 0, 4, 1, 1, 0, 0, 8, 1, 1, 1, 1, 5, 7, 0, 8},
{1, 3, 6, 1, 3, 0, 6, 2, 3, 0, 0, 6, 2, 1, 1, 1, 1, 1, 0, 2},
{3, 0, 0, 8, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 1, 1, 3, 0, 8},
{0, 0, 0, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 1, 5, 0, 4},
{5, 7, 4, 1, 3, 0, 4, 5, 0, 7, 4, 5, 7, 0, 1, 1, 1, 1, 7, 1},
{1, 3, 6, 2, 1, 0, 1, 1, 0, 1, 1, 1, 3, 0, 8, 1, 3, 6, 2, 1},
{3, 0, 0, 0, 6, 0, 2, 3, 0, 6, 1, 1, 5, 0, 2, 3, 0, 0, 0, 8},
{5, 0, 7, 0, 0, 0, 4, 5, 0, 0, 2, 1, 1, 0, 0, 0, 0, 7, 0, 4},
{3, 0, 1, 5, 7, 4, 1, 1, 5, 0, 8, 1, 1, 5, 4, 5, 0, 6, 1, 1},
{1, 0, 1, 3, 6, 2, 1, 1, 3, 0, 4, 1, 1, 1, 1, 1, 0, 1, 1, 1},
{3, 0, 2, 0, 0, 0, 0, 8, 3, 0, 2, 3, 6, 2, 3, 8, 0, 3, 0, 2},
{5, 0, 0, 0, 0, 7, 0, 2, 5, 0, 0, 0, 0, 0, 0, 4, 0, 5, 0, 8},
{1, 5, 7, 4, 1, 1, 0, 4, 1, 5, 7, 7, 4, 5, 0, 3, 0, 2, 0, 4},
{1, 3, 6, 2, 1, 1, 0, 2, 1, 3, 6, 2, 1, 1, 0, 5, 0, 8, 0, 2},
{3, 0, 0, 0, 6, 1, 0, 8, 3, 0, 0, 0, 2, 3, 0, 8, 0, 4, 0, 4},
{0, 0, 7, 0, 0, 8, 0, 4, 5, 0, 0, 0, 4, 5, 0, 2, 0, 3, 0, 2},
{1, 1, 1, 5, 0, 4, 0, 1, 1, 0, 7, 4, 1, 1, 0, 4, 7, 5, 0, 4},
{1, 1, 1, 3, 0, 1, 0, 2, 3, 0, 6, 2, 1, 3, 0, 6, 2, 1, 0, 2},
{1, 3, 0, 0, 0, 3, 0, 8, 5, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 8},
{0, 0, 0, 7, 4, 5, 0, 4, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 4},
{7, 7, 4, 1, 1, 1, 7, 1, 1, 5, 7, 7, 7, 4, 1, 1, 5, 7, 4, 1}
};
 
//Анимация при выигрыше
void youWin(){
    lcd.clear();
    lcd.home();
    lcd.setCursor(5, 1);
    lcd.print ("YOU WIN!!!");
 do  {
  lcd.setCursor(0,3);
  lcd.print("Press OK to restart!");
  delay(500);
  lcd.setCursor(0,3);
  lcd.print("\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20");
  delay(500);
 }while (!irrecv.decode(&results) && (results.value != 16712445));
    
}


//Очистка символа
void clearSym(){
    lcd.setCursor(x%20, y%4);
    lcd.print("\x20");
}


//генерируем карту
void createMap(){
    pos = (abs(x/20)*6+abs(y/4));
    lcd.clear();
    int i = pos*4;
    while (i < pos*4+4) {
        int j = 0;
        while (j < 20){
          if (list[i][j] == 1){
           lcd.setCursor(j, i%4);
           lcd.print("\xFF");
          }
            else if (list[i][j] > 1){
                lcd.setCursor(j, i%4);
                lcd.printByte(list[i+24*abs(j/20)][j%20]-2);      
            }
            j = j + 1;
            
        }
        i = i + 1;
    }
    if (pos == 9 && key == 1){
        list[37][19] = 0;
        lcd.setCursor(19, 1);
        lcd.print("\x84");
        lcd.home();
    }
    if (pos == 5 && key == 0){
        lcd.setCursor(15,2);
        lcd.print("\x8D");
      }
}

void setup()
{
lcd.init();                     
lcd.backlight();
Serial.begin(9600); 
irrecv.enableIRIn(); 
for (int i = 0 ; i <8; i++)
{
  lcd.createChar(i, customS[i]); //генерируем кастомные символы
  
}

//Начальная заставка
lcd.home();
lcd.print("\xFF");
lcd.print("\xFF");
lcd.printByte(1);
lcd.printByte(4);
lcd.printByte(0);
lcd.print("\xFF");
lcd.printByte(1);
lcd.printByte(0);
lcd.print("\xFF");
lcd.print("\xFF");
lcd.print("\xFF");
lcd.printByte(1);
lcd.printByte(4);
lcd.print("\xFF");
lcd.printByte(1);
lcd.printByte(4);
lcd.printByte(0);
lcd.print("\xFF");
lcd.print("\xFF");
lcd.print("\xFF");
lcd.setCursor(0,1);
lcd.print("\xFF\xFF\x8D  LABIRINTH   \x8D\xFF\xFF");
lcd.setCursor(0,2);
lcd.print("\xFF");
lcd.print("\xFF");
lcd.print("\xFF");
lcd.print("\xFF");
lcd.printByte(5);
lcd.printByte(5);
lcd.print("\xFF");
lcd.print("\xFF");
lcd.printByte(3);
lcd.printByte(5);
lcd.printByte(2);
lcd.print("\xFF");
lcd.print("\xFF");
lcd.print("\xFF");
lcd.printByte(3);
lcd.printByte(5);
lcd.printByte(5);
lcd.printByte(2);
lcd.print("\xFF");
lcd.print("\xFF");

do {
  lcd.setCursor(1,3);
  lcd.print("Press OK to start!");
  delay(500);
  lcd.setCursor(1,3);
  lcd.print("\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20");
  delay(500);
  } while (!irrecv.decode(&results) && results.value != 16712445);
createMap();
lcd.setCursor(x%20, y%4);
lcd.printByte(7);
}
 
void loop() {

delay(100);
if (irrecv.decode(&results)) 
{   clearSym();
    if (results.value == 16736925) //Кнопка вверх, на вашем пульте значение будет другим
    { 
     if (list[y+24*abs(x/20)-1][x%20] == 0)
     {
     y = y - 1; 
     }
     }
    if (results.value == 16754775) //Вниз
    {
      if (list[y+24*abs(x/20)+1][x%20] == 0)
     {
     y = y + 1;
     }
    }
    if (results.value == 16761405) //Вправо
    {
      if (list[y+24*abs((x+1)/20)][(x+1)%20] == 0)
      {
     x = x + 1;
     }
     }
     if (results.value == 16720605) //Влево
     {
      if (list[y+24*abs((x-1)/20)][(x-1)%20] == 0)
      {
     x = x - 1;
      }
     } 
     if ((abs(x/20)*6+abs(y/4)) != pos) //Если вышли за экран генерим заново карту
     {
      createMap();
      }
     lcd.setCursor(x%20, y%4);
     lcd.printByte(7);
    Serial.println(results.value, HEX);  //Показания пульта пишем в консоль
    irrecv.resume(); 
    
}
if (x==39 && y==13)
    {
      youWin();
     x = 1;
     y = 9;
     }
if (x==15 && y==22){
  key = 1;
  }
}

