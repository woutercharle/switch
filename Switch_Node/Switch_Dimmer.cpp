/*?*************************************************************************
*                           Switch_Dimmer.cpp
*                           -----------------------
*    copyright            : (C) 2013 by Wouter Charle
*    email                : wouter.charle@gmail.com
*
*    DISCLAIMER OF DAMAGES
*    ---------------------
*    Wouter Charle has made every effort possible to ensure that the software
*    is free of any bugs or errors, however in no way is the software to
*    be considered error or bug free. You assume all responsibility for
*    any damages or lost data that may result from any errors or bugs in
*    the software.
*
*    IN NO EVENT WILL VISION++ BE LIABLE TO YOU FOR ANY GENERAL, SPECIAL,
*    INDIRECT, CONSEQUENTIAL, INCIDENTAL OR OTHER DAMAGES ARISING OUT OF
*    THIS LICENSE.
*
*    In no case shall Wouter Charle's liability exceed the purchase price for
*    the software or services.
*
***************************************************************************/

#include "Switch_Dimmer.h"

#ifdef ARDUINO

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Debug.h"
#include "../Switch_Base/Switch_StdLibExtras.h"
#include "../Switch_Base/Switch_Utilities.h"

// thirdparty includes
#include "Switch_TimerOne.h"

Switch::Dimmer Dimmer; //pre-instantiate

volatile uint8_t vm_clockSource = 0;
volatile uint8_t vm_dimmerLevel = 0;


void OnZeroCrossingInterrupt ()
{
  // STOP THE CLOCK
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));

  // SET THE COUNTER VALUE
  // If the value written to TCNT1 equals the OCR1x value, the compare match will be missed (and that is what we want!)
  // Do not write the TCNT1 equal to TOP in PWM modes with variable TOP values. The compare match for the TOP will be ignored and the counter will continue to 0xFFFF.
  uint8_t bcpSREG = SREG;
  cli ();
  TCNT1 = 21894+71*vm_dimmerLevel; // a value between 20000 and 40000 (TOP). Lower values result in lower dimming levels.
  SREG = bcpSREG;

  // ENABLE THE CLOCK
  TCCR1B |= vm_clockSource;
}

/*!
  \brief Constructor
 */
Switch::Dimmer::Configuration::Configuration ()
{
  m_dimmingDurationMs = 1;
}

/*!
  \brief Destructor
 */
Switch::Dimmer::Configuration::~Configuration ()
{
}

/*!
  \brief Default constructor
 */
Switch::Dimmer::Dimmer ()
{
}

/*!
  \brief Destructor
 */
Switch::Dimmer::~Dimmer ()
{
}

/*!
  \brief Begins the dimmer with the given configuration

  Resets all state variables and initializes the dimmer
 */
void Switch::Dimmer::Begin (const Switch::Dimmer::Configuration& i_configuration)
{
  // RESET THE TIMER
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;

  // ENABLE THE TIMER
  // The PRTIM1 bit in ”PRR – Power Reduction Register” on page 45 must be written to zero to enable Timer/Counter1 module.
  PRR &= ~_BV(PRTIM1);

  // SET COMPARE OUTPUT MODE FOR CHANNEL A
  TCCR1A |= 0x80; // equals TCCR1A |= _BV(COM1A1)

  // SET WAVEFORM GENERATION MODE 14
  TCCR1A |= _BV(WGM11);
  TCCR1B |= _BV(WGM13) | _BV(WGM12);

  // SELECT THE CLOCK SOURCE
  vm_clockSource = _BV(CS11); // => clkTimer = clkIO / 8

  // SET THE PERIOD AND PULSE LENGTH IN THE OUTPUT COMPARE REGISTER
  // If the OCR1x is set equal to BOTTOM (0x0000) the output will be a narrow spike for each TOP+1 timer clock cycle.
  uint8_t bcpSREG = SREG; // store the interrupt state
  cli ();                 // disable interrupts
  ICR1 = 40000;           // this is the TOP value in MODE 14 => 320000 clkIO cycles or 40000 clkTimer cycles => equals 20 ms or two dimmer cycles
  OCR1A = 10;              // or 1, maybe two. Defines the on-pulse length. When TCNT1 equals OCR1A the comparator signals a match. The waveform generator uses the match signal to generate an output according to the operating mode (WGM and COM)
  SREG = bcpSREG;         // restore interrupt state

  // ENABLE PORT B1 AS OUTPUT FOR PWM (digital port 9)
  // The setup of the OC1x should be performed before setting the Data Direction Register for the port pin to output.
  DDRB |= _BV(PORTB1);

  // attach the ISR to the interrupt
  attachInterrupt (i_configuration.m_interruptNrZeroCrossing, OnZeroCrossingInterrupt, FALLING);

  m_dimmingDurationMs = i_configuration.m_dimmingDurationMs;
  m_targetDimmerLevel = 0;
  vm_dimmerLevel      = 0;
  m_lastUpdateTime    = Switch::NowInMilliseconds ();

  SWITCH_DEBUG_MSG_0 ("dimmer started\n");
}

void Switch::Dimmer::SetLevel (uint8_t i_dimmerLevel)
{
  m_targetDimmerLevel = i_dimmerLevel;
  m_lastUpdateTime = Switch::NowInMilliseconds ();
}

/*!
  \brief Updates the dimmer.

  Gives processing time to the dimmer to adjust its state.
 */
void Switch::Dimmer::Update ()
{
  if (vm_dimmerLevel != m_targetDimmerLevel)
  {
    uint8_t deltaDimmerLevel = Switch::MillisecondsSince (m_lastUpdateTime)/m_dimmingDurationMs;

    if (m_targetDimmerLevel > vm_dimmerLevel)
    {
      vm_dimmerLevel += min (deltaDimmerLevel, m_targetDimmerLevel-vm_dimmerLevel);
    }
    else
    {
      vm_dimmerLevel -= min (deltaDimmerLevel, vm_dimmerLevel-m_targetDimmerLevel);
    }

    m_lastUpdateTime += deltaDimmerLevel*m_dimmingDurationMs;
  }
}

#endif // ARDUINO
