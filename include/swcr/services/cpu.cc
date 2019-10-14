#include <swcr/services/cpu.h>

#include <root/component.h>
#include <cpu_session/connection.h>

#include <swcr/services/cpu_thread.h>

using namespace SWCR;

Cpu_session_component::Cpu_session_component(Genode::Env &env, Pd_session_component &psc) : _env(env),
                                                                                            _real_cpu(_env),
                                                                                            _psc(psc),
                                                                                            _heap(_env.ram(), _env.rm())
{
    _env.ep().rpc_ep().manage(this);
}

Cpu_session_component::~Cpu_session_component()
{
    _env.ep().rpc_ep().dissolve(this);
}

Thread_capability Cpu_session_component::create_thread(Capability<Pd_session> pd,
                                                       Name const &name,
                                                       Affinity::Location affinity,
                                                       Weight weight,
                                                       addr_t utcb)
{
    Genode::log("CPU::\033[33m", __func__, "\033[0m");

    /* use custom cpu_thread_component */
    //return (new (_heap) Cpu_thread_component(*this, _psc.parent_pd_cap(), name, affinity, weight, utcb))->cap();

    /* use default cpu_thread_component */
    return _real_cpu.create_thread(_psc.parent_pd_cap(), name, affinity, weight, utcb);
};

Rpc_entrypoint &Cpu_session_component::thread_ep()
{
    Genode::log("CPU::\033[33m", __func__, "\033[0m");
    return _env.ep().rpc_ep();
}

void Cpu_session_component::kill_thread(Thread_capability thread)
{
    Genode::log("CPU::\033[33m", __func__, "\033[0m");
    _real_cpu.kill_thread(thread);
};

void Cpu_session_component::exception_sigh(Signal_context_capability sigh)
{
    Genode::log("CPU::\033[33m", __func__, "\033[0m");
    _real_cpu.exception_sigh(sigh);
};

Affinity::Space Cpu_session_component::affinity_space() const
{
    Genode::log("CPU::\033[33m", __func__, "\033[0m");
    return _real_cpu.affinity_space();
};

Dataspace_capability Cpu_session_component::trace_control()
{
    Genode::log("CPU::\033[33m", __func__, "\033[0m");
    return _real_cpu.trace_control();
};

int Cpu_session_component::ref_account(Cpu_session_capability cpu_session)
{
    Genode::log("CPU::\033[33m", __func__, "\033[0m");
    return _real_cpu.ref_account(cpu_session);
};

int Cpu_session_component::transfer_quota(Cpu_session_capability cpu_session, size_t amount)
{
    Genode::log("CPU::\033[33m", __func__, "\033[0m");
    return _real_cpu.transfer_quota(cpu_session, amount);
};

Cpu_session::Quota Cpu_session_component::quota()
{
    Genode::log("CPU::\033[33m", __func__, "\033[0m");
    return _real_cpu.quota();
};

Capability<Cpu_session::Native_cpu> Cpu_session_component::native_cpu()
{
    Genode::log("CPU::\033[33m", __func__, "\033[0m");
    return _real_cpu.native_cpu();
};

Cpu_session &Cpu_session_component::real_cpu_session()
{
    return _real_cpu;
}
