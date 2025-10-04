#include "Utils.hpp"

namespace Utils {
	bool isSupportedExtension(std::string extension) {
		return !extension.empty() && (extension == ".mp3" || extension == ".ogg" || extension == ".oga" || extension == ".wav" || extension == ".flac");
	}
	void stopMusicRemoveLowPass(Manager* manager) {
		bool isMgrPlaying;
		manager->channel->isPlaying(&isMgrPlaying);
		if (!isMgrPlaying) return;
		manager->channel->removeDSP(manager->lowPassFilterDSP);
		manager->lowPassFilterDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 0.f);
		manager->channel->stop();
	}
}