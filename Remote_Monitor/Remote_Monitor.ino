/* Projeto de Sistmas Embarcados
 *  Gabriel Felipe Vieira de Sousa
 *  Samuel Nicolas Nobre
 *  FEELT - 2018
 *  */

#include <SPI.h>
#include <Ethernet.h> // Initialize the libraries.
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd.c"
#include "ad.c"
#define F_CPU 16000000
#include <stdlib.h>





byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};; 
IPAddress ip(10, 0, 66 , 199); // casa= 192, 168, 1, 16
byte gateway[] = { 10, 0, 64, 1 };// casa= 192, 168, 1, 1
byte subnet[] = { 255, 255, 252, 0 };// casa =  255, 255, 255, 0

EthernetServer server(80); // Assigning the port forwarded number. It's almost always 80.

String readString; // We will be using strings to keep track of things.
int val;    // Using val as a variable for the PIR status.
int pir = 2;

int8_t ad = 0;
int8_t temp = 0;
char snum[10];
int8_t temperatura = 0;
char TEMP[3];
//int i = 1; // Loop variable

static int uart_putchar (char c, FILE *stream) {
  Serial.write(c);
  return 0;
}

void pwm_init(void)
{
  DDRD |= (1 << DDD3); //define pino D3 como saída 
 
  TCCR2A |= (1 << COM2B1);// modo de saída comparador (compare match)ok 

  TCCR2A |= (1 << WGM21) | (1 << WGM20);//ok necessário setar para ter o modo fast pwm

  TCCR2A |= (1 << WGM22);// Timer/Counter Mode of Operation = Fast PWM-10-bit, TOP = 0x03FF,Update of OCR1x at = BOTTOM, TOV1 Flag Set on = TOP (Pag. 132)

  TCCR2B |= (1 << CS21);  // set prescaler to 8 and starts PWM
}


void setup()
{

  Ethernet.begin(mac, ip);
  server.begin();
 
  
  //---- Direcionando o fluxo de dados de printf para a Serial (UART) -------
  static FILE uartout = {0} ;
  fdev_setup_stream (&uartout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &uartout;
  LCD_Init();
  LCD_LineTwo();
  Serial.begin(9600);
  ad_init();
  pwm_init();
  DDRB |= (1 << DDB1);

}

  
void loop(){

  ad = ADCL;  // ADCL must be read first, then ADCH
    ad = (ADCH << 8) + ad;

    
    temperatura = ((ad * 5) / (1023 * 0.01));
    
    
    itoa(temperatura, TEMP, 10);
    //printf("O valor da temperatura %c: %s\r\n", 233, TEMP);

   
    EthernetClient client = server.available();
    if (client) {
      while (client.connected())
      {
        if (client.available())
        {                     // This is all where we start up the server and strings.
          char c = client.read();                     
          if (readString.length() < 100)
          {
                  readString += c;
                  
          if (c == '\n') {
            Serial.println(readString);                      // And here we begin including the HTML
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("<hmtl>");

client.println("<head>");
client.println("Monitor de Temperatura");
client.println("</head>");
client.println("<title>");
client.println("Monitor de temperatura");
client.println("</title>");
//client.println("<meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'></meta>");
client.println("<meta charset='UTF-8' />");
client.println("<body bgcolor=black>");
client.println("<font color=white>");
client.println("<meta http-equiv=\"refresh\" content=\"4\">"); // This is used to refresh the page so
client.println("<center>");                                      // we can see if there is Motion or not.
client.println("<b>");
client.println("Monitoramento Remoto de temperatura");
client.println("</br>");
client.println("Sistemas embarcados - FEELT - UFU");
client.println("</br>");
//client.println("--");
//client.println("</br>");
client.println("<img src='https://image.ibb.co/hcVMHJ/logoinvert.jpg' alt='logoinvert' border='0'></a><br /><br />");
client.println("</b>");
client.println("<p>");
client.println("<table border=0 width=200>");
client.println("<tr>");
client.println("<td align=center>");
client.println("<font color=white>");
client.println("A temperatura atual é:");
client.println("</td>");
client.println("</tr>");
client.println("<tr>");
client.println("<td align=center>");
client.println("<font color = red size=10>");//ok

client.println(TEMP);
client.println("° C");
client.println("</td>");
client.println("</tr>");
client.println("</table>");
client.println("<p>");
client.println("<FORM>");
client.println("<INPUT type=button value=LIGAR_LCD onClick=window.location='/?lighton1\'>");
client.println("<INPUT type=button value=DESLIGAR_LCD onClick=window.location='/?lightoff1\'>");
client.println("</FORM>");   // Above and below you'll see that when we click on a button, it adds a little snippet
client.println("<FORM>");    // to the end of the URL. The Arduino watches for this and acts accordingly.
client.println("<INPUT type=button value=Refrigeração_Automática onClick=window.location='/?Lig_ar\'>");
client.println("<INPUT type=button value=Refrigeração_Desligada onClick=window.location='/?off_ar\'>");
//client.println("<INPUT type=button value=Servo-90 onClick=window.location='/?serv90\'>");
//client.println("
client.println("</FORM>");
client.print("<table border=1 width=200>");
client.print("<tr>");
client.print("<td align=center>");
client.print("<font color=white size=3>");
client.print("Status:  ");
client.print("</td>");
client.print("</tr>");
client.print("<tr>");
client.print("<td align=center>");
client.print("<font color=white size=3>");  
val = OCR2B;
          if (temperatura<100)
          {
          if (val > 0){
           client.print("Climatizando Ambiente");
          }
         else {
          client.print("Climatização Desligada");
         }
          }
          else {
            client.print("Alerta Incêndio");
            PORTB |= (1 << PORTB1); 
            _delay_ms(500); 
            PORTB &= ~(1 << PORTB1); 
            _delay_ms(500); 
            
          }
client.print("</td>");
client.print("</tr>");
client.print("</table>");
client.println("</center>");
client.println("</font>");
client.println("</body>");
client.println("</html>");
 
          delay(1);
      
          if(readString.indexOf("?lighton") >0)     // these are the snippets the Arduino is watching for.
          {
              LCD_Clear();
              _delay_us(2000);
              Send_A_String("TEMPERATURA (C)");
              LCD_LineTwo();
              Send_A_String(TEMP);
          }
          else{
          if(readString.indexOf("?lightoff") >0)
          {
              LCD_Clear();
              _delay_us(2000);
              LCD_CENTER();
              Send_A_String("DESLIGADO");
              LCD_LineTwoINCENDIO();
              Send_A_String(" ");
          }
          
          else{
            if(readString.indexOf("?Lig_ar") >0)
            {
              OCR2B = 255; //
              _delay_ms(20);
             OCR2B = ad;
            }
          
          else{
            if(readString.indexOf("?off_ar") >0)
            {
              OCR2B = 0;
            }
          
          }
          }
          }
          readString="";
          client.stop();            // End of session.

        }
      }
    }
  }
 }
}






