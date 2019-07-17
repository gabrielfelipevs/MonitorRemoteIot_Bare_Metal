#include <avr/io.h>
#include <util/delay.h>

//--------Variáveis globais caso necessário alterar a pinagem
#define DataBus PORTD //Low(0)-High(1)
#define DataDir_DataBus DDRD //Input(0)-Output(1)
#define ControlBus PORTC //Low(0)-High(1)
#define DataDir_ControlBus DDRC //Input(0)-Output(1)
#define EN 5 //Porta 10 -> ativado por um pulso High-Low
#define RW 4 //Porta 9 -> 0 = write to LCD; 1 = read from LCD
#define RS 3 //Porta 8 -> 0 = enviar comando, 1 = enviar caractere

//--------Flags para comandos do manual do LCD----
// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00
// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00
// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00
// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x8DOTS 0x00
#define LCD_5x10DOTS 0x04
// commands
#define LCD_LINEONE 0x80
#define LCD_LINETWO 0xc0
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _row_offsets[4];

//------------Início das funções-------------
//Manda um pulso para o pino Enable (para execução do comando)
void Pulse_Enable()
{
 ControlBus &= ~(1 << EN);
 _delay_us(1);
 ControlBus |= (1<<EN);
 _delay_us(1);
 ControlBus &= ~(1 << EN);
 _delay_us(100);
}

//Função para mandar um comando
void Send_A_Command(uint8_t value){
 ControlBus &= ~(1<<RS | 1<<RW);

 //Manda o primeiro nibble (metade)
 DataBus = value;
 Pulse_Enable();

 //Manda o segundo nibble
 DataBus = (value << 4);
 Pulse_Enable();
}

//Função para mandar um caractere
void Send_A_Character(unsigned char c_character){
 ControlBus &= ~(1<<RW);
 ControlBus |= 1<<RS;

 DataBus = c_character;
 Pulse_Enable();

 DataBus = (c_character << 4);
 Pulse_Enable();
}

//Função para mandar uma string
void Send_A_String(char *p_StringOfCharacters)
{
 while(*p_StringOfCharacters > 0)
 {
 Send_A_Character(*p_StringOfCharacters++);
 _delay_ms(5);
 }
}

//Função para configurar o LCD na inicialização
void LCD_Init()
{   //Seta todos os pinos como OUTPUT
  DataDir_ControlBus |= 1<<EN | 1<<RW | 1<<RS;
  DataDir_DataBus = 0xff;
  _delay_us(50000);
  //Seta RW, RS e EN como LOW para começar a mandar comandos
  ControlBus &= ~ ((1<<RW)|(1<<RS)|(1<<EN));
   //Como default, o LCD vem no modo 8 bits
 //São necessárias 3 tentativas para mudar para modo 4 bits:
  Send_A_Command(0x03);
  _delay_us(4500);
  Send_A_Command(0x03);
  _delay_us(4500);
  Send_A_Command(0x03);
  _delay_us(150);
  Send_A_Command(0x02);

   //Manda as configurações para o LCD
  Send_A_Command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
  Send_A_Command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
  //Clear display
   Send_A_Command(LCD_CLEARDISPLAY);
  _delay_us(2000);
  //Set entry mode
  Send_A_Command(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);

   //Variáveis para usar o scroll
  _row_offsets[0] = 0x00;
  _row_offsets[1] = 0x40;
  _row_offsets[2] = 0x00 + 16;
  _row_offsets[3] = 0x40 + 16;
}

//----------Comandos para o usuário-------------
void LCD_Clear()
{
  Send_A_Command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
  _delay_us(2000);
}
void LCD_ReturnHome()
{
  Send_A_Command(LCD_RETURNHOME); // set cursor position to zero
  _delay_us(2000);
}
void LCD_LineTwo()
{
  Send_A_Command(0x80 + 0x40 + 5);
}
void LCD_CENTER()
{
  Send_A_Command(0x80 + 0x4 );
}
void LCD_LineTwoINCENDIO()
{
  Send_A_Command(0x80 + 0x40 + 4 );
}
void SetCursor(uint8_t u8_col, uint8_t u8_row);
void LCD_DisplayOn();
void LCD_DisplayOff();
void LCD_CursorOff();
void LCD_CursorOn();
void LCD_CursorBlinkOff();
void LCD_CursorBlinkOn();
void LCD_ScrollDisplayLeft()
{
  Send_A_Command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LCD_ScrollDisplayRight();
void LCD_LeftToRight();
void LCD_RightToLeft();
