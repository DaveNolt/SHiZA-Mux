#include "WavFile/WavFile.h"


int main() {
    WavFile first("first.wav");
    WavFile second("second.wav");

    first.loadData();
    second.loadData();

    first.overVoice(second, 0.2, 1.3, 0.4, -30, 15);
    first.mixWith(second);
    first.save("result.wav");

    return 0;
}

