#include "include.h"
#include "Cfg.h"
#include "SdFat.h"
#include "GlobalVariableNoncfg.h"

Adafruit_MLX90614 mlx = Adafruit_MLX90614();        //seleciona o sensor na library e atribui um nome no caso mlx
Adafruit_MLX90614 mlx2 = Adafruit_MLX90614();

LiquidCrystal_I2C lcd(LCDid, lcdColumns, lcdRows); //define o endereco padrao para lcd e o tamnaho deste

TaskHandle_t Task2;                              // inicia o traskhandler para dar um nucleo selecionado para a trask

bool sdsave();                                  // declara funcao sdsave usada para salvar os dados                  

void Task2code( void * pvParameters ){          // um loop feito para mostrar os dados na tela, roda independete do codigo core
//  lcd.blink();                                // usar para debug, mostra onde o cursor esta
  lcd.setCursor(0, 0);                          //coloca o cursor na posicao 0,0
  lcd.print("Temp ");                           //escreve Temp_      (_ = espaço)
  lcd.setCursor(11, 0);                         //coloca o cursor na posicao 11,0
  lcd.print("Temp ");                           //escreve Temp_ 
  lcd.setCursor(0, 1);                          //coloca o cursor na posicao 0,1 (linha de baixo)
  lcd.print("Pressao ");                        //escreve Pressao_
  while (true)                                  //loop para atualizar apenas os valores significativos
  {
  lcd.setCursor(4, 0);                          //coloca o cursor na posicao 4,0
  lcd.print(tempc);                             //printa pneu1  aka L
  lcd.print("C");                               //escreve C (unidade de madida Graus Ceusios)
  lcd.setCursor(16, 0);                         //coloca o cursor na posicao 16,0
  lcd.print(tempc2);                            //printa pneu2  aka R
  lcd.print("C");                               //escreve C     

  float UnderPressure = (RawPressure*0.0293) - 12.5;   // funcao usada para transformar o analogread de 0 a 4096 em o a 100 bar seguindo uma equacao linear

  lcd.setCursor(8, 1);          //coloca o cursor na posicao 8,1
  lcd.print(UnderPressure);     //mostra a pressao
  lcd.print("Bar");             //escreve Bar(unidade de medida de pressao)
  delay(lcdSlow);               //delay para o display nao atualizar muito rapido causando blurr 
  }
}


void setup() {

//  Serial.begin(115200);        // usar para debug, liga serial console

  SPI.begin(SCK, MISO, MOSI, CS);     // inicia a comunicacao spi usada no sdcard

  Wire.begin(SDA_1, SCL_1); //coms rate not set 'std 100000' lcd
  Wire1.begin(SDA_2, SCL_2); //       (SDA_2, SCL_2, 100000) ir        
  
  delay(100);   // delay para os sensores ativar o i2c 

  mlx.begin(IRL, Wire1);  //inicia o sensor no i2c buss secundario
  delay(10);              //espera o i2c buss 
  mlx2.begin(IRR, Wire1); //inicia o sensor no i2c buss secundario

  lcd.init();            // inicializa o lcd no wire 
  lcd.clear();           // garante que em caso do esp reiniciar ele limpe o display antes de escrever novamente
  lcd.backlight();       // inicia o controle de brilho

  readingID++;          // conta quantas medidas foram feitas para salvar no sd //basicamente um id de medidas

  if (!SD.begin(CS)) {            //checa se o cartao esta disponivel, caso nao estiver desativa o save de dados
    sdpresent = false;            // desliga o sdcard data logger
  }

  xTaskCreatePinnedToCore(       //cria a task responsavel por fazer update do display
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(100); 
}

void loop() {

    tempc = mlx.readObjectTempC(); // pega a temp do freio L
    delay(timeout);                // delay responsavel por garantir que o sensor consiga transmitir os dados antes de chamar o outro sensor
    tempc2 = mlx2.readObjectTempC();// pega a temp do freio R
    RawPressure = analogRead(Vanalog);//pega o valor de 0 a 4096 do adc

//  Serial.print(tempc); //printa no serial monitor a temp em c para debugg
//  Serial.print(tempc2); //printa no serial monitor a temp em c para debugg

 if(sdpresent == true){  // verifica se sd esta presente ou abilitado, caso nao estiver pula a funcao de data loggin

    sdsave();            //chama a funcao de salvamento de dados 

 }
}

bool sdsave(){     //salva os dados no sd

  tempR = tempc2;  //transfere os dados tempc2 para tempR
  tempL = tempc;   //transfere os dados tempc2 para tempRL
  UnderPressureeee = RawPressure;
  dataMessage = String(readingID) + " " + String(tempL) + " " + String(tempR) + " " + String(UnderPressureeee) + "\r\n"; // junta todos os dados em uma string que sera escrita no sd 

//  Serial.print("Save data: ");  // debugg only
//  Serial.println(dataMessage);  // debugg only

  myFile = SD.open("data.txt", FILE_WRITE);  //abre o arquivo
  if (myFile) {                              //verifica se o arquivo foi aberto

    myFile.close();                          // fecha o arquivo para salvar tempo de processamento e memoria

  } 
  return true;
}
