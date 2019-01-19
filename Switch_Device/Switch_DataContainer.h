/*?*************************************************************************
*                           Switch_DataContainer.h
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

#ifndef _SWITCH_DATACONTAINER
#define _SWITCH_DATACONTAINER

// project includes
#include "Switch_DataAdapter.h"

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Serialization/Switch_JsonSerializableInterface.h"

// third party includes
#include <string>
#include <list>

namespace Switch
{
  class DataContainer
  {
  public:

    class DataFormat : public Switch::JsonSerializableInterface
    {
    public:

      /*!
        \brief Default constructor.
       */
      explicit DataFormat ();
      /*!
        \brief Constructor.

        \param [in] i_name        Name of the data object.
        \param [in] i_description Description of the data object.
       */
      explicit DataFormat (const std::string& i_name, const std::string& i_description);
      /*!
        \brief Destructor.
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
        \brief Gets the total size of the data in the container in bits.

        \return The total size of the data in the container in bits.
       */
      uint32_t GetTotalBitSize () const;

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
      std::string m_name;         ///< The name of the container object.
      std::string m_description;  ///< A description of the content of the container object.

      std::list <Switch::DataAdapter::DataFormat> m_nestedAdapters; ///< List of data formats of data adapters contained by this container.
      std::list <DataFormat> m_nestedContainers;                    ///< List of data formats of nested data containers.
    };

    /*!
      \brief Container element descriptor class.

      Simple description of an element in the container.
     */
    class Element
    {
    public:

      uint32_t  m_address;      ///< The address of the element in bits. Unique for all elements in a nested container structure.
      uint32_t  m_magicNumber;  ///< Magic number for element adress verification.
      int32_t   m_value;        ///< The value of the element.
    };

    /*!
      \brief Constructor

      \param [in] i_dataFormat The data format of the data adapter
     */
    explicit DataContainer (const DataFormat& i_dataFormat);
    /*!
      \brief Destructor
     */
    virtual ~DataContainer ();
    /*!
      \brief Copy constructor.

      \param [in] i_other Object to copy.
     */
    DataContainer (const DataContainer& i_other);
    /*!
      \brief Assignment operator.

      \param [in] i_other Object to assign.

      \return Reference to this object.

      \note Only objects with same data format can be assigned to each-other.
     */
    DataContainer& operator= (const DataContainer& i_other);

    /*!
      \brief Gets the data format.

      \return Const reference to the data format.
     */
    const DataFormat& GetDataFormat () const;

    /*!
      \brief Sets the value of a selection of elements.

      \param [in,out] io_changedElements List of elements whose value has changed.
      \param [in] i_elements List of elements for which to set the value.

      \note The list must be sorted on element address.

      \return True if the value of one or more elements has changed.
     */
    bool SetElements (std::list <Element>& io_changedElements, const std::list <Element>& i_elements);
    /*!
      \brief Gets all elements.

      \param [in, out] i_elements List of all elements.
     */
    void GetElements (std::list <Element>& io_elements) const;

    /*!
      \brief Sets the content of the container.

      \param [in,out] io_changedElements List of elements whose value has changed.
      \param [in] i_pData Data buffer containing the content to set.

      \return True if the content has changed, false otherwise.
     */
    bool SetContent (std::list <Element>& io_changedElements, const uint8_t* const i_pData);
    /*!
      \brief Gets the content of the container.

      \param [in] o_pData Data buffer that must be filled with the content of the container.

      \note Can only be called on the data container that owns its own data buffer.
     */
    void GetContent (uint8_t* const o_pData) const;

  protected:

    /*!
      \brief Constructor

      \param [in] i_pData         Access to the underlying data buffer, shared with this data adapter.
      \param [in] i_startAddress  Address of the first element of the container.
      \param [in] i_dataFormat    The data format of the data adapter.
     */
    explicit DataContainer (uint8_t* i_pData, const uint32_t& i_startAddress, const DataFormat& i_dataFormat);

    /*!
      \brief Parses the data format and generates nested objects.

      \note The nested object lists must be empty.
     */
    void _GenerateNestedObjectsFromDataFormat ();

    const DataFormat& mr_dataFormat;  ///< Const reference to the dataformat of the container.
    bool              m_ownsData;     ///< Flags if this data container owns the data or not.
    uint8_t*          m_pData;        ///< Pointer to the underlying data array. The first byte contains the first nested adapter in the container.
    uint32_t          m_startAddress; ///< Address in bits of the first nested adapter in this container. Unique for all elements in a nested container structure.

    std::list <Switch::DataAdapter>   m_nestedDataAdapters;   ///< List of nested data adapters in this container.
    std::list <Switch::DataContainer> m_nestedDataContainers; ///< List of nested data containers in this container.
  };
}

#endif // _SWITCH_DATACONTAINER

