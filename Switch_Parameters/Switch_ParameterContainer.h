/*?*************************************************************************
*                           Switch_ParameterContainer.h
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

#ifndef _SWITCH_PARAMETERCONTAINER
#define _SWITCH_PARAMETERCONTAINER

// project includes

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Serialization/Switch_JsonSerializableInterface.h"

// third party includes
#include <string>
#include <list>

namespace Switch
{
  // forward declarations
  class Parameter;

  /*!
    \brief Container class for parameters and other parameter containers.
   */
  class ParameterContainer : public Switch::JsonSerializableInterface
  {
  public:

    typedef std::list <Switch::Parameter*>          ParameterList;  ///< Parameter list type.
    typedef std::list <Switch::ParameterContainer>  ContainerList;  ///< Parameter container list type.

    /*!
     \brief Default constructor.
     */
    ParameterContainer ();
    /*!
      \brief Constructor.

      \param [in] i_name        Name of the container.
      \param [in] i_description Description of the container.
     */
    ParameterContainer (const std::string& i_name, const std::string& i_description);
    /*!
      \brief Destructor.
     */
    virtual ~ParameterContainer ();
    /*!
      \brief Copy constructor.

      \param [in] i_other Object to copy.
     */
    ParameterContainer (const ParameterContainer& i_other);
    /*!
      \brief Assignment operator.

      \param [in] i_other Object to assign.

      \return Reference to this object.
     */
    ParameterContainer& operator= (const ParameterContainer& i_other);

    // accessors
    /*!
      \brief Gets the name of the parameter container.

      \return The name of the parameter container.
     */
    const std::string& GetName () const;
    /*!
      \brief Gets the description of the parameter container.

      \return The description of the parameter container.
     */
    const std::string& GetDescription () const;
    /*!
      \brief Gets a list of all parameters in this container.

      \return Const reference to a list of all parameters in this container.
     */
    const ParameterList& GetParameters () const;
    /*!
      \brief Gets a list of all parameters in this container.

      \return Reference to a list of all parameters in this container.
     */
    ParameterList& GetParameters ();
    /*!
      \brief Gets a list of all sub-containers in this container.

      \return Const reference to a list of all sub-containers in this container.
     */
    const ContainerList& GetSubContainers () const;
    /*!
      \brief Gets a list of all sub-containers in this container.

      \return Reference to a list of all sub-containers in this container.
     */
    ContainerList& GetSubContainers ();

    // modifiers
    /*!
      \brief Sets the name of the container.

      \param [in] i_name The name of the container.
     */
    void SetName (const std::string& i_name);
    /*!
      \brief Sets the description of the container.

      \param [in] i_description The description of the container.
     */
    void SetDescription (const std::string& i_description);

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

    /*!
      \brief Deallocates all resources.
     */
    void _Deallocate ();

    std::string m_name;         ///< Name of the container.
    std::string m_description;  ///< Description of the container.

    // data members
    ParameterList m_parameters;     ///< Parameters contained by this container.
    ContainerList m_subContainers;  ///< Containers nested in this container.
  };
}

#endif // _SWITCH_PARAMETERCONTAINER

