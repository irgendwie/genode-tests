#ifndef __swcr_services_cpu_h__
#define __swcr_services_cpu_h__

#include <root/component.h>
#include <cpu_session/connection.h>

#include <swcr/services/pd.h>

namespace SWCR
{
    class Cpu_session_component;
    class Cpu_session_factory;
}

using namespace Genode;

class SWCR::Cpu_session_component : public Genode::Rpc_object<Genode::Cpu_session>
{
    private:
    Genode::Env &_env;
    Genode::Cpu_connection _real_cpu;
    SWCR::Pd_session_component &_psc;

    public:
    Cpu_session_component(Genode::Env &env, Pd_session_component &psc) : _env(env),
                                              _real_cpu(_env),
                                              _psc(psc)
    {
        _env.ep().rpc_ep().manage(this);
    }

    ~Cpu_session_component()
    {
        _env.ep().rpc_ep().dissolve(this);
    }

    Thread_capability create_thread(Capability<Pd_session> pd,
                                    Name const            &name,
                                    Affinity::Location     affinity,
                                    Weight                 weight,
                                    addr_t                 utcb = 0)
    {
        Genode::log("CPU::\033[33m", __func__, "\033[0m");
        return _real_cpu.create_thread(_psc.parent_pd_cap(), name, affinity, weight, utcb);
    };

    void kill_thread(Thread_capability thread)
    {
        Genode::log("CPU::\033[33m", __func__, "\033[0m");
        _real_cpu.kill_thread(thread);
    };

    void exception_sigh(Signal_context_capability sigh)
    {
        Genode::log("CPU::\033[33m", __func__, "\033[0m");
        _real_cpu.exception_sigh(sigh);
    };

    Affinity::Space affinity_space() const
    {
        Genode::log("CPU::\033[33m", __func__, "\033[0m");
        return _real_cpu.affinity_space();
    };

    Dataspace_capability trace_control()
    {
        Genode::log("CPU::\033[33m", __func__, "\033[0m");
        return _real_cpu.trace_control();
    };

    int ref_account(Cpu_session_capability cpu_session)
    {
        Genode::log("CPU::\033[33m", __func__, "\033[0m");
        return _real_cpu.ref_account(cpu_session);
    };

    int transfer_quota(Cpu_session_capability cpu_session, size_t amount)
    {
        Genode::log("CPU::\033[33m", __func__, "\033[0m");
        return _real_cpu.transfer_quota(cpu_session, amount);
    };

    Quota quota()
    {
        Genode::log("CPU::\033[33m", __func__, "\033[0m");
        return _real_cpu.quota();
    };

    Capability<Native_cpu> native_cpu()
    {
        Genode::log("CPU::\033[33m", __func__, "\033[0m");
        return _real_cpu.native_cpu();
    };
};

class SWCR::Cpu_session_factory : public Genode::Local_service<Cpu_session_component>::Factory
{
    private:
    Genode::Env &_env;
    Genode::Allocator &_alloc;

    Pd_session_component &_psc;

    public:
    Cpu_session_factory(Genode::Env &env, Genode::Allocator &alloc, Pd_session_component &psc) : _env(env), _alloc(alloc), _psc(psc) { }
    ~Cpu_session_factory() { }

    Cpu_session_component &create(Args const &, Genode::Affinity) override
    {
        return *new (_alloc) Cpu_session_component(_env, _psc);
    }

    void upgrade(Cpu_session_component &, Args const &) override { }

    void destroy(Cpu_session_component &session) override
    {
        Genode::destroy(_alloc, &session);
    }
};

#endif /*__swcr_services_cpu_h__*/