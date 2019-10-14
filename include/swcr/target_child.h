#ifndef __swcr_target_child_h__
#define __swcr_target_child_h__

#include <base/child.h>
#include <swcr/services/pd.h>
#include <swcr/services/cpu.h>

namespace SWCR
{
    class Target_child;
    typedef Genode::Registered<Genode::Parent_service> Parent_service;
    typedef Genode::Registry<Parent_service> Parent_services;
}

class SWCR::Target_child : public Genode::Child_policy
{
    private:
    Genode::Env &_env;
    Genode::Heap _heap;
    Genode::Cap_quota const _cap_quota { 50 };
    Genode::Ram_quota const _ram_quota { 8 * 1024 * 1024 };

    Parent_services &_parent_services;

    SWCR::Pd_session_factory _psf;
    Genode::Local_service<Pd_session_component> _pls;

    SWCR::Cpu_session_factory *_csf;
    Genode::Local_service<Cpu_session_component> *_cls;

    Pd_session_component *pd_sess_comp;

    Genode::Child _child;

    template <typename T>
    static Genode::Service *_find_service(Genode::Registry<T> &services, Genode::Service::Name const &name)
    {
        Genode::Service *service = nullptr;
        services.for_each([&] (T &s) {
            if (!service && (s.name() == name))
                service = &s;
        });
        return service;
    }

    Genode::Child_policy::Route
    resolve_session_request(Genode::Service::Name const &service_name,
			                Genode::Session_label const &label) override
    {
        Genode::log(__func__, ": ", service_name, " ", label);
        auto route = [&] (Genode::Service &service) {
            return Genode::Child_policy::Route { .service = service,
							                     .label   = label,
					                    		 .diag    = Genode::Session::Diag() }; };

        Genode::Service *service = nullptr;
        if (!Genode::strcmp("PD", service_name.string()))
            service = &_pls;
        else if (!Genode::strcmp("CPU", service_name.string()))
            service = _cls;
        else
            service = _find_service(_parent_services, service_name);
        return route(*service);
    }

    public:
    Target_child(Genode::Env &env,
                 Parent_services &parent_services) : _env(env),
                                                     _heap(_env.ram(), _env.rm()),
                                                     _parent_services(parent_services),
                                                     _psf(_env, _heap, "mem_alloc"),
                                                     _pls(_psf),
                                                     _child(_env.rm(), _env.ep().rpc_ep(), *this)
    { }
    ~Target_child() { };

    Name name() const override { return "mem_alloc"; };

    Genode::Pd_session &ref_pd() override { return _env.pd(); }
    Genode::Pd_session_capability ref_pd_cap() const override { return _env.pd_session_cap(); }

    void init(Genode::Pd_session &custom_pd, Genode::Pd_session_capability pd_cap) override
    {
        custom_pd.ref_account(ref_pd_cap());
        Genode::log("transfering");
        Pd_session_component &pdsc = _psf.custom_pd_session_component();
        pd_sess_comp = &pdsc;
        _csf = new (_heap) SWCR::Cpu_session_factory(_env, _heap, pdsc);
        _cls = new (_heap) Genode::Local_service<SWCR::Cpu_session_component>(*_csf);
        ref_pd().transfer_quota(pdsc.parent_pd_cap(), _cap_quota);
        ref_pd().transfer_quota(pdsc.parent_pd_cap(), _ram_quota);
        Genode::log("done");
        /*ref_pd().transfer_quota(_cpsc.parent_pd_cap(), _cap_quota);
        ref_pd().transfer_quota(_cpsc.parent_pd_cap(), _ram_quota);*/
    }

    void resource_request(Genode::Parent::Resource_args const &args)
    {
        Genode::Ram_quota ram = Genode::ram_quota_from_args(args.string());
        Genode::Cap_quota caps = Genode::cap_quota_from_args(args.string());

        if (ram.value) {
            _env.pd().transfer_quota(pd_sess_comp->parent_pd_cap(), ram);
        }

        if (caps.value) {
            _env.pd().transfer_quota(pd_sess_comp->parent_pd_cap(), caps);
        }

        _child.notify_resource_avail();
    }
};

#endif /*__swcr_target_child_h__*/