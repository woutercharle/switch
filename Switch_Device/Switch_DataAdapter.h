/*?*************************************************************************
*                           Switch_DataAdapter.h
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

#ifndef _SWITCH_DATAADAPTER
#define _SWITCH_DATAADAPTER

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Serialization/Switch_JsonSerializableInterface.h"

// third party includes
#include <string>
#include <list>

namespace Switch
{
  class DataAdapter
  {
  public:

    /*!
      \brief Data format description class.

      Contains all information to describe the data format of a data adapter.
     */
    class DataFormat : public Switch::JsonSerializableInterface
    {
    public:

      /*!
        \brief Default constructor.
       */
      explicit DataFormat ();
      /*!
        \brief Constructor with minimal set of arguments

        \param [in] i_name        Name of the data object
        \param [in] i_description Description of the data object
        \param [in] i_bitOffset   The index of the first bit. Ordered from right (small indices) to left (large indices). Values 0 to 7.
        \param [in] i_bitSize     The data length in bits, starting from the bit at m_bitOffset in m_pData.
       */
      explicit DataFormat (const std::string& i_name,  const std::string& i_description,
                           const uint8_t& i_bitOffset, const uint32_t& i_bitSize);
      /*!
        \brief Constructor with maximal set of arguments

        \param [in] i_name        Name of the data object
        \param [in] i_description Description of the data object
        \param [in] i_group       The name of the group this type of adapter belongs to.
        \param [in] i_bitOffset   The index of the first bit. Ordered from right (small indices) to left (large indices). Values 0 to 7.
        \param [in] i_bitSize     The data length in bits, starting from the bit at m_bitOffset in m_pData.
        \param [in] i_maxValue    The minimum data value.
        \param [in] i_minValue    The maximum data value.
       */
      explicit DataFormat (const std::string& i_name,  const std::string& i_description, const std::string& i_group,
                           const uint8_t& i_bitOffset, const uint32_t& i_bitSize,        const int32_t& i_minValue,
                           const int32_t& i_maxValue);
      /*!
        \brief Destructor
       */
      virtual ~DataFormat ();

      /*!
        \brief Copy constructor.

        \param [in] i_other Object to copy.
       */
      DataFormat (const DataFormat& i_other);
      /*!
        \brief Assignment operator.

        \param [in] i_other Object to assign.

        \return Reference to this object.
       */
      DataFormat& operator= (const DataFormat& i_other);

      /*!
       \brief Equality comparator.

       \param [in] i_other Object to compare to.

       \return True if the compared object is equal, false otherwise.
       */
      bool operator== (const DataFormat& i_other) const;
      /*!
       \brief Inequality comparator.

       \param [in] i_other Object to compare to.

       \return True if the compared object is inequal, false otherwise.
       */
      bool operator!= (const DataFormat& i_other) const;

      /*!
        \brief Validates a value according the data format's description.

        \param [in] i_value The value to validate against the data format.

        \return True if the value satisfies the data format's description, false otherwise.
       */
      bool ValidateValue (const int32_t& i_value) const;

      /*!
        \brief Computes the magic number for this object.

        The magic number can be used to verify e.g., the address of data adapters.

        \param [in] i_baseNumber Base number used to compute the magic number.

        \return Magic number.
       */
      uint32_t ComputeMagicNumber (const uint32_t& i_baseNumber) const;

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
      std::string m_name;         ///< The name of the data object.
      std::string m_description;  ///< A description of the data object.
      std::string m_group;        ///< The name of the group this type of adapter belongs to.
      uint8_t     m_bitOffset;    ///< The index of the first bit. Ordered from right (small indices) to left (large indices). Values 0 to 7.
      uint32_t    m_bitSize;      ///< The data length in bits, starting from the bit at m_bitOffset in m_pData.
      int32_t     m_minValue;     ///< The minimum data value.
      int32_t     m_maxValue;     ///< The maximum data value.
    };

    /*!
      \brief Constructor.

      \param [in] i_dataFormat The data format of the data adapter.
     */
    explicit DataAdapter (const DataFormat& i_dataFormat);
    /*!
      \brief Constructor

      \param [in] i_pData Access to the underlying data buffer, shared with this data adapter
      \param [in] i_dataFormat The data format of the data adapter
     */
    explicit DataAdapter (uint8_t* i_pData, const DataFormat& i_dataFormat);
    /*!
      \brief Destructor.
     */
    virtual ~DataAdapter ();
    /*!
      \brief Copy constructor.

      \param [in] i_other Object to copy.
     */
    DataAdapter (const DataAdapter& i_other);
    /*!
      \brief Assignment operator.

      \param [in] i_other Object to assign.

      \return Reference to this object.

      \note Only objects with same data format can be assigned to each-other.
     */
    DataAdapter& operator= (const DataAdapter& i_other);

    /*!
      \brief Gets the data format.

      \return Const reference to the data format.
     */
    const DataFormat& GetDataFormat () const;

    /*!
      \brief Sets the data value.

      Extracts the data value from the given data array and sets it in this object's data array.

      \return True if the value has changed, false otherwise.
     */
    bool SetDataValue (const uint8_t* const i_pData);
    /*!
      \brief Sets the data value.

      Sets the given value in this object's data array.

      \return True if the value has changed, false otherwise.
     */
    bool SetDataValue (const int32_t& i_value);
    /*!
      \brief Gets the data value.

      Gets the given value from this object's data array.
     */
    void GetDataValue (int32_t& o_value) const;

  protected:

    /*!
      \brief Creates a mask for the data on the binary array
     */
    void _CreateMask ();

    const DataFormat& mr_dataFormat;  ///< A const reference to this object's data format.
    uint8_t*          m_pData;        ///< Pointer to the data array. Ordered from left (small indices) to right (large indices).
    bool              m_ownsData;     ///< Flags whether this object owns the data array or not.
    uint8_t           m_mask [4];     ///< Binary mask on the data array for the memory of this data adapter.
  };
}

#endif // _SWITCH_DATAADAPTER

