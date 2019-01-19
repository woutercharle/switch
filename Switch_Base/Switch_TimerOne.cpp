/*
 *  Interrupt and PWM utilities for 16 bit Timer1 on ATmega168/328
 *  Original code by Jesse Tane for http://labs.ideo.com August 2008
 *  Modified March 2009 by Jérôme Despatis and Jesse Tane for ATmega328 support
 *  Modified June 2009 by Michael Polli and Jesse Tane to fix a bug in setPeriod() which caused the timer to stop
 *  Modified June 2011 by Lex Talionis to add a function to read the timer
 *  Modified Oct 2011 by Andrew Richards to avoid certain problems:
 *  - Add (long) assignments and casts to TimerOne::read() to ensure calculations involving tmp, ICR1 and TCNT1 aren't truncated
 *  - Ensure 16 bit registers accesses are atomic - run with interrupts disabled when accessing
 *  - Remove global enable of interrupts (sei())- could be running within an interrupt routine)
 *  - Disable interrupts whilst TCTN1 == 0.  Datasheet vague on this, but experiment shows that overflow interrupt
 *    flag gets set whilst TCNT1 == 0, resulting in a phantom interrupt.  Could just set to 1, but gets inaccurate
 *    at very short durations
 *  - startBottom() added to start counter at 0 and handle all interrupt enabling.
 *  - start() amended to enable interrupts
 *  - restart() amended to point at startBottom()
 * Modified 7:26 PM Sunday, October 09, 2011 by Lex Talionis
 *  - renamed start() to resume() to reflect it's actual role
 *  - renamed startBottom() to start(). This breaks some old code that expects start to continue counting where it left off
 *
 *  This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  See Google Code project http://code.google.com/p/arduino-timerone/ for latest
 */

#include "Switch_TimerOne.h"

#ifdef ARDUINO

Switch::TimerOne Timer1;              // pre-instantiate

ISR(TIMER1_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  if (0x0 != Timer1.ISRCallback)
  {
    Timer1.ISRCallback ();
  }
}

/*!
  \brief Default constructor.
 */
Switch::TimerOne::TimerOne ()
: ISRCallback (0x0)
{
}

/*!
  \brief Destructor.
 */
Switch::TimerOne::~TimerOne ()
{
  ISRCallback = 0x0;
}

/*!
  \brief Initializes the timer.

  You must call this method first to use any of the other methods.

  \note This breaks analogWrite () for digital pins 9 and 10 on Arduino Uno.
 */
void Switch::TimerOne::Begin (const uint32_t& i_periodUs)
{
  TCCR1A = 0;                     // clear control register A
  TCCR1B = _BV(WGM13);            // set mode 8: phase and frequency correct pwm, stop the timer
  SetPeriod (i_periodUs);         // set the timer period
}

/*!
  \brief Sets the timer period in microseconds.

  Sets the period in microseconds. The minimum period or highest frequency this library supports is 1 microsecond or 1 MHz.
  The maximum period is 8388480 microseconds or about 8.3 seconds.

  \param [in] i_periodUs The timer period in microseconds.

  \note Setting the period will change the attached interrupt and both PWM outputs' frequencies and duty cycles simultaneously.
 */
void Switch::TimerOne::SetPeriod (const uint32_t& i_periodUs)		// AR modified for atomic access
{
  uint32_t cycles = (F_CPU / 2000000) * i_periodUs;                                 // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  if (cycles < RESOLUTION)              m_clockSelectBits = _BV(CS10);              // no prescale, full xtal
  else if ((cycles >>= 3) < RESOLUTION) m_clockSelectBits = _BV(CS11);              // prescale by /8
  else if ((cycles >>= 3) < RESOLUTION) m_clockSelectBits = _BV(CS11) | _BV(CS10);  // prescale by /64
  else if ((cycles >>= 2) < RESOLUTION) m_clockSelectBits = _BV(CS12);              // prescale by /256
  else if ((cycles >>= 2) < RESOLUTION) m_clockSelectBits = _BV(CS12) | _BV(CS10);  // prescale by /1024
  else         cycles = RESOLUTION - 1, m_clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum

  uint8_t oldSREG = SREG;
  cli ();							// Disable interrupts for 16 bit register access
  ICR1 = (uint16_t)cycles;                                          // ICR1 is TOP in p & f correct pwm mode
  SREG = oldSREG;

  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));                     // equals pause
  TCCR1B |= m_clockSelectBits;                                          // reset clock select register, and starts the clock (equals resume)
}

