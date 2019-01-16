/*
 * \brief  Test client for the Hello RPC interface
 * \author Björn Döbel
 * \author Norman Feske
 * \date   2008-03-20
 */

/*
 * Copyright (C) 2008-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <base/component.h>
#include <base/log.h>
#include <libc/component.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <util/string.h>

#include <base/child.h>
#include <init/child_policy.h>

namespace xyz {
  class Parent_service : public Genode::Parent_service {
  private:
    Genode::Registry<Parent_service>::Element _reg_elem;

  public:
    Parent_service(Genode::Registry<Parent_service> &registry, Genode::Env &env,
                   Service::Name const &name)
                   :
                   Genode::Parent_service(env, name), _reg_elem(registry, *this) { }
  };

  class MyChild : public Genode::Child_policy {
    Name const _name;

    Genode::Env &_env;

		Genode::Cap_quota const _cap_quota;
    Genode::Ram_quota const _ram_quota;

    Genode::Registry<xyz::Parent_service> &_parent_services;

    Genode::Rpc_entrypoint _child_ep;

    Init::Child_policy_provide_rom_file _binary_policy;

    Genode::Child _child;

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

    MyChild(Genode::Env &env,
      Genode::Session_label const &label,
      Genode::Cap_quota cap_quota,
      Genode::Ram_quota ram_quota,
      Genode::Registry<xyz::Parent_service> parent_services,
      Genode::Dataspace_capability binary) :
          _name(label),
          _env(env),
          _cap_quota(Genode::Child::effective_quota(cap_quota)),
          _ram_quota(Genode::Child::effective_quota(ram_quota)),
          _parent_services(parent_services),
          _child_ep{&_env.pd(), 12 * 1024, name().string()},
          _binary_policy(name(), binary, &_child_ep),
          _child(_env.rm(), _env.ep().rpc_ep(), *this)
    { };
    ~MyChild() { };

    Name name() const override { return _name; }

    Genode::Pd_session           &ref_pd()           override { return _env.pd(); }
    Genode::Pd_session_capability ref_pd_cap() const override { return _env.pd_session_cap(); }

    void init(Genode::Pd_session &session,
		          Genode::Pd_session_capability cap) override
		{
			session.ref_account(_env.pd_session_cap());
			_env.pd().transfer_quota(cap, _cap_quota);
			_env.pd().transfer_quota(cap, _ram_quota);
    }

    Genode::Service &resolve_session_request(Genode::Service::Name const &service_name,
		                                         Genode::Session_state::Args const &args) override
    {
      Genode::Service *service = nullptr;
      if ((service = _binary_policy.resolve_session_request(service_name.string(), args.string())))
  		{
        Genode::log("Service from policy: ", service_name, "/", args);
  		  return *service;
  		}

      Genode::log("Service from parent: ", service_name, "/", args);

      return *_find_service(_parent_services, service_name);
    }
  };
}

void Libc::Component::construct(Libc::Env &env)
{
  Libc::with_libc([&] () {
		    struct sockaddr_in srv_addr;
		    srv_addr.sin_port = htons(8080);
		    srv_addr.sin_family = AF_INET;
		    srv_addr.sin_addr.s_addr = inet_addr("131.159.12.17");

		    int sd = ::socket(AF_INET, SOCK_STREAM, 0);
		    ::connect(sd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));

        Genode::Ram_dataspace_capability ds;
        ds = env.pd().alloc(1 * 1024 * 1024);
        Genode::addr_t addr = env.rm().attach(ds);

        ::recv(sd, (void *)addr, 16383, 0);
		    Genode::String<16383> s((const char *)addr);
		    Genode::log(s);

        Genode::Heap _heap { env.ram(), env.rm() };

        Genode::Registry<xyz::Parent_service> parent_services;
        static const char *names[] = { "PD", "CPU", "LOG", "ROM", 0 };
        for (unsigned i = 0; names[i]; i++)
          new (_heap) xyz::Parent_service(parent_services, env, names[i]);
        xyz::MyChild child { env, Genode::Session_label("app"), Genode::Cap_quota{6}, Genode::Ram_quota{1 * 1024 * 1024}, parent_services, ds };

			while(1) {
			}
  });
}
