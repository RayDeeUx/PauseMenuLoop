#include <Geode/loader/SettingEvent.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
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

$execute {
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

class $modify(MyPauseLayer, PauseLayer) {
	void onModSettings(cocos2d::CCObject* sender) {
		openSettingsPopup(Mod::get());
	}
	void stopMusicRemoveLowPass(Manager *manager = Manager::getSharedInstance()) {
		manager->channel->removeDSP(manager->lowPassFilterDSP);
		manager->lowPassFilterDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 0.f);
		manager->channel->stop();
	}
	void customSetup() {
		PauseLayer::customSetup();
		if (!Mod::get()->getSettingValue<bool>("enabled")) return;
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
		if (Mod::get()->getSettingValue<bool>("random")) {
			// original vector logic by adam729's random death sounds
			std::vector<std::string> menuLoops;
            std::smatch match;
			for (const auto& file : std::filesystem::directory_iterator(Mod::get()->getConfigDir().string())) {
				std::string tempPath = file.path().string();
				std::string tempExtension = file.path().extension().string();
				if (Utils::isSupportedExtension(tempExtension)) {
					menuLoops.push_back(tempPath);
				}
			}
			if (!menuLoops.empty()) { manager->path = menuLoops[rand() % menuLoops.size()].c_str(); }
			else { manager->path = Mod::get()->getSettingValue<std::filesystem::path>("path").string(); }
		} else {
			manager->path = Mod::get()->getSettingValue<std::filesystem::path>("path").string();
		}
		manager->system->createSound((manager->path).c_str(), FMOD_LOOP_NORMAL, nullptr, &(manager->sound));
		manager->sound->setLoopCount(-1);
		manager->system->playSound((manager->sound), nullptr, false, &(manager->channel));
		setVolume();
		if (Mod::get()->getSettingValue<bool>("muffle")) applyMuffle();
	}
	void onResume(cocos2d::CCObject* sender) {
		stopMusicRemoveLowPass();
		PauseLayer::onResume(sender);
	}
	void onQuit(cocos2d::CCObject* sender) {
		stopMusicRemoveLowPass();
		PauseLayer::onQuit(sender);
	}
	void onRestart(cocos2d::CCObject* sender) {
		stopMusicRemoveLowPass();
		PauseLayer::onRestart(sender);
	}
	void onRestartFull(cocos2d::CCObject* sender) {
		stopMusicRemoveLowPass();
		PauseLayer::onRestartFull(sender);
	}
	void onEdit(cocos2d::CCObject* sender) {
		stopMusicRemoveLowPass();
		PauseLayer::onEdit(sender);
	}
	void onNormalMode(cocos2d::CCObject* sender) {
		stopMusicRemoveLowPass();
		PauseLayer::onNormalMode(sender);
	}

	void onPracticeMode(cocos2d::CCObject* sender) {
		stopMusicRemoveLowPass();
		PauseLayer::onPracticeMode(sender);
	}
	void keyDown(cocos2d::enumKeyCodes p0) {
		if (p0 == enumKeyCodes::KEY_Space) { Manager::getSharedInstance()->channel->stop(); }
		PauseLayer::keyDown(p0);
	}
};
