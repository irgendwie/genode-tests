#ifndef __swcr_services_rm_h__
#define __swcr_services_rm_h__

#include <root/component.h>
#include <rm_session/connection.h>

namespace SWCR
{
  class Rm_session_component;
  class Rm_session_factory;
}

using namespace Genode;

class SWCR::Rm_session_component : public Genode::Rpc_object<Genode::Rm_session>
{
};

#endif /*__swcr_services_rm_h__*/
