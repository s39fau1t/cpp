#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

int main() {
	SoLoud::Soloud soloud;
	SoLoud::Wav wave;
	
	soloud.init();
	
	wave.load("assets/audio/pickupCoin.wav");
	
	soloud.play(wave);
	
	while(soloud.getVoiceCount() > 0) {
		
	}
	
	soloud.deinit();
	
	return 0;
}
