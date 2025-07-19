#include "engine/engine.hpp"
#include "uci/uci.hpp"

int main() {
    Engine engine;
    UCI uci(&engine);
    uci.run();
    
    return 0;
}