/*?*************************************************************************
*                           Switch_ParameterizedObject.h
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

#ifndef _SWITCH_PARAMETERIZEDOBJECT
#define _SWITCH_PARAMETERIZEDOBJECT

// project includes
#include "Switch_ParameterContainer.h"
#include "Switch_Parameter.h"

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Debug.h"
#include "../Switch_Base/Switch_StdLibExtras.h"

// third party includes
#include <mutex>
#include <map>
#include <list>


namespace Switch
{
  /*!
    \brief Definition of a parameterized object.

    A parameterized object foresees functionality to automatically generate and parse parameter containers from and into
    Parameters objects. The class provides thread-safe parameter access by implementing the public access and modification
    methods. These public methods guarantee thread-safety and forward the call to internal methods. Override the internal
    methods to alter their functionality. To avoid deadlocks, never call a public method on this object from an internal
    method.

    Pure virtual methods provided by this class:
    _GetParameters (Parameters&) const
    _SetParameters (const Parameters&)

    Other virtual methods that might be overridden:
    _GetParameters (Switch::ParameterContainer&) const
    _SetParameters (const Switch::ParameterContainer&)

    Note: Always call the virtual method of the super class when overriding it OR explicitly state that this is not wanted!!

    Methods to define the object's parameter container:
    => _AddParameter              Adds a parameter to this object's container.
    => _LinkParameterizedObject   Nests another object's parameter container into this object's parameter container.
    => _OverrideParameter         Hides a parameter from another object's parameter container. The other object's parameter
                                  container must be nested into this object's parameter container.

    For the interested reader:
      Key for this class is that it can construct a fake Parameters object. For this, the class needs to know (1) the address
      of the Parameters object's virtual function table (vftable), (2) the Parameters object's total byte size and
      (3) the position and size of each parameter in the Parameters object.
      The fake Parameters object is created as a blob of 0-initialized memory in which the vftable address is copied (byte 0)
      and which is then casted to the Parameters object base class defined by the Switch::ParameterizedObject.
      The vftable is needed to cast the object to its highest derived form and thus enables passing the fake Parameters
      object to the _SetParameters and _GetParameters methods.
   */
  class ParameterizedObject
  {
  public:

    /*!
      \brief Parameters container object.
     */
    class Parameters
    {
    public:
      /*!
        \brief Default constructor.
       */
      explicit Parameters ();
      /*!
        \brief Destructor.
       */
      virtual ~Parameters ();
    };

    /*!
      \brief Default constructor.
     */
    explicit ParameterizedObject ();
    /*!
      \brief Destructor.
     */
    virtual ~ParameterizedObject ();

    // accessors
    /*!
      \brief Gets the parameters of the parameterized object.

      \param [out] o_parameters Parameters of the parameterized object.
     */
    virtual void GetParameters (Parameters& o_parameters) const;
    /*!
      \brief Gets the parameters of the parameterized object.

      \param [out] o_parameters Parameters of the parameterized object.
     */
    virtual void GetParameters (Switch::ParameterContainer& o_parameters) const;

    // modifiers
    /*!
      \brief Sets the parameters of the parameterized object.

      \param [in] i_parameters Parameters to set in the parameterized object.
     */
    virtual void SetParameters (const Parameters& i_parameters);
    /*!
      \brief Sets the parameters of the parameterized object.

      \param [in] i_parameters Parameters to set in the parameterized object.
     */
    virtual void SetParameters (const Switch::ParameterContainer& i_parameters);

  protected:

	  // accessors
    /*!
      \brief Gets the parameters of the parameterized object.

      \param [out] o_parameters Parameters of the parameterized object.
     */
    virtual void _GetParameters (Parameters& o_parameters) const = 0;
    /*!
      \brief Gets the parameters of the parameterized object.

      \param [out] o_parameters Parameters of the parameterized object.
     */
    virtual void _GetParameters (Switch::ParameterContainer& o_parameters) const;

    // modifiers
    /*!
      \brief Sets the parameters of the parameterized object.

      \param [in] i_parameters Parameters to set in the parameterized object.
     */
    virtual void _SetParameters (const Parameters& i_parameters) = 0;
    /*!
      \brief Sets the parameters of the parameterized object.

      \param [in] i_parameters Parameters to set in the parameterized object.
     */
    virtual void _SetParameters (const Switch::ParameterContainer& i_parameters);

    // copy constructor and assignment operator are protected
    ParameterizedObject (const Switch::ParameterizedObject& i_other);
    ParameterizedObject& operator= (const Switch::ParameterizedObject& i_other);

	  // modifiers
	  // > general container properties
    void _SetContainerName (const std::string& i_name);
    void _SetContainerDescription (const std::string& i_description);

    // > container content
    template <class P>
    void _UpdateParametersObjectDefinition (const Parameters& i_parameters = P ());
    template <class T, class P>
    void _AddParameter (const P& i_parameters, const T& i_parameter, const std::string& i_name,
                        const std::string& i_description, const std::string& i_group);
    template <class T, class P>
    void _AddParameter (const P& i_parameters, const T& i_parameter, const std::string& i_name,
                        const std::string& i_description, const std::string& i_group,
                        const std::map <double, double>& i_validationCriteria);
    void _LinkParameterizedObject (Switch::ParameterizedObject& i_object, const std::string& i_name);

    // > container representation
    template <class PSlave, class T>
    void _OverrideParameter (Switch::ParameterizedObject& i_other, const PSlave& i_slaveParameters, const T& i_slaveParameter);
    template <class T, class P>
    void _HideParameter (const P& i_parameters, const T& i_parameter);
    void _HideParameter (const std::string& i_name, const std::string& i_group);
    template <class T, class P>
    void _UnhideParameter (const P& i_parameters, const T& i_parameter);
    void _UnhideParameter (const std::string& i_name, const std::string& i_group);
    void _UnlinkParamterizedObject (Switch::ParameterizedObject& i_object);
    void _UnlinkParamterizedObject (const std::string& i_listName);

    // threading
    mutable std::mutex m_parameterMutex;  ///< Avoids simultaneous parameter access

  private:

    /*!
      \brief Linker base class for parameters.

      Links a parameter in a Parameters object to a Switch::Parameter object.
     */
    class ParameterLinkBase
    {
    public:

      // constructor and destructor
      explicit ParameterLinkBase (const size_t& i_byteOffset, const size_t& i_byteSize);
      virtual ~ParameterLinkBase ();

      // member methods
      bool GetIsHidden () const;
      void SetHidden (const bool& m_hidden);
      virtual void InitializeData (unsigned char* io_pData) = 0;
      virtual void CleanData (unsigned char* io_pData) = 0;
      virtual void UpdateValue (const unsigned char* i_pData) = 0;
      virtual void ExtractValue (unsigned char* io_pData, const Switch::Parameter& i_parameter) const = 0;
      virtual const Switch::Parameter& ToParameter () const = 0;

      size_t GetByteOffset () const;

    protected:

      // link members
      bool    m_hidden;     ///< Marks if the parameters is hidden or not. Hidden parameters are not added to or extracted from the parameter list
      size_t  m_byteOffset; ///< Position of the parameter in the Parameters object
      size_t  m_byteSize;   ///< Size of the parameter in bytes

    private:

    };

    /*!
      \brief Templated linker class for parameters
      Links a parameter in a Parameters object to a specific type of VPP_Parameters object
     */
    template <template <typename> class ParameterType, typename Type>
    class ParameterLink : public ParameterLinkBase, public ParameterType <Type>
    {
    public:

      // constructor and destructor
      explicit ParameterLink (const size_t& i_byteOffset, const std::string& i_name, const std::string& i_description,
                              const std::string& i_group);
      virtual ~ParameterLink ();

      // member methods
      virtual void InitializeData (unsigned char* io_pData);
      virtual void CleanData (unsigned char* io_pData);
      virtual void UpdateValue (const unsigned char* i_pData);
      virtual void ExtractValue (unsigned char* io_pData, const Switch::Parameter& i_parameter) const;
      virtual const Switch::Parameter& ToParameter () const;

    protected:

    private:

    };

    /*!
      \brief Linker base class for parameterized objects
      Links to a parameterized object
     */
    class ParameterizedObjectLink
    {
    public:

      // constructor and destructor
      explicit ParameterizedObjectLink (Switch::ParameterizedObject& i_object, const std::string& i_containerName);
      virtual ~ParameterizedObjectLink ();

      // member methods
      virtual void GetParameters (Switch::ParameterContainer& o_parameters) const;
      virtual void SetParameters (const Switch::ParameterContainer& i_parameters) const;

      const Switch::ParameterizedObject& GetParameterizedObject () const;
      Switch::ParameterizedObject& GetParameterizedObject ();

    protected:

      // link members
      Switch::ParameterizedObject& m_parameterizedObject; ///< Reference to the parameterized object
      std::string                  m_containerName;       ///< Name of the container

    private:

      // copy constructor and assignment operator are never to be used
      ParameterizedObjectLink (const ParameterizedObjectLink& i_other);
      ParameterizedObjectLink& operator= (const ParameterizedObjectLink& i_other);

    };

    // utility methods
    template <class P, class T>
    size_t _GetParameterByteOffset (const P& i_parameters, const T& i_parameter) const;
    bool _ContainsParameterLink (const size_t& i_parameterByteOffset) const;
    bool _ContainsParameterContainerLink (const Switch::ParameterizedObject& i_other) const;

    // generalized (less-safe) methods to add / remove parameters
    template <class T>
    typename std::enable_if <std::is_arithmetic <T>::value, void>::type
    _AddParameter (const size_t& i_byteOffset, const std::string& i_name, const std::string& i_description, const std::string& i_group);
    template <class T>
    typename std::enable_if <std::is_arithmetic <T>::value, void>::type
    _AddParameter (const size_t& i_byteOffset, const std::string& i_name, const std::string& i_description, const std::string& i_group,
                   const std::map <double, double>& i_validationCriteria);
    template <class T>
    typename std::enable_if <std::is_same <T, std::string>::value, void>::type
    _AddParameter (const size_t& i_byteOffset, const std::string& i_name, const std::string& i_description, const std::string& i_group);
    void _HideParameter (const size_t& i_byteOffset);
    void _UnhideParameter (const size_t& i_byteOffset);

    // variables
    std::string m_containerName;        ///< Name of the container generated by GetParameters
    std::string m_containerDescription; ///< Description of the container generated by GetParameters

    std::list <ParameterLinkBase*>                                    m_parameterLinks;       ///< List of all parameter links
    std::map  <std::string, std::list <ParameterLinkBase*>::iterator> m_parameterMap;         ///< Maps keys to parameter links for each parameter added to the object
    std::list <ParameterizedObjectLink*>                                    m_containerLinks; ///< List of all parameter container links
    std::map  <std::string, std::list <ParameterizedObjectLink*>::iterator> m_containerMap;   ///< Maps keys to parameter container links for each parameterized object added to the object

    uintptr_t m_vftableAddress; ///< Address to the parameter object's virtual function table
    size_t    m_byteSize;       ///< Total size of the parameter object
  };

  // template specializations
  /*!
   *  \brief Initializes the native parameter in the parameters container object.
   *
   *  Specialization for std::string parameters.
   *
   *  \param [in, out] i_pData Pointer to the parameters container object.
   *
   *  \remark {Exception safety: no-throw guarantee if io_pData is allocated correctly, undefined behavior otherwise.}
   */
  template <>
  void ParameterizedObject::ParameterLink <Switch::TextParameter, std::string>::InitializeData (unsigned char* io_pData);

  /*!
   \ brief Resets the native parameter in the parameters container object.

   Specialization for std::string parameters.

   \param [in, out] i_pData Pointer to the parameters container object.

   \remark {Exception safety: no-throw guarantee if io_pData is allocated correctly, undefined behavior otherwise.}
   */
  template <>
  void Switch::ParameterizedObject::ParameterLink <Switch::TextParameter, std::string>::CleanData (unsigned char* io_pData);
}

