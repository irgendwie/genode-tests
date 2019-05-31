#include <base/component.h>
#include <swcr/target_child.h>
#include <swcr/services/pd.h>
//#include <swcr/services/cpu.h>

namespace SWCR
{
    struct Main;
}

struct SWCR::Main
{
    Genode::Env &_env;
    Genode::Heap _heap { _env.ram(), _env.rm() };

    Parent_services _parent_services { };

    Main(Genode::Env &env) : _env(env)
    {
        const char *names[] = { "RM", "PD", "CPU", "IO_MEM", "IO_PORT", "IRQ", "ROM", "LOG", 0 };

        for (unsigned i = 0; names[i]; i++)
        {
            new (_heap) Parent_service(_parent_services, env, names[i]);
        }

        /* custom services */
        SWCR::Pd_session_factory _psf { _env, _heap, "hello" };
        Genode::Local_service<SWCR::Pd_session_component> _pls { _psf };

        Genode::log("creating child...");
        new (_heap) Target_child(_env, _parent_services, _pls);
        Genode::log("done");
    }
};

void Component::construct(Genode::Env &env)
{
    static SWCR::Main main(env);
}