#pragma once

#include "Manager.hpp"

using namespace geode::prelude;

namespace Utils {
    bool isSupportedExtension(std::string extension);
	void stopMusicRemoveLowPass(Manager* manager = Manager::getSharedInstance());
}