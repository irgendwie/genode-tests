#include <base/component.h>
#include <base/log.h>

Genode::size_t Component::stack_size() { return 32 * 1024; }

namespace Threads {
    struct Main;
    struct MyThread;
}

struct Threads::MyThread : Genode::Thread {
    void entry() {
        for(int i = 0; i<10000; i++) {
            Genode::log("i=", i);
        }
    };

    MyThread(Genode::Env &env, Genode::String<64> name) : Genode::Thread(env,
                                                Name(name),
                                                4 * 1024)
    {
        start();
    };
    ~MyThread() { };
};

struct Threads::Main {
    Genode::Env &_env;

    Main(Genode::Env &env) : _env(env) {
        Genode::log("Hello World from Threads::Main!");

        static Threads::MyThread t1(_env, Genode::String<64>("thread_1"));
        static Threads::MyThread t2(_env, Genode::String<64>("thread_2"));
    }
};

void Component::construct(Genode::Env &env) {
  static Threads::Main main(env);
}