#include "uci.hpp"
#include "../engine/engine.hpp"
#include <iostream>
#include "../util/logger.hpp"
#include <sstream>

UCI::UCI(Engine* engine) : engine(engine) {}

UCI::~UCI() {
    // The UCI class does not own the engine instance, so it must not delete it.
}

void UCI::run() {
    std::string line;
    while (std::getline(std::cin, line)) {
        LOG("Received command: " << line << std::endl);
        
        std::istringstream ss(line);
        std::string token;
        
        if (!(ss >> token)) {
            LOG("Empty line received" << std::endl);
            continue;
        }
        
        LOG("Processing token: " << token << std::endl);
        
        if (token == "uci") {
            LOG("Handling uci command" << std::endl);
            engine->initUci();
        } else if (token == "isready") {
            LOG("Handling isready command" << std::endl);
            engine->onIsReady();
        } else if (token == "position") {
            LOG("Handling position command" << std::endl);
            util::PositionCmd pos(ss);
            engine->onPosition(pos);
        } else if (token == "go") {
            LOG("Handling go command" << std::endl);
            util::GoCmd go(ss);
            engine->onGo(go);
        } else if (token == "stop") {
            LOG("Handling stop command" << std::endl);
            engine->onStop();
        } else if (token == "setoption") {
            LOG("Handling setoption command" << std::endl);
            engine->onSetOption(ss);
        } else if (token == "ucinewgame") {
            LOG("Handling ucinewgame command" << std::endl);
            engine->onNewGame();
        } else if (token == "quit") {
            LOG("Handling quit command" << std::endl);
            break;
        } else {
            LOG("Unknown command: " << token << std::endl);
        }
        
        LOG("Finished processing command" << std::endl);
    }
}