/*!
  \brief Updates the definition of the parameters container object.

  The object definition (byte size and vftable address) is used to reconstruct a parameters object.

  \param[in] i_parameters Parameters object.

  \warning i_parameters must be of the same type as the template type.

  \remark {Exception safety: no-throw if i_parameters is of same type as the template type, undefined behavior otherwise.}
 */
template <class P>
void Switch::ParameterizedObject::_UpdateParametersObjectDefinition (const Parameters& i_parameters)
{
  // check that P is a type deriving from Parameters
  const P* pInParameters = dynamic_cast<const P*> (&i_parameters);
  SWITCH_ASSERT_RETURN_0 (0x0 != pInParameters);

  // if the size of the object is larger than the already known object, update our intelligence
  // note: equality for derived objects that do not add parameters
  if (sizeof (*pInParameters) >= m_byteSize)
  {
    // update the size of the parameters object
    m_byteSize = sizeof (*pInParameters);
    // get the address to the virtual function table
    m_vftableAddress = reinterpret_cast <const uintptr_t*> (pInParameters) [0];
  }
}

/*!
  \brief Gets the byte offset of the specified parameter in the given parameters object.

  \param[in] i_parameters Parameters container object.
  \param[in] i_parameter  Parameter in the container object of which to get the byte offset in the container object.

  \warning i_parameter must be allocated in i_parameters.

  \remark {Exception safety: no-throw if i_parameters is allocated in i_parameters, undefined behavior otherwise.}
 */
