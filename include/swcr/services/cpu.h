#include <root/component.h>
#include <cpu_session/connection.h>

namespace SWCR
{
    class Cpu_session_component;
    class Cpu_session_factory;
}

using namespace Genode;

class SWCR::Cpu_session_component : public Genode::Rpc_object<Genode::Cpu_session>
{
    public:
    Cpu_session_component() { }
    ~Cpu_session_component() { }

    Thread_capability create_thread(Capability<Pd_session> pd,
                                    Name const            &name,
                                    Affinity::Location     affinity,
                                    Weight                 weight,
                                    addr_t                 utcb = 0) { };
    void kill_thread(Thread_capability thread) { };
    void exception_sigh(Signal_context_capability) { };
    Affinity::Space affinity_space() const { };
    Dataspace_capability trace_control() { };
    int ref_account(Cpu_session_capability cpu_session) { };
    int transfer_quota(Cpu_session_capability cpu_session, size_t amount) { };
    Quota quota() { };
    Capability<Native_cpu> native_cpu() { };
};

class SWCR::Cpu_session_factory : public Genode::Local_service<Cpu_session_component>::Factory
{
    private:
    Genode::Allocator &_alloc;

    public:
    Cpu_session_factory(Genode::Allocator &alloc) : _alloc(alloc) { }
    ~Cpu_session_factory() { }

    Cpu_session_component &create(Args const &, Genode::Affinity) override
    {
        return *new (_alloc) Cpu_session_component();
    }

    void upgrade(Cpu_session_component &, Args const &) override { }

    void destroy(Cpu_session_component &session) override
    {
        Genode::destroy(_alloc, &session);
    }
};