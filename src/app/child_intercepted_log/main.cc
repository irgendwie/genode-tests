#include <base/component.h>
#include <base/log.h>

#include <util/string.h>

#include <base/child.h>
#include <init/child_policy.h>

#include <rom_session/connection.h>

#include <loader_session/connection.h>

/* custom services */
#include <root/component.h>

using namespace Genode;

namespace Custom {
  class Log_session_component;
  class Log_session_factory;
}

class MyChild : public Child_policy {
public:
  typedef Genode::Registered<Genode::Parent_service> Parent_service;
  typedef Genode::Registry<Parent_service> Parent_services;
private:
  Env &_env;
  Cap_quota const _cap_quota {50};
  Ram_quota const _ram_quota {1 * 1024 * 1024};

  Parent_services &_parent_services;
  Genode::Local_service<Custom::Log_session_component> &_ls;

  Child _child;

  template <typename T>
  static Genode::Service *_find_service(Genode::Registry<T> &services,
					Genode::Service::Name const &name)
  {
    Genode::Service *service = nullptr;
    services.for_each([&] (T &s) {
			if (!service && (s.name() == name))
			  service = &s; });
    return service;
  }

public:
  MyChild(Env &env, Parent_services &parent_services, Genode::Local_service<Custom::Log_session_component> &ls)
    :
    _env(env), _parent_services(parent_services), _ls(ls), _child(_env.rm(), _env.ep().rpc_ep(), *this)
  {}
  ~MyChild() { };

  Name name() const override { return "hello"; };

  Pd_session           &ref_pd()           override { return _env.pd(); }
  Pd_session_capability ref_pd_cap() const override { return _env.pd_session_cap(); }

  void init(Pd_session &pd, Pd_session_capability pd_cap) override
  {
    pd.ref_account(ref_pd_cap());
    ref_pd().transfer_quota(pd_cap, _cap_quota);
    ref_pd().transfer_quota(pd_cap, _ram_quota);
  }

  Genode::Child_policy::Route
  resolve_session_request(Genode::Service::Name const &service_name,
			  Genode::Session_label const &label) override
  {
    auto route = [&] (Genode::Service &service) {
		   return Genode::Child_policy::Route { .service = service,
							 .label   = label,
							 .diag    = Genode::Session::Diag() }; };

    Genode::Service *service = nullptr;
    service = _find_service(_parent_services, service_name);
    if (!strcmp("LOG", service_name.string())) {
      service = &_ls;
    }
    return route(*service);
  }
};

namespace Example {
  struct Main;
}

class Custom::Log_session_component : public Genode::Rpc_object<Genode::Log_session>
{
  Genode::Rpc_entrypoint &_ep;
  Genode::Env &_env;
  Genode::Log_connection _parent_log;

  public:

  Log_session_component(Genode::Rpc_entrypoint &ep, Genode::Env &env, const char *label)
  :
    _ep(ep),
    _env(env),
    _parent_log(env, label)
  {
    _ep.manage(this);
  }

  ~Log_session_component() {
    _ep.dissolve(this);
  }

  size_t write(String const &string_buf) {
    Genode::log("--> log session intercepter!");
    auto result = _parent_log.write(string_buf);
    return result;
  }
};

class Custom::Log_session_factory : public Genode::Local_service<Custom::Log_session_component>::Factory
{
  Genode::Allocator &_alloc;
  Genode::Rpc_entrypoint &_ep;
  Genode::Env &_env;

  public:

  Log_session_factory(Genode::Allocator &alloc, Genode::Rpc_entrypoint &ep, Genode::Env &env)
  :
    _alloc(alloc),
    _ep(ep),
    _env(env)
  { }

  Custom::Log_session_component &create(Args const &args, Genode::Affinity) override
  {
    return *new (_alloc) Custom::Log_session_component(_ep, _env, "hello");
  }

  void upgrade(Custom::Log_session_component &, Args const &) override { }

  void destroy(Custom::Log_session_component &session) override
  {
    Genode::destroy(_alloc, &session);
  }
};

struct Example::Main
{
  Genode::Env &_env;

  Genode::Heap _heap { _env.ram(), _env.rm() };

  MyChild::Parent_services _parent_services { };

  Main(Genode::Env &env) : _env(env)
  {
    const char *names[] = { "RM", "PD", "CPU", "IO_MEM", "IO_PORT", "IRQ", "ROM", "LOG", 0 };

    for (unsigned i = 0; names[i]; i++) {
      new (_heap) MyChild::Parent_service(_parent_services, env, names[i]);
    }

    /* custom service */
    // create new entrypoint
    Genode::Entrypoint *_ep = new (_heap) Genode::Entrypoint(_env, 16 * 1024, "custom ep");
    Custom::Log_session_factory _clf { _heap, _ep->rpc_ep(), _env };
    Genode::Local_service<Custom::Log_session_component> ls { _clf };
    /******************/

    new (_heap) MyChild(_env, _parent_services, ls);
  }
};

void Component::construct(Genode::Env &env)
{
  Genode::log("Hello World from child!");

  static Example::Main main(env);

  /* The component does not exit after the construct function returns. Instead, it be-
   * comes ready to respond to requests or signals originating from other components. The
   * example above does not interact with other components though.
   *
   * Genode Foundations p. 30
   */
}
