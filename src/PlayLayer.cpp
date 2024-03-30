#ifndef GEODE_IS_MACOS
#include <Geode/modify/PlayLayer.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    bool canPauseGame() {
        bool canPause = PlayLayer::canPauseGame();
        if (canPause) {
		    Manager::getSharedInstance()->channel->stop();
        }
        return canPause;
    }
};
#endif