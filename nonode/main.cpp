#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"
#include "simulator.hpp"
#include "dispatcher.hpp"
#include "algorithms.hpp"

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int sig)
{
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main(int argc, char* argv[])
{
    signal(SIGSEGV, handler);

    try
    {
        std::string port = "5000";

        if (argc < 3)
        {
            Dispatcher::algorithm = Algorithms::get("sim");
            if (Dispatcher::algorithm == nullptr)
            {
                std::cerr << "Algorithm 'sim' not found." << std::endl;
                return 1;
            }
        }
        else if (argc == 3)
        {
            port = argv[1];
            Dispatcher::algorithm = Algorithms::get(argv[2]);
            if (Dispatcher::algorithm == nullptr)
            {
                std::cerr << "Algorithm '" << argv[2] << "' not found." << std::endl;
                return 1;
            }
        }
        else
        {
            std::cerr << "invalid args\n";
            return 1;
        }

        // Initialise the server.
        http::server::server s("0.0.0.0", port, ".");

        // Run the server until stopped.
        s.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    SimThread::stopAll();

    return 0;
}
