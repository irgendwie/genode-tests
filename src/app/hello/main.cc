#include <base/component.h>

void Component::construct(Genode::Env &env)
{
  Genode::log("Hello World from hello!");
}
