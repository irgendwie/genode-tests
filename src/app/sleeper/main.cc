#include <base/component.h>
#include <base/log.h>

#include <timer_session/connection.h>

namespace Sleeper {
    struct Main;
    struct Test;
}

struct Sleeper::Test
{
    Genode::Env &env;
    Timer::Connection timer { env };
    Genode::Signal_transmitter done;

    Test(Genode::Env &env, Genode::Signal_context_capability done) : env(env), done(done)
    {
        Genode::log("Going to sleep...");
        timer.msleep(5000);
        Genode::log("Waking up...");
    }
};

struct Sleeper::Main
{
    Genode::Env &_env;

    Genode::Constructible<Sleeper::Test> test_1 { };
    Genode::Signal_handler<Main>       test_1_done { _env.ep(), *this, &Main::handle_test_1_done };

    void handle_test_1_done()
        {
                test_1.destruct();
        }

    Main(Genode::Env &env) : _env(env)
    {
        Genode::log("Hello World from sleeper!");

        test_1.construct(env, test_1_done);

        Genode::log("yeah?");
    }
};

void Component::construct(Genode::Env &env)
{
    static Sleeper::Main main(env);
}