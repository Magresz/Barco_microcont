void Fast_Pwm()
{
    TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0B1) | (1 << COM0A1); // Fast PWM não invertido, timer0, controle dos motores CC
    TCCR0B = (1 << CS00);                                                 // Sem prescaler
    OCR0A = 127;
}

void Sonar()
{
    TCCR1A = 0;
    TCCR1B = (1 << ICES1) | (1 << CS11); // Borda de subida/descida, Prescaler 8, controle do sonar
    TIMSK1 = (1 << ICIE1);               // Enable da interrupção da captura de entrada
}

void Timer2_CTC()
{
    TCCR2A = (1 << WGM21);              // Modo CTC
    TCCR2B = (1 << CS21) | (1 << CS20); // Prescaler de 32
    TIMSK2 = (1 << OCIE2A);             // Enable da interrupção de comparação
    OCR2A = 124;                        // Valor TOP da comparação T = 250us
}

void desvio(volatile unsigned char ang)
{
    if (ang == 0 || ang == 1 || ang == 3)
    {
        OCR0B = 0;
        OCR0A = 255;
    }
    if (ang == 2)
    {
        OCR0A = 0;
        OCR0B = 255;
    }
}
