#ifndef __swcr_services_cpu_h__
#define __swcr_services_cpu_h__

#include <root/component.h>
#include <cpu_session/connection.h>

#include <swcr/services/pd.h>

#include <base/heap.h>

namespace SWCR
{
class Cpu_session_component;
class Cpu_session_factory;
} // namespace SWCR

class SWCR::Cpu_session_component : public Genode::Rpc_object<Genode::Cpu_session>
{
private:
    Genode::Env &_env;
    Genode::Cpu_connection _real_cpu;
    SWCR::Pd_session_component &_psc;

    Genode::Heap _heap;

public:
    Cpu_session_component(Genode::Env &env, Pd_session_component &psc);

    ~Cpu_session_component();

    Thread_capability create_thread(Capability<Pd_session> pd,
                                    Name const &name,
                                    Affinity::Location affinity,
                                    Weight weight,
                                    addr_t utcb = 0);

    void kill_thread(Thread_capability thread);

    void exception_sigh(Signal_context_capability sigh);

    Affinity::Space affinity_space() const;

    Dataspace_capability trace_control();

    int ref_account(Cpu_session_capability cpu_session);

    int transfer_quota(Cpu_session_capability cpu_session, size_t amount);

    Quota quota();

    Capability<Native_cpu> native_cpu();

    Cpu_session &real_cpu_session();

    Rpc_entrypoint &thread_ep();
    Entrypoint &signal_ep();
};

class SWCR::Cpu_session_factory : public Genode::Local_service<Cpu_session_component>::Factory
{
private:
    Genode::Env &_env;
    Genode::Allocator &_alloc;

    Pd_session_component &_psc;

public:
    Cpu_session_factory(Genode::Env &env, Genode::Allocator &alloc, Pd_session_component &psc) : _env(env), _alloc(alloc), _psc(psc) {}
    ~Cpu_session_factory() {}

    Cpu_session_component &create(Args const &, Genode::Affinity) override
    {
        return *new (_alloc) Cpu_session_component(_env, _psc);
    }

    void upgrade(Cpu_session_component &, Args const &) override {}

    void destroy(Cpu_session_component &session) override
    {
        Genode::destroy(_alloc, &session);
    }
};

#endif /*__swcr_services_cpu_h__*/