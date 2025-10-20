#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "Settings.hpp"
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

static constexpr float muffleConstant = 2000.f; // undefined set a random number, so will i!

void applyMuffle(Manager *manager = Manager::getSharedInstance()) {
	manager->system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &manager->lowPassFilterDSP);
	manager->lowPassFilterDSP->setParameterFloat(FMOD_DSP_LOWPASS_RESONANCE, 0.f);
	float cutoff = INT_FAST32_MAX;
	int64_t muffleStrength = Mod::get()->getSettingValue<int64_t>("muffleStrength");
	if (muffleStrength != 0) cutoff = muffleConstant / muffleStrength;
	manager->lowPassFilterDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, cutoff);
	manager->channel->addDSP(0, manager->lowPassFilterDSP);
}

void setVolume(int64_t volumeSetting = Mod::get()->getSettingValue<int64_t>("volume"), double reductionSetting = Mod::get()->getSettingValue<double>("reduction"), Manager *manager = Manager::getSharedInstance()) {
	manager->channel->setVolume(volumeSetting / 100.f * reductionSetting);
}

// muffle filter adapted from undefined with consent: https://discord.com/channels/911701438269386882/911702535373475870/1277758859204890738

$on_mod(Loaded) {
	(void) Mod::get()->registerCustomSettingType("configdir", &MyButtonSettingV3::parse);
	listenForSettingChanges<int64_t>("muffleStrength", [](int64_t muffleStrength) {
		float cutoff = INT_FAST32_MAX;
		if (muffleStrength != 0) cutoff = muffleConstant / muffleStrength;
		Manager::getSharedInstance()->lowPassFilterDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, cutoff);
	});
	listenForSettingChanges<int64_t>("volume", [](int64_t volume) {
		setVolume(volume, Mod::get()->getSettingValue<double>("reduction"));
	});
	listenForSettingChanges<double>("reduction", [](double reduction) {
		setVolume(Mod::get()->getSettingValue<int64_t>("volume"), reduction);
	});
}

class $modify(MyPlayLayer, PlayLayer) {
	void findAudioFilesIn(std::vector<std::string>& menuLoops, const std::filesystem::path& folder) {
		if (!std::filesystem::exists(folder)) return;
		for (const auto& file : std::filesystem::directory_iterator(folder)) {
			std::string tempPath = geode::utils::string::pathToString(file.path());
			if (Utils::isSupportedExtension(geode::utils::string::pathToString(file.path().extension()))) menuLoops.push_back(tempPath);
		}
	}

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
		Utils::stopMusicRemoveLowPass(); // in case someone's got enough hubris to use mishpro's comment in levels mod to exit a playlayer to enter someone else's level or whatever idk
		Mod* mod = Mod::get();
		Manager* manager = Manager::getSharedInstance();
		const std::filesystem::path& audioFile = mod->getSettingValue<std::filesystem::path>("file");
		auto singlePauseMenuLoop = geode::utils::string::pathToString(audioFile);
		if (mod->getSettingValue<bool>("random")) {
			// original vector logic by adam729's random death sounds
			std::vector<std::string> menuLoops;
			MyPlayLayer::findAudioFilesIn(menuLoops, mod->getConfigDir());
			MyPlayLayer::findAudioFilesIn(menuLoops, mod->getSettingValue<std::filesystem::path>("folder"));
			if (!menuLoops.empty()) manager->path = menuLoops[rand() % menuLoops.size()];
			else manager->path = singlePauseMenuLoop;
		} else manager->path = singlePauseMenuLoop;
		if (!std::filesystem::exists(manager->path)) {
			log::error("{} DOES NOT EXIST", manager->path);
			return true;
		}
		manager->system->createSound(manager->path.c_str(), FMOD_LOOP_NORMAL, nullptr, &manager->sound);
		return true;
	}

	void resume() {
		PlayLayer::resume();
		Utils::stopMusicRemoveLowPass();
	}

	void resumeAndRestart(bool p0) {
		PlayLayer::resumeAndRestart(p0);
		Utils::stopMusicRemoveLowPass();
	}

	void resetLevel() {
		PlayLayer::resetLevel();
		Utils::stopMusicRemoveLowPass();
	}

	void togglePracticeMode(bool practiceMode) {
		PlayLayer::togglePracticeMode();
		Utils::stopMusicRemoveLowPass();
	}

	void onQuit() {
		PlayLayer::onQuit();
		Utils::stopMusicRemoveLowPass();
	}
};

class $modify(MyPauseLayer, PauseLayer) {
	void onModSettings(cocos2d::CCObject* sender) {
		openSettingsPopup(Mod::get());
	}
	void customSetup() {
		PauseLayer::customSetup();
		const auto mod = Mod::get();
		auto manager = Manager::getSharedInstance();
		auto emptyBtn = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
        emptyBtn->setScale(.75f);
        auto btnIcon = CCSprite::create("btn.png"_spr);
        btnIcon->setPosition(emptyBtn->getContentSize() / 2);
        btnIcon->setScale(.625f);
        emptyBtn->addChild(btnIcon);
        emptyBtn->setID("settings"_spr);
        auto settingsBtn = CCMenuItemSpriteExtra::create(emptyBtn, this, menu_selector(MyPauseLayer::onModSettings));
		if (auto menu = getChildByID("left-button-menu")) {
			menu->addChild(settingsBtn);
			menu->updateLayout();
		}
		if (!mod->getSettingValue<bool>("enabled")) return;
		manager->sound->setLoopCount(-1);
		manager->system->playSound(manager->sound, nullptr, false, &(manager->channel));
		setVolume();
		if (mod->getSettingValue<bool>("muffle")) applyMuffle();
	}
	void removeMeAndCleanup() {
		PauseLayer::removeMeAndCleanup();
		Utils::stopMusicRemoveLowPass();
	}
	void onEdit(CCObject* sender) {
		Utils::stopMusicRemoveLowPass();
		PauseLayer::onEdit(sender);
	}
};
