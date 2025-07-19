#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <sstream>
#include "../util/util.hpp"
#include "../core/game/board/board.hpp"

class Engine {
    public:
        Engine();
        ~Engine();

        void initUci();
        void onIsReady();
        void onPosition(const util::PositionCmd& position);
        void onGo(const util::GoCmd& go);
        void onStop();
        void onSetOption(std::istringstream& ss);
        void onNewGame();

    private:
        bool is_ready = false;
        Board board;

};

#endif // ENGINE_HPP