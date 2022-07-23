#include <avr/io.h>
#include <avr/interrupt.h>
#include "def_principais.h"
#include "USART.h"
#include <util/delay.h>

void desvio(volatile unsigned char ang);

volatile unsigned int aux, dist, time = 0;
volatile unsigned char flag = 0, aut_flag = 1, servo_flag = 0, dado, serial[4], var = 0, transmit_time = 0, pos_time = 0, ang_servo = 0, stop_servo = 0;
unsigned char servo_pos[4] = {2, 6, 10, 6};

ISR(TIMER1_CAPT_vect)
{ // Função de capturar o valor de ICR1 quando há a mudança de estado em PB0 - Calculo de distancia sonar
    if (tst_bit(TCCR1B, ICES1))
    {
        aux = ICR1;
        clr_bit(TCCR1B, ICES1);
    }
    else
    {
        dist = ((ICR1 - aux) / 58) * 0.5;
        set_bit(TCCR1B, ICES1);
    }
}

ISR(TIMER2_COMPA_vect)
{ // Contador para o servo e tempo de transmissão
    if ((time < servo_pos[ang_servo]) && stop_servo == 0)
    {
        set_bit(PORTD, PD2);
    }
    else
    {
        clr_bit(PORTD, PD2);
    }
    time++;
    if (time > 79)
    { // time 79 = Periodo = 20 ms
        time = 0;
        if (stop_servo == 0)
        {
            pos_time++;
        }
        if (pos_time > 50)
        {
            pos_time = 0;
            servo_flag = 1;
        }
    }
    if (flag == 1)
    {
        transmit_time++;
    }
    else
        transmit_time = 0;
}

int main(void)
{
    USART_Inic(MYUBRR);

    Fast_Pwm();   // Pwm rápido nas portas 5 e 6 do Arduíno
    Sonar();      // Sonar nas portas 8(echo) e (trigger)
    Timer2_CTC(); // Contador para utilização do Servo Motor e transmissão de dados

    sei();

    DDRB = 0b10100010; // PB1, PB5 e PB7 configurados como saída, o restante configurado como entrada
    DDRD = 0xFF;       // Todas as portas D foram configuradas como saída

    while (1)
    {
        if (servo_flag == 1)
        {
            servo_flag = 0;
            ang_servo++;
            if (ang_servo > 3)
            {
                ang_servo = 0;
            }
        }

        set_bit(PORTB, PB1);
        _delay_us(10);
        clr_bit(PORTB, PB1);

        if (transmit_time > 80)
        {
            var = 0;
            flag = 0;
        }

        //-------------------- Mantedor de estado Joystick -----

        if (serial[0] == 1)
        { // Botão X pressionado
            aut_flag = 1;
        }

        if (serial[1] == 1)
        { // Botão Quadrado pressionado
            aut_flag = 0;
        }
        //-------------------- Modo Controle -------------------

        if (aut_flag == 1)
        {
            stop_servo = 1;
            OCR0B = serial[3]; // Trigger direito pressionado
            OCR0A = serial[2]; // Trigger esquerdo pressionado
        }

        //-------------------- Modo autônomo -------------------

        if (aut_flag == 0)
        {
            if (dist < 50)
            {
                stop_servo = 1;
                desvio(ang_servo);
            }
            else
            {
                OCR0A = 255;
                OCR0B = 255;
                stop_servo = 0;
            }
        }
    }
}

ISR(USART_RX_vect)
{
    flag = 1;
    if (var < 4)
    {
        serial[var] = UDR0;
    }
    var++;
}
