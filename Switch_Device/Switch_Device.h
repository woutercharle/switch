/*?*************************************************************************
*                           Switch_Device.h
*                           -----------------------
*    copyright            : (C) 2014 by Wouter Charle
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

#ifndef _SWITCH_DEVICE
#define _SWITCH_DEVICE

// project includes
#include "Switch_DataContainer.h"

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"
#include "../Switch_Serialization/Switch_JsonSerializableInterface.h"

// third party includes
#include <string>

namespace Switch
{

  class Device
  {
  public:

    /*!
      \brief Container class for all device information.

      Contains all information about a specific type of devices.
     */
    class Description : public Switch::JsonSerializableInterface
    {
    public:

      /*!
        \brief Constructor
       */
      explicit Description ();

      /*!
        \brief Destructor
       */
      virtual ~Description ();

      /*!
       \brief Copy constructor.

       \param [in] i_other Object to copy.
       */
      Description (const Description& i_other);
      /*!
       \brief Assignment operator.

       \param [in] i_other Object to assign.

       \return Reference to this object.
       */
      Description& operator= (const Description& i_other);

      /*!
        \brief Equality comparator.

        \param [in] i_other Object to compare to.

        \return True if the compared object is equal, false otherwise.
       */
      bool operator== (const Description& i_other) const;
      /*!
       \brief Inequality comparator.

       \param [in] i_other Object to compare to.

       \return True if the compared object is inequal, false otherwise.
       */
      bool operator!= (const Description& i_other) const;

      /*!
        \brief Serializes the object to a JSON value.

        \param [out] o_root Root value of the object as JSON.
       */
      virtual void Serialize   (Json::Value& o_root) const;
      /*!
        \brief De-serializes the object from a JSON value.

        \param [in] i_root Root value of the object as JSON.
       */
      virtual void Deserialize (const Json::Value& i_root);

      // members
      switch_brand_id_type              m_deviceBrandId;        ///< The id of the brand of the device.
      std::string                       m_deviceBrandName;      ///< The brand of the device.
      switch_product_id_type            m_deviceProductId;      ///< The id of the product of the device.
      std::string                       m_deviceProductName;    ///< The name of the product.
      switch_product_version_type       m_deviceProductVersion; ///< The version of the product.
      Switch::DataContainer::DataFormat m_dataFormat;           ///< The format of data used by the device.
    };

    /*!
      \brief Constructor

      \param [in] i_dataFormat The data format of the data adapter
     */
    explicit Device (const switch_device_address_type& i_deviceAddress);
    /*!
      \brief Destructor
     */
    virtual ~Device ();
    /*!
      \brief Copy constructor.

      \param [in] i_other Object to copy.
     */
    Device (const Device& i_other);
    /*!
      \brief Assignment operator.

      \param [in] i_other Object to assign.

      \return Reference to this object.

      \note Only objects with same address can be assigned to each-other.
     */
    Device& operator= (const Device& i_other);

    /*!
      \brief Sets the device's description

      Sets general device properties and initializes the data container.

      \param [in] i_deviceDescription Description of the device.

      \throw std::rutime_error if the device description was already set.

      \see GetDescription
     */
    void SetDescription (const Description& i_deviceDescription);

    /*!
      \brief Gets the device's description

      Gets the general device properties.

      \return Description of the device.

      \note The device's description must be set.

      \throw std::runtime_error if the device description was not set.

      \see SetDescription
     */
    const Description& GetDescription () const;

    /*!
      \brief Gets the device's connection state.

      \return True if the device is connected, false otherwise.
     */
    bool GetConnectionState () const;

    /*!
      \brief Sets the device's connection state.

      \param [in] i_connected The device's connection state: True if the device is connected, false otherwise.
     */
    void SetConnectionState (const bool& i_connected);

    /*!
      \brief Gets a const reference to the device's data container.

      \return Const reference to the device's data container.

      \note The device's description must be set.

      \throw std::runtime_error if the device description was not set.

      \see SetDescription
     */
    const Switch::DataContainer& GetDataContainer () const;

    /*!
      \brief Gets a reference to the device's data container.

      \return Reference to the device's data container.

      \note The device's description must be set.

      \throw std::runtime_error if the device description was not set.

      \see SetDescription
     */
    Switch::DataContainer& GetDataContainer ();

  protected:

  private:

    const switch_device_address_type  m_address;        ///< The unique address of this device.
    bool                              m_connected;      ///< Flags if the node is connected (true) or not (false).
    Description const*                m_pDescription;   ///< Pointer to the device's description.
    Switch::DataContainer*            m_pDataContainer; ///< Pointer to the data container for this device.
  };
}

#endif // _SWITCH_DEVICE

