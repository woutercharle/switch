/*?*************************************************************************
*                           Switch_JSONRequest.h
*                           -----------------------
*    copyright            : (C) 2013 by Wouter Charle
*    email                : wouter.charle@gmail.com 
*
***************************************************************************/

#ifndef _SWITCH_JSONREQUEST
#define _SWITCH_JSONREQUEST

// Switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"

// third-party includes
#include <cppcms/view.h>  
#include <cppcms/form.h> 

class Switch_JSONRequest : public cppcms::base_content
{
public:

    class Form : public public cppcms::form
    {
      /*!
        \brief Constructor
       */
      explicit Form ()
      {
        // register widgets in the form
        add (method);  
        add (params);  
        add (id);
        
        // set-up field validation
        method.non_empty ();
        params.non_empty ();
        id.non_empty ();
      }

      /*!
        \brief Destructor
       */
      virtual ~Form ()
      {
      }
      
      // form members
      cppcms::widgets::text               method;
      cppcms::widgets::text               params;
      cppcms::widgets::numeric <uint32_t> id;
    };
    
    explicit Switch_JSONRequest ()
    {
    }
    
    virtual Switch_JSONRequest ()
    {
    }
    
    void Load (cppcms::http::context& i_context)
    {
      // note: can also get "method", "params", "id" from i_context.request ().post ()
      // or: construct request from context?
      m_form.load (i_context);
      if (m_form.validate ())
      {
        m_method      = m_form.method.value ();
        m_parameters  = m_form.params.value ();
        m_id          = m_form.id.value ();
      }
      m_form.clear ();
    }

    std::string m_method;
    std::string m_parameters;
    uint32_t    m_id;
    
protected:
    
    Form m_form;
    
};

#endif