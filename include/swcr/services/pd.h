#ifndef __swcr_pd_h__
#define __swcr_pd_h__

#include <root/component.h>
#include <pd_session/connection.h>

namespace SWCR
{
    class Pd_session_component;
    class Pd_session_factory;
}

using namespace Genode;

class SWCR::Pd_session_component : public Genode::Rpc_object<Genode::Pd_session>
{
    private:
    Genode::Env &_env;
    Genode::Pd_connection _real_pd;

    public:
    Pd_session_component(Genode::Env &env, const char *label) : _env(env), _real_pd(_env, label)
    {
        _env.ep().rpc_ep().manage(this);
    }
    ~Pd_session_component()
    {
        _env.ep().rpc_ep().dissolve(this);
    }

    Ram_dataspace_capability alloc(size_t size, Cache_attribute cached = CACHED)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.alloc(size, cached);
    };

    void free(Ram_dataspace_capability ds)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        _real_pd.free(ds);
    };

    size_t dataspace_size(Ram_dataspace_capability ds) const
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.dataspace_size(ds);
    };

    void assign_parent(Capability<Parent> parent)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        _real_pd.assign_parent(parent);
    };

    bool assign_pci(addr_t pci_config_memory_address, uint16_t bdf)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.assign_pci(pci_config_memory_address, bdf);
    };

    void map(addr_t virt, addr_t size)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        _real_pd.map(virt, size);
    };

    Signal_source_capability alloc_signal_source()
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.alloc_signal_source();
    };

    void free_signal_source(Signal_source_capability cap)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        _real_pd.free_signal_source(cap);
    };

    Capability<Signal_context> alloc_context(Signal_source_capability source, unsigned long imprint)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.alloc_context(source, imprint);
    };

    void free_context(Capability<Signal_context> cap)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        _real_pd.free_context(cap);
    };

    void submit(Capability<Signal_context> context, unsigned cnt = 1)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        _real_pd.submit(context, cnt);
    };

    Native_capability alloc_rpc_cap(Native_capability ep)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.alloc_rpc_cap(ep);
    };

    void free_rpc_cap(Native_capability cap)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        _real_pd.free_rpc_cap(cap);
    };

    Capability<Region_map> address_space()
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.address_space();
    };

    Capability<Region_map> stack_area()
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.stack_area();
    };

    Capability<Region_map> linker_area()
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.linker_area();
    };

    void ref_account(Capability<Pd_session> pd)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        _real_pd.ref_account(pd);
    };

    void transfer_quota(Capability<Pd_session> to, Cap_quota amount)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        _real_pd.transfer_quota(to, amount);
    };

    Cap_quota cap_quota() const
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.cap_quota();
    };

    Cap_quota used_caps() const
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.used_caps();
    };

    void transfer_quota(Capability<Pd_session> to, Ram_quota amount)
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        _real_pd.transfer_quota(to, amount);
    };

    Ram_quota ram_quota() const
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.ram_quota();
    };

    Ram_quota used_ram() const
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.used_ram();
    };

    Capability<Native_pd> native_pd()
    {
        Genode::log("Pd::\033[33m", __func__, "\033[0m");
        return _real_pd.native_pd();
    };
};

class SWCR::Pd_session_factory : public Genode::Local_service<Pd_session_component>::Factory
{
    private:
    Genode::Env &_env;
    Genode::Allocator &_alloc;
    const char *_label;

    public:
    Pd_session_factory(Genode::Env &env, Genode::Allocator &alloc, const char *label) : _env(env), _alloc(alloc), _label(label) { }
    ~Pd_session_factory() { }

    Pd_session_component &create(Args const &, Genode::Affinity) override
    {
        return *new (_alloc) Pd_session_component(_env, _label);
    }

    void upgrade(Pd_session_component &, Args const &) override { }

    void destroy(Pd_session_component &session) override
    {
        Genode::destroy(_alloc, &session);
    }
};

#endif /*__swcr_pd_h__*/