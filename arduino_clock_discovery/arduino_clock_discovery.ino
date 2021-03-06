// simple sketch to figure out the actual frequency an arduino is trying to run at
// useful for cheap micro/nano boards that don't say on the silkscreen and use a tiny crystal
// that has no writing on it, or too small writing
// how to use:
// set baud on serial monitor at 9600
// the serial string will be transmitted at 2400, 4800, 9600, 19200 and 38400
// so it should show up regardless

// AUTHOR: MKB

/************************************************************
 *  The MIT License (MIT)

Copyright © 2021 <copyright holders>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 ************************************************************/

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
