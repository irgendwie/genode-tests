#ifndef __swcr_services_region_map_component_h__
#define __swcr_services_region_map_component_h__

namespace SWCR
{
  class Region_map_component;
}

using namespace Genode;

class SWCR::Region_map_component : public Genode::Rpc_object<Genode::Region_map>,
  private List<SWCR::Region_map_component>::Element
{
  Local_addr attach(Dataspace_capability, size_t, off_t, bool, Local_addr, bool, bool) { }
  void detach(Local_addr) { }
  void fault_handler(Signal_context_capability) { }
  State state() { }
  Dataspace_capability dataspace() { }
};

#endif /*__swcr_services_region_map_component_h__*/
