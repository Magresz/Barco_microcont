#include <Ps3Controller.h>

int player = 1;
int battery = 0;
int left_trigger, right_trigger, rumble, press_but;


void notify()
{
  //--- Digital cross ---

  if (Ps3.event.button_down.cross)  //Modo controle
  {
    press_but = 1;
  }
  if (Ps3.event.button_up.cross) 
    press_but = 0;

  //--- Digital Square ---

  if (Ps3.event.button_down.square) //Modo autônomo
  {
    press_but = 2;
  }
  if (Ps3.event.button_up.square){
    press_but = 0;
  }
  //--- Digital circle ---

  if (Ps3.event.button_down.circle) //Modo autônomo
  {
    press_but = 3;
  }
  if (Ps3.event.button_up.circle){
    press_but = 0;
  }

  //--- Digital pad up ---
  if (Ps3.event.button_down.up)  //Servo frente
  {
    press_but = 4;
  }
  if (Ps3.event.button_up.up){ 
    press_but = 0;
  }

  //--- Digital pad left ---

  if (Ps3.event.button_down.left)  //Servo esquerda
  {
    press_but = 5;
  }
  if (Ps3.event.button_up.left){ 
    press_but = 0;
  }

  //--- Digital pad right ---

  if (Ps3.event.button_down.right)  //Servo direita
  {
    press_but = 6;
  }
  if (Ps3.event.button_up.right){ 
    press_but = 0;
  }

}

void onConnect()
{
  Serial.println("Connected.");
}

void setup()
{
  Serial.begin(115200);

  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin("01:02:03:04:05:06");

  Serial.println("Conectado.");

  //-------------------- Player LEDs -------------------
  Ps3.setPlayer(player);
}

void loop()
{
  if (!Ps3.isConnected()) 
  return;

  delay(70);
  //---------- Botões digitais ----------
  
  Serial.write(press_but);

  //---------- Triggers ----------

  Serial.write(Ps3.data.analog.button.l2 / 2);
  Serial.write(Ps3.data.analog.button.r2 / 2);
}
