#include <base/component.h>
#include <base/log.h>

#include <util/profiler.h>

Genode::size_t Component::stack_size() { return 32 * 1024; }

namespace Threads {
    struct Main;
    struct MyThread;
}

struct Threads::MyThread : Genode::Thread {
    Timer::Connection &_timer;
    Genode::String<64> _name;

    void entry() {
        {
            PROFILE_SCOPE(_name.string(), "yellow", _timer)
            for(int i = 0; i<500000000; i++) {
                __asm__("NOP");
                //Genode::log("i=", i);
            }
        }
    };

    MyThread(Genode::Env &env,
             Genode::String<64> name,
             Timer::Connection &timer,
             int pos) : Genode::Thread(env,
                                        Genode::Cpu_session::Name(name.string()),
                                        4 * 1024,
                                        Genode::Affinity::Location(pos, 0),
                                        Genode::Cpu_session::Weight(1),
                                        env.cpu()
                                       ),
                                         _timer(timer),
                                         _name(name)
    {
        start();
    };
    ~MyThread() { };
};

struct Threads::Main {
    Genode::Env &_env;

    Timer::Connection timer { _env };

    Main(Genode::Env &env) : _env(env) {
        Genode::log("Hello World from Threads::Main!");

        static Threads::MyThread t1(_env, Genode::String<64>("thread_1"), timer, 0);
        static Threads::MyThread t2(_env, Genode::String<64>("thread_2"), timer, 1);
        static Threads::MyThread t3(_env, Genode::String<64>("thread_3"), timer, 2);
        static Threads::MyThread t4(_env, Genode::String<64>("thread_4"), timer, 3);

        t1.join();
        t2.join();
        t3.join();
        t4.join();

        Genode::log("done123");
    }
};

void Component::construct(Genode::Env &env) {
  static Threads::Main main(env);
}