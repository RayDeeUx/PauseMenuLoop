#pragma once

#include "Manager.hpp"

using namespace geode::prelude;

namespace Utils {
    bool isSupportedExtension(std::string extension);
    
	bool isSceneRunning(std::string sceneName);
}