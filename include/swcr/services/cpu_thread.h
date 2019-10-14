#ifndef __swcr_services_cpu_thread_h__
#define __swcr_services_cpu_thread_h__

/* Genode includes */
#include <base/thread.h>
#include <cpu_session/cpu_session.h>
#include <cpu_thread/client.h>

#include <swcr/services/cpu.h>

namespace SWCR
{
class Cpu_thread_component;
}

class SWCR::Cpu_thread_component : public Rpc_object<Cpu_thread>
{
private:
    Cpu_session_component &_cpu_session_component;
    Cpu_thread_client _real_cpu_thread;

public:
    Cpu_thread_component(Cpu_session_component &cpu_session_component,
                         Capability<Pd_session> pd,
                         Cpu_session::Name const &name,
                         Affinity::Location affinity,
                         Cpu_session::Weight weight,
                         addr_t utcb);
    ~Cpu_thread_component();

    void start(addr_t ip, addr_t sp) override;

    void pause() override;

    void resume() override;

    void cancel_blocking() override;

    void state(Thread_state const &state) override;

    Thread_state state() override;

    void exception_sigh(Signal_context_capability handler) override;

    void single_step(bool enable) override;

    Dataspace_capability utcb() override;

    void affinity(Affinity::Location location) override;

    unsigned trace_control_index() override;

    Dataspace_capability trace_buffer() override;

    Dataspace_capability trace_policy() override;
};

#endif /* __swcr_services_cpu_thread_h__ */