template <class P, class T>
size_t Switch::ParameterizedObject::_GetParameterByteOffset (const P& i_parameters, const T& i_parameter) const
{
  // make sure the parameter is located inside the parameters object
  SWITCH_ASSERT_RETURN_1 (reinterpret_cast <uintptr_t> (&i_parameters) <  reinterpret_cast <uintptr_t> (&i_parameter), 0);
  SWITCH_ASSERT_RETURN_1 (reinterpret_cast <uintptr_t> (&i_parameter)  < (reinterpret_cast <uintptr_t> (&i_parameters) + sizeof (i_parameters)), 0);

  // calculate the parameter's offset against the address of the parameters object
  return reinterpret_cast <uintptr_t> (&i_parameter) - reinterpret_cast <uintptr_t> (&i_parameters);
}

/*!
  \brief Constructor.

  \param [in] i_byteOffset Position of the parameter in the parameters container object (in bytes).
  \param [in] i_name Name of the parameter,
  \param [in] i_description Description of the parameter.
  \param [in] i_group Group the parameter belongs to.
  \param [in] i_parameterOptions Parameter editing options.

  \tparam T Native parameter type.
  \tparam Tp Switch parameter type.

  \remark {Exception safety: strong guarantee.}
 */
template <template <typename> class Tp, typename T>
Switch::ParameterizedObject::ParameterLink <Tp, T>::ParameterLink (const size_t& i_byteOffset,
                                                                   const std::string& i_name,
                                                                   const std::string& i_description,
                                                                   const std::string& i_group)
