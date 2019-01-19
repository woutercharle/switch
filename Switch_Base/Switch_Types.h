/*?*************************************************************************
*                           Switch_Types.h
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

#ifndef _SWITCH_TYPES
#define _SWITCH_TYPES

#include "Switch_CompilerConfiguration.h"

// third-party includes
#ifndef ARDUINO
#include <functional>
#endif

typedef uint16_t switch_network_address_type;
typedef uint32_t switch_device_address_type;
typedef uint64_t switch_pipe_address_type;
typedef uint32_t switch_brand_id_type;
typedef uint32_t switch_product_id_type;
typedef uint16_t switch_product_version_type;

namespace Switch
{
  struct DeviceInfo
  {
    switch_brand_id_type        brandId;              ///< The id of the brand of the device
    switch_product_id_type      productId;            ///< The id of the product the devide is
    switch_product_version_type productVersion;       ///< The version of the product
  };

  struct DeviceDetails
  {
    DeviceInfo info;
  };
}

#ifndef ARDUINO
typedef std::function <int (const int&, char const* const*, char const* const*)> SQLiteCallback;
#endif

#endif // _SWITCH_TYPES
