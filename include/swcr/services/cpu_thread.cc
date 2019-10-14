#include <swcr/services/cpu_thread.h>

#include <base/thread.h>
#include <cpu_session/cpu_session.h>
#include <cpu_thread/client.h>

using namespace SWCR;

Cpu_thread_component::Cpu_thread_component(Cpu_session_component &cpu_session_component,
                                           Capability<Pd_session> pd,
                                           Cpu_session::Name const &name,
                                           Affinity::Location affinity,
                                           Cpu_session::Weight weight,
                                           addr_t utcb) : _cpu_session_component(cpu_session_component),
                                                          _real_cpu_thread(_cpu_session_component.real_cpu_session().create_thread(pd,
                                                                                                                                   name,
                                                                                                                                   affinity,
                                                                                                                                   weight,
                                                                                                                                   utcb))
{
    _cpu_session_component.thread_ep().manage(this);
}
Cpu_thread_component::~Cpu_thread_component() {}

void Cpu_thread_component::start(addr_t ip, addr_t sp)
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    _real_cpu_thread.start(ip, sp);
}

void Cpu_thread_component::pause()
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    _real_cpu_thread.pause();
}

void Cpu_thread_component::resume()
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    _real_cpu_thread.resume();
}

void Cpu_thread_component::cancel_blocking()
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    _real_cpu_thread.cancel_blocking();
}

void Cpu_thread_component::state(Thread_state const &state)
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    _real_cpu_thread.state(state);
}

Thread_state Cpu_thread_component::state()
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    return _real_cpu_thread.state();
}

void Cpu_thread_component::exception_sigh(Signal_context_capability handler)
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    _real_cpu_thread.exception_sigh(handler);
}

void Cpu_thread_component::single_step(bool enable)
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    _real_cpu_thread.single_step(enable);
}

Dataspace_capability Cpu_thread_component::utcb()
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    return _real_cpu_thread.utcb();
}

void Cpu_thread_component::affinity(Affinity::Location location)
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    _real_cpu_thread.affinity(location);
}

unsigned Cpu_thread_component::trace_control_index()
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    return _real_cpu_thread.trace_control_index();
}

Dataspace_capability Cpu_thread_component::trace_buffer()
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    return _real_cpu_thread.trace_buffer();
}

Dataspace_capability Cpu_thread_component::trace_policy()
{
    Genode::log("CPU_THREAD::\033[33m", __func__, "\033[0m");
    return _real_cpu_thread.trace_policy();
}