#include <Ps3Controller.h>

int player = 1;
int battery = 0;
int cross, square, left_trigger, right_trigger;


void notify()
{
  //--- Digital cross/square/triangle/circle button events ---
  if( Ps3.event.button_down.cross ){
  cross=1;                              
  }
  if(Ps3.event.button_up.cross) cross=0;
 
  if( Ps3.event.button_down.square ){
  square=1;
  }
  if(Ps3.event.button_up.square) square=0;

}

void onConnect(){
  Serial.println("Connected.");
}

void setup()
{
  Serial.begin(115200);

  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin("01:02:03:04:05:06");

  Serial.println("Ready.");
 
  //-------------------- Player LEDs -------------------
  Ps3.setPlayer(player);
}

void loop()
{ 
  if(!Ps3.isConnected())
  return;

  delay(70);
  Serial.write(cross);
  Serial.write(square);
 
  //---------- Analog shoulder/trigger button events ----------

  Serial.write(Ps3.data.analog.button.l2 /2);
  Serial.write(Ps3.data.analog.button.r2 /2);

}