: Switch::ParameterizedObject::ParameterLinkBase (i_byteOffset, sizeof (T)),
  Tp <T> (i_name, i_description, i_group)
{
}

/*!
  \brief Destructor.

  \tparam T Native parameter type.
  \tparam Tp Switch parameter type.

  \remark {Exception safety: no-throw guarantee.}
 */
template <template <typename> class Tp, typename T>
Switch::ParameterizedObject::ParameterLink <Tp, T>::~ParameterLink ()
{
}

/*!
  \brief Initializes the native parameter in the parameters container object.

  \param [in, out] i_pData Pointer to the parameters container object.

  \tparam T Native parameter type.
  \tparam Tp Switch parameter type.

  \remark {Exception safety: no-throw guarantee.}
 */
template <template <typename> class Tp, typename T>
void Switch::ParameterizedObject::ParameterLink <Tp, T>::InitializeData (unsigned char* /*io_pData*/)
{
  // nothing to do for native types
}

/*!
  \brief Initializes the native parameter in the parameters container object.

  Specialization for std::string parameters.

  \param [in, out] i_pData Pointer to the parameters container object.

  \remark {Exception safety: no-throw guarantee if io_pData is allocated correctly, undefined behavior otherwise.}
 */
//template <>
//void Switch::ParameterizedObject::ParameterLink <Switch::TextParameter, std::string>::InitializeData (unsigned char* io_pData);