/*!
  \brief Generates a PWM waveform on the specified pin.

  Output pins for Timer1 are PORTB pins 1 and 2, so you have to choose between these two, anything else is ignored.
  On Arduino, these are digital pins 9 and 10, so those aliases also work. Output pins for Timer3 are from PORTE
  and correspond to 2, 3 & 5 on the Arduino Mega. The duty cycle is specified as a 10 bit value, so anything between
  0 and 1023.

  \param [in] i_pin Pin on which the PWM signal is generated (9 or 10).
  \param [in] i_duty 10 bit duty cycle (max value is 1023)
  \param [in] i_periodUs The signal period in microseconds.

  \note You can optionally set the period with this function if you include a value in microseconds as the last parameter when you call it.
 */
void Switch::TimerOne::PWM (uint8_t i_pin, uint16_t i_duty, const uint32_t& i_periodUs)  // expects duty cycle to be 10 bit (1024)
{
  // set the period
  if (i_periodUs > 0)
  {
    SetPeriod (i_periodUs);
  }

  // configure the PWM pins
  if (i_pin == 9)
  {
    DDRB |= _BV(PORTB1);                                   // sets data direction register for pwm output pin
    TCCR1A |= _BV(COM1A1);                                 // activates the output pin
  }
  else if (i_pin == 10)
  {
    DDRB |= _BV(PORTB2);
    TCCR1A |= _BV(COM1B1);
  }

  // configure the duty
  SetPWMDuty (i_pin, i_duty);

  // make sure the clock is running
  Resume ();			// Lex - make sure the clock is running.  We don't want to restart the count, in case we are starting the second WGM
					// and the first one is in the middle of a cycle
}

/*!
  \brief Sets the PWM duty regardless the period.

  A fast shortcut for setting the PWM duty for a given pin if you have already set it up by calling pwm () earlier.
  This avoids the overhead of enabling PWM mode for the pin, setting the data direction register, checking for
  optional period adjustments etc. that are mandatory when you call PWM ().

  \param [in] i_pin Pin on which the PWM signal is generated (9 or 10).
  \param [in] i_duty 10 bit duty cycle (max value is 1023)
 */
void Switch::TimerOne::SetPWMDuty (uint8_t i_pin, uint16_t i_duty)
{
  // compute the duty cycle
  uint32_t dutyCycle = ((uint32_t)ICR1*i_duty) >> 10;   // ICR1 holds the total cycle

  // set in the corresponding output compare register
  uint8_t oldSREG = SREG;
  cli ();
  if (i_pin == 9)
  {
    OCR1A = dutyCycle;
  }
  else if (i_pin == 10)
  {
    OCR1B = dutyCycle;
  }
  SREG = oldSREG;
}

/*!
  \brief Turns PWM off for the specified pin so you can use that pin for something else.

  \param [in] i_pin Pin for which to turn off the PWM signal (either 9 or 10).
 */
void Switch::TimerOne::DisablePWM (uint8_t i_pin)
{
  if (i_pin == 9)
  { // clear the bit that enables PWM on PB1
    TCCR1A &= ~_BV(COM1A1);
  }
  else if (i_pin == 10)
  { // clear the bit that enables PWM on PB2
    TCCR1A &= ~_BV(COM1B1);
  }
}

