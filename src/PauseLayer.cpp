#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "Manager.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
	void onSettings(cocos2d::CCObject* sender) {
		openSettingsPopup(Mod::get());
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
        auto settingsBtn = CCMenuItemSpriteExtra::create(emptyBtn, this, menu_selector(MyPauseLayer::onSettings));
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
			else { manager->path = Mod::get()->getSettingValue<ghc::filesystem::path>("path").string(); }
		}
		manager->system->createSound((manager->path).c_str(), FMOD_LOOP_NORMAL, nullptr, &(manager->sound));
		manager->sound->setLoopCount(-1);
		manager->system->playSound((manager->sound), nullptr, false, &(manager->channel));
		manager->channel->setVolume((Mod::get()->getSettingValue<int64_t>("volume") / 100.f) * Mod::get()->getSettingValue<double>("reduction"));
	}
	void onResume(cocos2d::CCObject* sender) {
		Manager::getSharedInstance()->channel->stop();
		PauseLayer::onResume(sender);
	}
	void onQuit(cocos2d::CCObject* sender) {
		Manager::getSharedInstance()->channel->stop();
		PauseLayer::onQuit(sender);
	}
	void onRestart(cocos2d::CCObject* sender) {
		Manager::getSharedInstance()->channel->stop();
		PauseLayer::onRestart(sender);
	}
	void onRestartFull(cocos2d::CCObject* sender) {
		Manager::getSharedInstance()->channel->stop();
		PauseLayer::onRestartFull(sender);
	}
	void onEdit(cocos2d::CCObject* sender) {
		Manager::getSharedInstance()->channel->stop();
		PauseLayer::onEdit(sender);
	}
	void onNormalMode(cocos2d::CCObject* sender) {
		Manager::getSharedInstance()->channel->stop();
		PauseLayer::onNormalMode(sender);
	}
	void onPracticeMode(cocos2d::CCObject* sender) {
		Manager::getSharedInstance()->channel->stop();
		PauseLayer::onPracticeMode(sender);
	}
	void keyDown(cocos2d::enumKeyCodes p0) {
		if (p0 == enumKeyCodes::KEY_Space) { Manager::getSharedInstance()->channel->stop(); }
		PauseLayer::keyDown(p0);
	}
};