/*!
  \brief Resets the native parameter in the parameters container object.

  \param [in, out] i_pData Pointer to the parameters container object.

  \tparam T Native parameter type.
  \tparam Tp VPP parameter type.

  \remark {Exception safety: no-throw guarantee}
 */
template <template <typename> class Tp, typename T>
void Switch::ParameterizedObject::ParameterLink <Tp, T>::CleanData (unsigned char* /*io_pData*/)
{
  // nothing to do for native types
}

/*!
  \brief Resets the native parameter in the parameters container object.

  Specialization for std::string parameters.

  \param [in, out] i_pData Pointer to the parameters container object.

  \remark {Exception safety: no-throw guarantee if io_pData is allocated correctly, undefined behavior otherwise.}
 */
//template <>
//void Switch::ParameterizedObject::ParameterLink <Switch::TextParameter, std::string>::CleanData (unsigned char* io_pData);

/*!
  \brief Updates the value of a Switch::Parameter in the parameter link with the value in the parameters container object.

  \param [in, out] i_pData Pointer to the parameters container object.

  \tparam T Native parameter type.
  \tparam Tp Switch parameter type.

  \remark {Exception safety: the same guarantee as the Set method in Tp if (i_pData + m_byteOffset) points to data of type T,
                             undefined behavior otherwise.}
 */
template <template <typename> class Tp, typename T>
void Switch::ParameterizedObject::ParameterLink <Tp, T>::UpdateValue (const unsigned char* i_pData)
{
  // update the parameter value
  Tp <T>::SetValue (*reinterpret_cast_ptr <const T*> (i_pData + m_byteOffset));
}

/*!
\brief Extracts the value of a Switch::Parameter into a parameters container object.

  \param [in, out] io_pData Pointer to the parameters container object.
  \param [in]      i_parameter Parameter object from which to extract the parameter value.

  \tparam T Native parameter type.
  \tparam Tp VPP parameter type.

  \remark {Exception safety: the same guarantee as the Get method in Tp if (i_pData + m_byteOffset) points to data of type T,
                             undefined behavior otherwise.}
 */
template <template <typename> class Tp, typename T>
void Switch::ParameterizedObject::ParameterLink <Tp, T>::ExtractValue (unsigned char* io_pData, const Switch::Parameter& i_parameter) const
{
  // cast into the VPP parameter type
  const Tp <T>* pInParameter = dynamic_cast <const Tp <T>*> (&i_parameter);
  SWITCH_ASSERT_RETURN_0 (0x0 != pInParameter);
  if (0x0 == pInParameter)
  {
    // the parameter can not be set as there is a mismatch between the type specified
    // in the parameter list and the actual parameter type
    return;
  }

  // get the value from the VPP parameter and set it into the output data
  *reinterpret_cast_ptr <T*> (io_pData + m_byteOffset) = pInParameter->GetValue ();
}

