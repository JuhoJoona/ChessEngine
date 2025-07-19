#ifndef UCI_HPP
#define UCI_HPP

#include <string>
#include "../engine/engine.hpp"

class UCI {
    public:
        UCI(Engine* engine);
        ~UCI();

        void run();

    private:
        void parse_command(const std::string& command);
        Engine* engine;
};

#endif // UCI_HPP