/*?*************************************************************************
*                           Switch_Parameter.h
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

#ifndef _SWITCH_PARAMETER
#define _SWITCH_PARAMETER

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Serialization/Switch_JsonSerializableInterface.h"

// third party includes
#include <string>
#include <list>
#include <regex>
#include <json/value.h>


namespace Switch
{
  /*!
    \brief Base class for parameters
   */
  class Parameter : public Switch::JsonSerializableInterface
  {
  public:

    /*!
      \brief Enumeration of all supported parameter types.

      \note All types should have an enum in another header which can be obtained through a templated method.
     */
    enum eParameterType
    {
      PT_NOT_A_TYPE = -1,
      PT_STRING     =  0,
      PT_INT8       =  1,
      PT_UINT8      =  2,
      PT_INT16      =  3,
      PT_UINT16     =  4,
      PT_INT32      =  5,
      PT_UINT32     =  6,
      PT_INT64      =  7,
      PT_UINT64     =  8,
      PT_FLOAT32    =  9,
      PT_FLOAT64    = 10
    };

    /*!
      \brief Default constructor.
     */
    explicit Parameter ();
    /*!
      \brief Constructor.

      \param [in] i_name        Name of the parameter.
      \param [in] i_description Description of the parameter.
      \param [in] i_group       Parameter group or subset this parameter belongs to.
     */
    explicit Parameter (const std::string& i_name, const std::string& i_description, const std::string& i_group);
    /*!
      \brief Destructor.
     */
    virtual ~Parameter ();
    /*!
      \brief Clones this parameter.

      \return Pointer to a clone of this parameter.
     */
    virtual Parameter* Clone () const = 0;

    // accessors
    /*!
      \brief Gets the name of the parameter.

      \return The name of the parameter.
     */
    const std::string& GetName () const;
    /*!
      \brief Gets the group of the parameter.

      \return The group of the parameter.
     */
    const std::string& GetGroup () const;
    /*!
      \brief Gets the description of the parameter.

      \return The description of the parameter.
     */
    const std::string& GetDescription () const;
    /*!
      \brief Gets the type of the parameter.

      \return The type of the parameter.
     */
    const eParameterType& GetType () const;

    /*!
      \brief Serializes the object to a JSON value.

      \param [out] o_root Root value of the object as JSON.
     */
    virtual void Serialize (Json::Value& o_root) const { throw std::runtime_error ("cannot serialize non-specialized parameter"); };
    /*!
      \brief De-serializes the object from a JSON value.

      \param [in] i_root Root value of the object as JSON.
     */
    virtual void Deserialize (const Json::Value& i_root) { throw std::runtime_error ("cannot deserialize non-specialized parameter"); };

  protected:

    eParameterType  m_type;         ///< Type of the parameter.
    std::string     m_name;         ///< Name of the parameter.
    std::string     m_description;  ///< Description of the parameter.
    std::string     m_group;        ///< Parameter group or subset this parameter belongs to.

  };

  /*!
    \brief Base class for scalar based parameters
   */
  template <class scalar_type>
  class ScalarParameter : public Parameter
  {
  public:

    /*!
      \brief Default constructor.
     */
    explicit ScalarParameter ();
    /*!
      \brief Constructor.

      \param [in] i_name        Name of the parameter.
      \param [in] i_description Description of the parameter.
      \param [in] i_group       Parameter group or subset this parameter belongs to.
     */
    explicit ScalarParameter (const std::string& i_name, const std::string& i_description, const std::string& i_group);

    /*!
      \brief Destructor.
     */
    virtual ~ScalarParameter ();
    /*!
      \brief Clones this parameter.

      \return Pointer to a clone of this parameter.
     */
    virtual Parameter* Clone () const;

    // modifiers
    /*!
      \brief Validates and sets the value of the parameter.

      \param [in] i_value Value to set.

      \return True if the value was validated and set, false otherwise.
     */
    bool SetValue (const double& i_value);

    // accessors
    /*!
      \brief Gets the parameter value.

      \return The parameter value.
     */
    double GetValue () const;
    /*!
      \brief Gets the list of validation criteria.

      \return Reference to the list of validation criteria.
     */
    std::list <std::pair <double, double>>& GetValidationCriteria ();

    // utility
    /*!
      \brief Validates a value against the parameter's validation criteria.

      \return True if the value is valid.
     */
    bool Validate (const double& i_value) const;

    /*!
      \brief Serializes the object to a JSON value.

      \param [out] o_root Root value of the object as JSON.
     */
    virtual void Serialize (Json::Value& o_root) const;
    /*!
      \brief De-serializes the object from a JSON value.

      \param [in] i_root Root value of the object as JSON.
     */
    virtual void Deserialize (const Json::Value& i_root);

  protected:

    // data members
    std::list <std::pair <double, double>>  m_validationCriteria; ///< Criteratia to validate the parameter value.
    scalar_type                             m_value;              ///< Value of the parameter.
  };

  /*!
    \brief Default constructor.
   */
  template <class scalar_type>
  Switch::ScalarParameter <scalar_type>::ScalarParameter ()
  {
  }

  /*!
   * \brief Destructor
   */
  template <class scalar_type>
  Switch::ScalarParameter <scalar_type>::~ScalarParameter ()
  {
  }

  /*!
    \brief Clones this parameter.

    \return Pointer to a clone of this parameter.
   */
  template <class scalar_type>
  Switch::Parameter* Switch::ScalarParameter <scalar_type>::Clone () const
  {
    return new Switch::ScalarParameter <scalar_type> (*this);
  }

  /*!
   * \brief Validates and sets the value of the parameter.
   *
   * \param [in] i_value Value to set.
   *
   * \return True if the value was validated and set, false otherwise.
   */
  template <class scalar_type>
  bool Switch::ScalarParameter <scalar_type>::SetValue (const double& i_value)
  {
    bool valid = Validate (i_value);
    if (!valid)
    {
      return false;
    }

    m_value = static_cast <scalar_type> (i_value);
    return true;
  }

  /*!
   * \brief Gets the parameter value.
   *
   * \return Const reference to the parameter value.
   */
  template <class scalar_type>
  double Switch::ScalarParameter <scalar_type>::GetValue () const
  {
    return static_cast <double> (m_value);
  }

  /*!
   * \brief Gets the list of validation criteria.
   *
   * \return Reference to the list of validation criteria.
   */
  template <class scalar_type>
  std::list <std::pair <double, double>>& Switch::ScalarParameter <scalar_type>::GetValidationCriteria ()
  {
    return m_validationCriteria;
  }

  /*!
   * \brief Validates a value against the parameter's validation criteria.
   *
   * \return True if the value is valid
   */
  template <class scalar_type>
  bool Switch::ScalarParameter <scalar_type>::Validate (const double& i_value) const
  {
    // iterate over all criteria
    std::list <std::pair <double, double>>::const_iterator criteriaIterator;
    for (criteriaIterator = m_validationCriteria.begin (); m_validationCriteria.end () != criteriaIterator; ++criteriaIterator)
    {
      // evaluate the current criterium
      const std::pair <double, double>& validationCriterium = *criteriaIterator;
      if ((i_value < validationCriterium.first) || (i_value >= validationCriterium.second))
      {
        // the criterium is not met
        return false;
      }
    }

    // all the criteria are met
    return true;
  }

  /*!
    \brief Serializes the object to a JSON value.

    \param [out] o_root Root value of the object as JSON.
   */
  template <class scalar_type>
  void Switch::ScalarParameter <scalar_type>::Serialize (Json::Value& o_root) const
  {
    o_root ["type"]         = m_type;
    o_root ["name"]         = m_name;
    o_root ["description"]  = m_description;
    o_root ["group"]        = m_group;
    o_root ["value"]        = static_cast <double> (m_value);
  }

  /*!
    \brief De-serializes the object from a JSON value.

    \param [in] i_root Root value of the object as JSON.
   */
  template <class scalar_type>
  void Switch::ScalarParameter <scalar_type>::Deserialize (const Json::Value& i_root)
  {
    m_type        = static_cast <eParameterType> (i_root ["type"].asInt ());
    m_name        = i_root ["name"].asString ();
    m_description = i_root ["description"].asString ();
    m_group       = i_root ["group"].asString ();
    m_value       = static_cast <scalar_type> (i_root ["value"].asDouble ());
  }

  /*!
    \brief Base class for text based parameters.
   */
  template <class text_type>
  class TextParameter : public Parameter
  {
  public:

    /*!
      \brief Default constructor.
     */
    explicit TextParameter ();
    /*!
      \brief Constructor.

      \param [in] i_name        Name of the parameter.
      \param [in] i_description Description of the parameter.
      \param [in] i_group       Parameter group or subset this parameter belongs to.
     */
    explicit TextParameter (const std::string& i_name, const std::string& i_description, const std::string& i_group);
    /*!
      \brief Destructor.
     */
    virtual ~TextParameter ();
    /*!
      \brief Clones this parameter.

      \return Pointer to a clone of this parameter.
     */
    virtual Parameter* Clone () const;

    // modifiers
    /*!
      \brief Validates and sets the value of the parameter.

      \param [in] i_value Value to set.

      \return True if the value was validated and set, false otherwise.
     */
    bool SetValue (const std::string& i_value);

    // accessors
    /*!
      \brief Gets the parameter value.

      \return The parameter value.
     */
    std::string GetValue () const;
    /*!
      \brief Gets the list of validation criteria.

      \return Reference to the list of validation criteria.
     */
    std::list <std::regex>& GetValidationCriteria ();

    // utility
    /*!
      \brief Validates a value against the parameter's validation criteria.

      \return True if the value is valid.
     */
    bool Validate (const std::string& i_value) const;

    /*!
      \brief Serializes the object to a JSON value.

      \param [out] o_root Root value of the object as JSON.
     */
    virtual void Serialize (Json::Value& o_root) const;
    /*!
      \brief De-serializes the object from a JSON value.

      \param [in] i_root Root value of the object as JSON.
     */
    virtual void Deserialize (const Json::Value& i_root);

  protected:

    // data members
    std::list <std::regex>  m_validationCriteria; ///< Criteria to validate the parameter value.
    text_type               m_value;              ///< Value of the parameter.
  };

  /*!
    \brief Default constructor.
   */
  template <class text_type>
  Switch::TextParameter <text_type>::TextParameter ()
  {
  }

  /*!
   * \brief Destructor.
   */
  template <class text_type>
  Switch::TextParameter <text_type>::~TextParameter ()
  {
  }

  /*!
    \brief Clones this parameter.

    \return Pointer to a clone of this parameter.
   */
  template <class text_type>
  Switch::Parameter* Switch::TextParameter <text_type>::Clone () const
  {
    return new Switch::TextParameter <text_type> (*this);
  }

  /*!
   * \brief Validates and sets the value of the parameter.
   *
   * \param [in] i_value Value to set.
   *
   * \return True if the value was validated and set, false otherwise.
   */
  template <class text_type>
  bool Switch::TextParameter <text_type>::SetValue (const std::string& i_value)
  {
    if (!Validate (i_value))
    {
      return false;
    }

    m_value = static_cast <text_type> (i_value);
    return true;
  }

  /*!
   * \brief Gets the parameter value.
   *
   * \return The parameter value.
   */
  template <class text_type>
  std::string Switch::TextParameter <text_type>::GetValue () const
  {
    return m_value;
  }

  /*!
   * \brief Gets the list of validation criteria.
   *
   * \return Reference to the list of validation criteria.
   */
  template <class text_type>
  std::list <std::regex>& Switch::TextParameter <text_type>::GetValidationCriteria ()
  {
    return m_validationCriteria;
  }

  /*!
   * \brief Validates a value against the parameter's validation criteria.
   *
   * \return True if the value is valid
   */
  template <class text_type>
  bool Switch::TextParameter <text_type>::Validate (const std::string& i_value) const
  {
    // iterate over all criteria
    std::list <std::regex>::const_iterator criteriaIterator;
    for (criteriaIterator = m_validationCriteria.begin (); m_validationCriteria.end () != criteriaIterator; ++criteriaIterator)
    {
      // evaluate the current criterium
      const std::regex& criterium = *criteriaIterator;
      if (!std::regex_match (m_value, criterium))
      {
        // the criterium is not met
        return false;
      }
    }

    // all the criteria are met
    return true;
  }

  /*!
    \brief Serializes the object to a JSON value.

    \param [out] o_root Root value of the object as JSON.
   */
  template <class text_type>
  void Switch::TextParameter <text_type>::Serialize (Json::Value& o_root) const
  {
    o_root ["type"]         = m_type;
    o_root ["name"]         = m_name;
    o_root ["description"]  = m_description;
    o_root ["group"]        = m_group;
    o_root ["value"]        = m_value;
  }

  /*!
    \brief De-serializes the object from a JSON value.

    \param [in] i_root Root value of the object as JSON.
   */
  template <class text_type>
  void Switch::TextParameter <text_type>::Deserialize (const Json::Value& i_root)
  {
    m_type        = static_cast <eParameterType> (i_root ["type"].asInt ());
    m_name        = i_root ["name"].asString ();
    m_description = i_root ["description"].asString ();
    m_group       = i_root ["group"].asString ();
    m_value       = i_root ["value"].asString ();
  }
}

#endif // _SWITCH_PARAMETER