/*!
  \brief Converts the parameter link to a Switch::Parameter object.

  \return Const reference to the Switch::Parameter object into which the parameter link is converted.

  \remark {Exception safety: no-throw}
 */
template <template <typename> class Tp, typename T>
const Switch::Parameter& Switch::ParameterizedObject::ParameterLink <Tp, T>::ToParameter () const
{
  return *this;
}

template <class T, class P>
void Switch::ParameterizedObject::_AddParameter (const P& i_parameters,
                                                 const T& i_parameter,
                                                 const std::string& i_name,
                                                 const std::string& i_description,
                                                 const std::string& i_group)
{
  // make sure we are aware about the largest version of the parameters object
  _UpdateParametersObjectDefinition <P> (i_parameters);

  // get the parameter's byte offset
  size_t byteOffset = _GetParameterByteOffset (i_parameters, i_parameter);

  // continue adding the parameter
  _AddParameter <T> (byteOffset, i_name, i_description, i_group);
}

template <class T, class P>
void Switch::ParameterizedObject::_AddParameter (const P& i_parameters,
                                                 const T& i_parameter,
                                                 const std::string& i_name,
                                                 const std::string& i_description,
                                                 const std::string& i_group,
                                                 const std::map <double, double>& i_validationCriteria)
{
  // make sure we are aware about the largest version of the parameters object
  _UpdateParametersObjectDefinition <P> (i_parameters);

  // get the parameter's byte offset
  size_t byteOffset = _GetParameterByteOffset (i_parameters, i_parameter);

  // continue adding the parameter
  _AddParameter <T> (byteOffset, i_name, i_description, i_group, i_validationCriteria);
}

/*!
  \brief Adds arithmetic parameters to the parameterized object's parameter container.

  \param [in] i_byteOffset  Position in the parameterized object's parameters object
  \param [in] i_name        Name of the parameter
  \param [in] i_description Description of the parameter
  \param [in] i_group       Group of the parameter
 */
template <class T>
typename std::enable_if <std::is_arithmetic <T>::value, void>::type
/*void*/ Switch::ParameterizedObject::_AddParameter (const size_t&      i_byteOffset,
                                                     const std::string& i_name,
                                                     const std::string& i_description,
                                                     const std::string& i_group)
{
  // first check if the parameter is already available
  SWITCH_ASSERT_RETURN_0 (!_ContainsParameterLink (i_byteOffset));
  if (_ContainsParameterLink (i_byteOffset))
  {
    return;
  }

  // create an enum parameter link
  ParameterLink <Switch::ScalarParameter, T>* pParameter = new ParameterLink <Switch::ScalarParameter, T> (i_byteOffset, i_name, i_description, i_group);

  // insert it at the end of the list of parameter list links
  std::list <ParameterLinkBase*>::iterator itParameter = m_parameterLinks.insert (m_parameterLinks.end (), pParameter);

  // add the iterator to the map
  std::string key = i_name + i_group;
  m_parameterMap [key] = itParameter;
}

/*!
  \brief Adds arithmetic parameters to the parameterized object's parameter container.

  \param [in] i_byteOffset          Position in the parameterized object's parameters object
  \param [in] i_name                Name of the parameter
  \param [in] i_description         Description of the parameter
  \param [in] i_group               Group of the parameter
  \param [in] i_validationCriteria  List of ranges in which the parameter is valid
 */
