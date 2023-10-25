int timeout = 5; // tempo minimo de codigo 5ms 

#define Vanalog 2  // pino analog do psensor

#define SDA_1 33  // define o i2c lcd
#define SCL_1 32 

#define SDA_2 27  // define o i2c ir
#define SCL_2 26

#define SCK  17  //define os pinos do cartao sd
#define MISO  19
#define MOSI  23
#define CS  5

int lcdColumns = 16;         // linha coluna lsd
int lcdRows = 2;

SPIClass spi = SPIClass(HSPI); // Hspi ou Vspi funciona depende do sd

bool sdpresent = true;         //define se o sd esta ou nao inserido

int lcdSlow = 250; //tempo do lcd dar refresh

const byte IRL = 0x5b;          //0x5b é o factry addres da pra mudar L= left R= right
const byte IRR = 0x5b;



