#include <base/component.h>

void Component::construct(Genode::Env &)
{
  Genode::log("Hello World from hello!");
  Genode::log("second msg!");
}
