#include "include.h"
#include "Cfg.h"
#include "SdFat.h"
#include "GlobalVariableNoncfg.h"

Adafruit_MLX90614 mlx = Adafruit_MLX90614(); //seleciona o sensor na library
Adafruit_MLX90614 mlx2 = Adafruit_MLX90614();

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); //define o endereco padrao para lcd e o tamnaho deste

TaskHandle_t Task2;

bool sdsave();


SdFat SD;

#define SD_FAT_TYPE 3

File myFile;

void Task2code( void * pvParameters ){
  while (true)
  {
  lcd.setCursor(0, 0);
  lcd.print("temp ");
  lcd.print(tempc);      //pneu1  aka L
  lcd.print("c"); 
  lcd.setCursor(8, 0);
  lcd.print("temp ");
  lcd.print(tempc2);     //pneu2  aka R
  lcd.print("c");     

  float UnderPressure = (RawPressure*0.0293) - 12.5;

  lcd.setCursor(0, 1);
  lcd.print("pressao ");
  lcd.print(UnderPressure);
  delay(lcdSlow);
  }
}


void setup() {
  Serial.begin(115200);
  SPI.begin(SCK, MISO, MOSI, CS);

  Wire.begin(SDA_1, SCL_1); //cons rate not set 'std 100000' lcd
  Wire1.begin(SDA_2, SCL_2); //       (SDA_2, SCL_2, 100000) ir        

  mlx.begin(IRL); //inicia o sensor no i2c buss
  mlx2.begin(IRR);

  lcd.init();                      // inicializa o lcd no wire 
  lcd.backlight();

  readingID++;

  if (!SD.begin(CS)) {            //checa se o cartao esta disponivel, caso nao estiver desativa o save de dados
    sdpresent = false;
    return;
  }
   myFile = SD.open("test.txt", FILE_WRITE);


  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 
}



void loop() {
    tempc = mlx.readObjectTempC(); // pega a temp do freio L
    delay(timeout);
    tempc2 = mlx2.readObjectTempC();// pega a temp do freio R
    RawPressure = analogRead(Vanalog);
//  Serial.print(tempc); //printa no serial monitor a temp em c para debugg
//  Serial.print(tempc2); //printa no serial monitor a temp em c para debugg
 if(sdpresent == true){
    sdsave();
 }

}

bool sdsave(){
  tempR = tempc2;
  tempL = tempc;
  UnderPressureeee = RawPressure;
  dataMessage = String(readingID) + "," + String(tempL) + "," + String(tempR) + "," +String(UnderPressureeee) + "\r\n";
//  Serial.print("Save data: ");
//  Serial.println(dataMessage);

  myFile = SD.open("data.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {

    Serial.print(dataMessage);
    myFile.close();

  } 

  return true;
}