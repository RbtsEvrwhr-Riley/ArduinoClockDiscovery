// simple sketch to figure out the actual frequency an arduino is trying to run at
// useful for cheap micro/nano boards that don't say on the silkscreen and use a tiny crystal
// that has no writing on it, or too small writing
// how to use:
// set baud on serial monitor at 9600
// the serial string will be transmitted at 2400, 4800, 9600, 19200 and 38400
// so it should show up regardless

#include <avr/sleep.h>
#include <avr/wdt.h>

volatile bool wdt_used; // volatile means load from ram rather than a register since the interrupt is changing it
unsigned long start_micros;
unsigned long end_micros;
unsigned long diff_micros;


// watchdog interrupt
ISR (WDT_vect)
{
  wdt_disable();  // disable watchdog. this makes this a oneshot
  wdt_used = true;
}

//declare reset function at address 0
void(* resetFunc) (void) = 0;

void setup ()
{
  noInterrupts ();           // timed sequence follows

  MCUSR = 0;       // allow changes, disable reset
  WDTCSR = bit (WDCE) | bit (WDE);  // set interrupt mode and an interval
  WDTCSR = bit (WDIE);    // set WDIE, and 16 ms seconds delay
  wdt_reset();  // pat the dog
  wdt_used = false;

  interrupts ();

  start_micros = micros ();
  while (!wdt_used) { }  // wait for watchdog. if it freezes here it means we have a bad oscillator
  end_micros = micros ();

  if (end_micros > start_micros)
    diff_micros = end_micros - start_micros;
  if (end_micros < start_micros)
    diff_micros = start_micros - end_micros;

  // spam result on serial port
  // one of these should show up if we are at 9600 in the terminal!
  printme(2400);
  printme(4800);
  printme(9600);
  printme(19200);
  printme(38400);
  resetFunc();
}

void printme(int baud)
{
  Serial.begin (baud);
  Serial.flush();
  Serial.println ();
  Serial.println ();
  Serial.print ("Baud guess = ");
  Serial.print (baud);
  Serial.print (". KHz = ");
  Serial.print (end_micros - start_micros);
  if (diff_micros > 6000 and diff_micros < 24000)
  {
    Serial.print (". Program at ");
    if (diff_micros > 19000)
      Serial.print("20?");
    if (diff_micros > 12000)
      Serial.print("16");
    else
      Serial.print("8");
    Serial.println(" MHz.");
  }
  else
  {
    Serial.flush();
    Serial.print(diff_micros);
    Serial.println(" not sure. Custom crystal? 8Mhz board that has been underclocked?");
  }
  Serial.println ();
  Serial.flush();
}

void loop ()  { } // not really using it
