#include "KSynth.h"
#include <iostream>
int main() {
	KSynth* synth = new KSynth();
	synth->initialize("F:/Music/Soundfonts/FluidR3 GM Modified Just Piano.sf2");

	std::cout << "Waiting to play thing..." << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	synth->setVolume(1.0f); //0.005

	//synth->sendNote(0, 60, 127, 8000000, 1000000); //1000000 / 16

	for (unsigned int i = 0; i < 1000; i++) { //127
		synth->sendNote(0, 60, 1, 1000000 * 9, 0, 1);
		//synth->sendNote(0, i, 127, 2000000, 2000000);
	}

	/*
	while (true) {
		synth->sendData(0x90, 0, 60, 127);

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		//synth->sendData(0x80, 0, 60, 127);

		synth->sendData(0x90, 0, 64, 127);

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		//synth->sendData(0x80, 0, 64, 127);

		synth->sendData(0x90, 0, 67, 127);

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		//synth->sendData(0x80, 0, 64, 127);

		synth->sendData(0x90, 0, 72, 127);

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		//synth->sendData(0x80, 0, 64, 127);
	}
	*/
	//synth->sendData(0x90, 0, 60, 127);
	//synth->sendData(0x90, 0, 64, 127);
	//synth->sendData(0x90, 0, 67, 127);
	//synth->sendData(0x90, 0, 40, 127);
	//synth->sendData(0x90, 0, 67, 127);

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}