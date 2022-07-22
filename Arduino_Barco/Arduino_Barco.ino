#include <avr/io.h>
#include <avr/interrupt.h>
#include "def_principais.h"
#include "USART.h"
#include <util/delay.h>

volatile int aux, dist, time = 0;
volatile unsigned char dado, serial[4], flag = 0, var = 0, tempo = 0, aut_flag = 1, rumble = 0;

ISR(TIMER1_CAPT_vect) {         //Função de capturar o valor de ICR1 quando há a mudança de estado em PB0 - Calculo de distancia sonar
  if (tst_bit(TCCR1B, ICES1)) {
    aux = ICR1;
    clr_bit(TCCR1B, ICES1);
  }
  else {
    dist = ((ICR1 - aux) / 58) * 0.5;
    set_bit(TCCR1B, ICES1);
  }
}

ISR(TIMER2_COMPA_vect) {        //Gerador de PWM
  if (time < 2) {
    set_bit(PORTD, PD2);
  }
  else {
    clr_bit(PORTD, PD2);
  }
  if (time > 79) {            //time 79 = Periodo = 20 ms
    time = 0;
  }
  time++;
  if (flag == 1) {
    tempo++;
  }
  else tempo = 0;


}

int main(void)
{
  USART_Inic(MYUBRR);

  TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0B1) | (1 << COM0A1); //Fast PWM não invertido, timer0, controle dos motores CC
  TCCR0B = (1 << CS00);                                   //Sem prescaler
  OCR0A = 127;

  TCCR1A = 0;
  TCCR1B = (1 << ICES1) | (1 << CS11); //Borda de subida/descida, Prescaler 8, controle do sonar
  TIMSK1 = (1 << ICIE1);        //Enable da interrupção da captura de entrada

  TCCR2A = (1 << WGM21);        //Modo CTC
  TCCR2B = (1 << CS21) | (1 << CS20); //Prescaler de 32
  TIMSK2 = (1 << OCIE2A);       //Enable da interrupção de comparação
  OCR2A = 124;                  //Valor TOP da comparação T = 250us
  sei();

  DDRB = 0b10100010;
  DDRD = 0xFF;

  while (1)
  {
    /*set_bit(PORTB, PB1);
      _delay_us(10);
      clr_bit(PORTB, PB1);
      _delay_ms(100);*/

    if (tempo > 80) {
      var = 0;
      flag = 0;
    }
    //-------------------- Mantedor de estado --------------

    if (serial[0] == 1) {
      aut_flag = 1;
    }

    if (serial[1] == 1) {
      aut_flag = 0;
    }
    //-------------------- Modo Controle -------------------

    if (aut_flag == 1) {
      OCR0B = serial[3]; //Motor Esquerda - R2
      OCR0A = serial[2]; //Motor Direita - L2
    }

    //-------------------- Modo autônomo -------------------

    if (aut_flag == 0) {
      OCR0B = 255;
      OCR0A = 255;
    }

    //-------------------- Vibração ------------------------

    if (dist < 100) {
      rumble = 30  * 100 / dist;
    }
    else rumble = 0;
    USART_Transmite(rumble);
  }
}


ISR(USART_RX_vect)
{
  flag = 1;
  if (var < 4) {
    serial[var] = UDR0;
  }
  var++;
}
