#include <base/component.h>

void Component::construct(Genode::Env &env)
{
  Genode::log("Hello World from mem_alloc!");

  try {
    env.pd().alloc(20 * 1024 * 1024);
  } catch(Genode::Out_of_ram) {
    Genode::log("Panic! No RAM left!");
  } catch(Genode::Out_of_caps) {
      Genode::error("Panic! No Caps left!");
  }

  Genode::log("We're back!");
}