#include "Utils.hpp"

namespace Utils {
	
	bool isSupportedExtension(std::string extension) {
		return (strcmp(".mp3", extension.c_str()) == 0 || strcmp(".wav", extension.c_str()) == 0 || strcmp(".ogg", extension.c_str()) == 0);
	}
	
	bool isSceneRunning(std::string sceneName) { return CCDirector::get()->getRunningScene()->getChildByID(sceneName.c_str()); }
	
}