template <class T>
typename std::enable_if <std::is_arithmetic <T>::value, void>::type
/*void*/ Switch::ParameterizedObject::_AddParameter (const size_t&      i_byteOffset,
                                                     const std::string& i_name,
                                                     const std::string& i_description,
                                                     const std::string& i_group,
                                                     const std::map <double, double>& i_validationCriteria)
{
  // first check if the parameter is already available
  SWITCH_ASSERT_RETURN_0 (!_ContainsParameterLink (i_byteOffset));
  if (_ContainsParameterLink (i_byteOffset))
  {
    return;
  }

  // create an enum parameter link
  ParameterLink <Switch::ScalarParameter, T>* pParameter = new ParameterLink <Switch::ScalarParameter, T> (i_byteOffset, i_name, i_description, i_group);
  pParameter->GetValidationCriteria () = i_validationCriteria;

  // insert it at the end of the list of parameter list links
  std::list <ParameterLinkBase*>::iterator itParameter = m_parameterLinks.insert (m_parameterLinks.end (), pParameter);

  // add the iterator to the map
  std::string key = i_name + i_group;
  m_parameterMap [key] = itParameter;
}

template <class T>
typename std::enable_if <std::is_same <T, std::string>::value, void>::type
/*void*/ Switch::ParameterizedObject::_AddParameter (const size_t&       i_byteOffset,
                                                     const std::string&  i_name,
                                                     const std::string&  i_description,
                                                     const std::string&  i_group)
{
  // first check if the parameter is already available
  SWITCH_ASSERT_RETURN_0 (!_ContainsParameterLink (i_byteOffset));
  if (_ContainsParameterLink (i_byteOffset))
  {
    return;
  }

  // create an enum parameter link
  ParameterLink <Switch::TextParameter, T>* pParameter = new ParameterLink <Switch::TextParameter, T> (i_byteOffset, i_name, i_description, i_group);

  // insert it at the end of the list of parameter list links
  std::list <ParameterLinkBase*>::iterator itParameter = m_parameterLinks.insert (m_parameterLinks.end (), pParameter);

  // add the iterator to the map
  std::string key = i_name + i_group;
  m_parameterMap [key] = itParameter;
}

/*!
  \brief Hides a parameter in this object's parameter container.

  \param[in] i_parameters Reference to the object's parameters container object.
  \param[in] i_parameter Reference to the parameter in the object's parameters container object.
 */
template <class T, class P>
void Switch::ParameterizedObject::_HideParameter (const P& i_parameters, const T& i_parameter)
{
  // calculate the parameter byte offset
  size_t parameterByteOffset = _GetParameterByteOffset (i_parameters, i_parameter);

  _HideParameter (parameterByteOffset);
}

/*!
  \brief Un-hides a parameter in this object's parameter container.

  \param[in] i_parameters Reference to the object's parameters container object.
  \param[in] i_parameter Reference to the parameter in the object's parameters container object.
 */
template <class T, class P>
void Switch::ParameterizedObject::_UnhideParameter (const P& i_parameters, const T& i_parameter)
{
  // calculate the parameter byte offset
  size_t parameterByteOffset = _GetParameterByteOffset (i_parameters, i_parameter);

  _UnhideParameter (parameterByteOffset);
}

/*!
  \brief Sets a parameter in the other object as hidden.

  Removes the parameter from the other object's parameter container.

  \param [in] i_other Const reference to the object containing the slave parameter.
  \param [in] i_slaveParameters Parameters object containing the slave parameter.
  \param [in] i_slaveParameter The slave parameter.

  \note The parameter container of the other object must be nested in this object's parameter container.
 */
template <class PSlave, class T>
void Switch::ParameterizedObject::_OverrideParameter (Switch::ParameterizedObject& i_other, const PSlave& i_slaveParameters, const T& i_slaveParameter)
{
  // verify that the other's parameter list is nested in our parameter list
  SWITCH_ASSERT_RETURN_0 (_ContainsParameterContainerLink (i_other));
  if (!_ContainsParameterContainerLink (i_other))
  {
    return;
  }

  // get the position of the parameter in this list
  size_t parameterByteOffset = _GetParameterByteOffset (i_slaveParameters, i_slaveParameter);

  // remove it from the other's parameter list
  i_other._HideParameter (parameterByteOffset);
}

#endif // _SWITCH_PARAMETERIZEDOBJECT
