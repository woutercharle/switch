/*?*************************************************************************
*                           Switch_Dimmer.h
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

#ifndef _SWITCH_DIMMER
#define _SWITCH_DIMMER

#ifdef ARDUINO

// project includes
#include "Switch_NodeConfiguration.h"

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"

// forward declarations


namespace Switch
{
  class Dimmer
  {
  public:

	/*!
	  \brief Configuration parameters container class.

	  Contains all dimmer-specific parameters needed to configure a dimmer.
	 */
    class Configuration
    {
    public:

      /*!
        \brief Constructor
       */
      Configuration ();

      /*!
        \brief Destructor
       */
      ~Configuration ();

      /*!
        \brief Copy constructor

        \param[in] i_other Object to copy
       */
      Configuration (const Configuration& i_other) = default;

      /*!
        \brief Assignment operator

        \param[in] i_other Object to assign to this object
       */
      Configuration& operator= (const Configuration& i_other) = default;

      // members
      uint8_t m_interruptNrZeroCrossing;  ///< Number of the interrupt on which to detect voltage zero crossing. Note: not the pin number!
      uint8_t m_dimmingDurationMs;  ///< The duration in milliseconds to dimm 1 value. 0 is instantaneous.
    };

    /*!
      \brief Default constructor.
     */
    Dimmer ();

    /*!
      \brief Destructor.
     */
    ~Dimmer ();

    /*!
      \brief Initializes the node.

      \param[in] i_configuration The node's configuration
     */
    void Begin (const Configuration& i_configuration);

    /*!
      \brief Updates the dimmer.

      Gives processing time to the dimmer to adjust its state.
     */
    void Update ();

    void SetLevel (uint8_t i_dimmerLevel);

  private:

    // copy constructor and assignment operator are never to be used
    Dimmer (const Dimmer& i_other);
    Dimmer& operator= (const Dimmer& i_other);

    // helper methods

    // variables
    uint32_t m_lastUpdateTime;

    // members
    uint8_t m_targetDimmerLevel;
    uint8_t m_dimmingDurationMs;  ///< The duration in milliseconds to dimm 1 value. 0 is instantaneous.
  };
}

extern Switch::Dimmer Dimmer;

#endif // ARDUINO

#endif // _SWITCH_DIMMER