/*!
  \brief Calls a function at the specified interval in microseconds.

  Be careful about trying to execute too complicated of an interrupt at too high of a frequency, or the CPU may never enter the
  main loop and your program will 'lock up'.

  \param [in] i_pISR Pointer to the interrupt service routine that will handle the interrupt.
 */
void Switch::TimerOne::AttachInterrupt (void (*i_pISR) ())
{
  ISRCallback = i_pISR;                                    // register the user's callback with the real ISR
  TIMSK1 = _BV(TOIE1);                                     // sets the timer overflow interrupt enable bit
	// might be running with interrupts disabled (eg inside an ISR), so don't touch the global state
//  sei();
}

/*!
  \brief Disables the attached interrupt.
 */
void Switch::TimerOne::DetachInterrupt ()
{
  TIMSK1 &= ~_BV(TOIE1);                                   // clears the timer overflow interrupt enable bit
  ISRCallback = 0x0;
  // timer continues to count without calling the isr
}

/*!
  \brief Resets the timer counter and starts the timer.

  \note Clears the interrupt.
 */
void Switch::TimerOne::Start ()	// AR addition, renamed by Lex to reflect it's actual role
{
  uint16_t tcnt1;

  TIMSK1 &= ~_BV(TOIE1);        // AR added - clears the timer overflow interrupt enable bit
  GTCCR |= _BV(PSRSYNC);   		// AR added - reset prescaler (NB: shared with all 16 bit timers);

  // reset the counter
  uint8_t oldSREG = SREG;				// AR - save status register
  cli();						// AR - Disable interrupts
  TCNT1 = 0;
  SREG = oldSREG;          		// AR - Restore status register

  // resume the clock
	Resume();

  do
  {	// Nothing -- wait until timer moved on from zero - otherwise get a phantom interrupt
    oldSREG = SREG;
    cli();
    tcnt1 = TCNT1;
    SREG = oldSREG;
  }
  while (tcnt1 == 0);

//  TIFR1 = 0xff;              		// AR - Clear interrupt flags
//  TIMSK1 = _BV(TOIE1);              // sets the timer overflow interrupt enable bit
}

/*!
  \brief Stops the timer.

  Clears all clock select bits.
 */
void Switch::TimerOne::Pause ()
{
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
}

/*!
  \brief Resumes the clock.

  Call this method to ensure the clock is running.
 */
void Switch::TimerOne::Resume ()				// AR suggested
{
  TCCR1B |= m_clockSelectBits;
}

/*!
  \brief Gets the value of the timer in microseconds.

  \return The value of the timer in microseconds.
 */
uint32_t Switch::TimerOne::Read ()
{									//rember! phase and freq correct mode counts up to then down again
  uint32_t tmp;				// AR amended to hold more than 65536 (could be nearly double this)
  uint16_t tcnt1;				// AR added

  // read the counter
	uint8_t oldSREG = SREG;
  cli ();
  tmp = TCNT1;
	SREG = oldSREG;

	uint8_t scale = 0; // no prescale if 1 == m_clockSelectBits
  if (2  == m_clockSelectBits)
  { // x8 prescale
		scale = 3;
	}
  else if (3  == m_clockSelectBits)
  { // x64
		scale = 6;
	}
  else if (4  == m_clockSelectBits)
  { // x256
		scale = 8;
	}
  else if (5  == m_clockSelectBits)
  { // x1024
		scale = 10;
	}

	do
  {	// Nothing -- max delay here is ~1023 cycles.  AR modified
		oldSREG = SREG;
		cli ();
		tcnt1 = TCNT1;
		SREG = oldSREG;
	}
  while (tcnt1==tmp); //if the timer has not ticked yet

	//if we are counting down add the top value to how far we have counted down
  if (tcnt1 < tmp)
  {
    tmp = (uint32_t)(ICR1 - tcnt1) + (uint32_t)ICR1; // AR amended to add casts and reuse previous TCNT1
  }
	return ((tmp*1000L)/(F_CPU /1000L)) << scale;
}

#endif // ARDUINO
