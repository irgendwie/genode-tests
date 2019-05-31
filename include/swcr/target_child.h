#include <base/child.h>
#include <swcr/services/pd.h>

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
    Genode::Cap_quota const _cap_quota { 50 };
    Genode::Ram_quota const _ram_quota { 1 * 1024 * 1024 };

    Parent_services &_parent_services;

    Genode::Local_service<Pd_session_component> &_swcr_pd;

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
            service = &_swcr_pd;
        else
            service = _find_service(_parent_services, service_name);
        return route(*service);
    }

    public:
    Target_child(Genode::Env &env, Parent_services &parent_services, Genode::Local_service<Pd_session_component> &swcr_pd) : _env(env), _parent_services(parent_services), _swcr_pd(swcr_pd), _child(_env.rm(), _env.ep().rpc_ep(), *this) { }
    ~Target_child() { };

    Name name() const override { return "hello"; };

    Genode::Pd_session &ref_pd() override { return _env.pd(); }
    Genode::Pd_session_capability ref_pd_cap() const override { return _env.pd_session_cap(); }

    void init(Genode::Pd_session &pd, Genode::Pd_session_capability pd_cap) override
    {
        pd.ref_account(ref_pd_cap());
        ref_pd().transfer_quota(pd_cap, _cap_quota);
        ref_pd().transfer_quota(pd_cap, _ram_quota);
    }
};