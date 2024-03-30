#pragma once

#include <regex>

// Manager.hpp structure by acaruso
// reused with explicit permission and strong encouragement

using namespace geode::prelude;

class Manager {

protected:
	static Manager* instance;
public:
	
	FMOD::Sound* sound;
	FMOD::Channel* channel;
	FMOD::System* system = FMODAudioEngine::sharedEngine()->m_system;
	
	std::string path = Mod::get()->getSettingValue<ghc::filesystem::path>("path").string();

	static Manager* getSharedInstance(){

		if (!instance) {
			instance = new Manager();
		};
		return instance;
	}

};