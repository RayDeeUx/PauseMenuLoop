#include "Utils.hpp"

namespace Utils {
	
	bool isSupportedExtension(std::string extension) {
		return !extension.empty() && (extension == ".mp3" || extension == ".ogg" || extension == ".oga" || extension == ".wav" || extension == ".flac");
	}
	
	bool isSceneRunning(std::string sceneName) { return CCDirector::get()->getRunningScene()->getChildByID(sceneName.c_str()); }
	
}