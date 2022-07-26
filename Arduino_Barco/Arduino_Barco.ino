#include <avr/io.h>
#include <avr/interrupt.h>
#include "def_principais.h"
#include "USART.h"
#include <util/delay.h>

void desvio(volatile unsigned char ang);

volatile unsigned int aux, dist, time = 0;
volatile unsigned char flag = 0, aut_flag = 1, servo_flag = 0, dado , serial[4], var = 0, transmit_time = 0, pos_time = 0, ang_servo = 0, stop_servo = 0, rumble;
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
	else if (stop_servo == 0)
	{
    	clr_bit(PORTD, PD2);
	}
    
	if ((time < servo_pos[ang_servo]) && stop_servo == 1)
	{
    	set_bit(PORTD, PD2);
	}
	else if (stop_servo == 1)
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
    	if (pos_time > 25)
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
	Sonar();  	// Sonar nas portas 8(echo) e 9(trigger)
	Timer2_CTC(); // Contador para utilização do Servo Motor na porta 4 e transmissão e recepção de dados nos Pinos Rx e Tx

	sei();

	DDRB = 0b10100010; // PB1, PB5 e PB7 configurados como saída, o restante configurado como entrada
	DDRD = 0xFF;   	// Todas as portas D foram configuradas como saída

	while (1)
	{
        //-------------------- Varredura do Servo -----
        
    	if (servo_flag == 1)
    	{
        	servo_flag = 0;
        	ang_servo++;
        	if (ang_servo > 3)
        	{
            	ang_servo = 0;
        	}
    	}

    	//-------------------- Trigger do Sonar -----

    	set_bit(PORTB, PB1);
    	_delay_us(10);
    	clr_bit(PORTB, PB1);

        //-------------------- Contagem do tempo de pausa entre transmissões -----

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

    	if (serial[0] == 2)
    	{ // Botão Quadrado pressionado
        	aut_flag = 0;
    	}

    	if(serial[0]  == 3){
      	// Botão Bolinha pressionado
      	aut_flag=3;
    	}
    	//-------------------- Modo Controle -------------------

    	if (aut_flag == 1 || aut_flag == 3)
    	{// Circulo ou X entram no modo controle
        	stop_servo = 1;
        	
            if(aut_flag == 3 && serial[2] > 0)
        	{//Modo ré
          	set_bit(PORTD, 4);
        	}
        	else
        	{//Modo frente
            clr_bit(PORTD, 4);
          	OCR0B = serial[2]; // Trigger direito pressionado
        	}

        	if(aut_flag == 3 && serial[1] > 0)
        	{//Modo ré
           	set_bit(PORTD, 3);
        	}
        	else
        	{//Modo frente
            clr_bit(PORTD, 3);
          	OCR0A = serial[1]; // Trigger esquerdo pressionado
        	}
       	 
         //------- Controle do Servo -----------

        	if(serial[0] == 4)
        	{//D-PAD UP
          	ang_servo=1;
        	}
        	if(serial[0] == 5)
        	{//D-PAD LEFT
         	ang_servo=2;
        	}
        	if(serial[0] == 6)
        	{//D-PAD RIGHT
          	ang_servo=0;
        	}   

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
	if (var < 3)
	{
    	serial[var] = UDR0;
	}
	var++;
}