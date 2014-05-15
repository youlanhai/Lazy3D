
#include "stdafx.h"
#include "WrapperMusic.h"

void wrapperMusic()
{
    LOG_INFO(L"EXPORT - wrapperMusic");

#if 1
    //music////////////////////////////////////////////////////////////////////
    class_<cMusicPlayer>("cMusicPlayer", init<>())
        .def("loadMusic", &cMusicPlayer::loadMusic)
        .def("play", &cMusicPlayer::play)
        .def("pause", &cMusicPlayer::pause)
        .def("stop", &cMusicPlayer::stop)
        .def("isPlaying", &cMusicPlayer::isPlaying)
        .def("reset", &cMusicPlayer::reset)
        .def("setRepeat", &cMusicPlayer::setRepeat)
        .def("getRepeat", &cMusicPlayer::getRepeat)
        .def("setVolume", &cMusicPlayer::setVolume)
        .def("getVolume", &cMusicPlayer::getVolume)
        .def("setPan", &cMusicPlayer::setPan)
        .def("getPan", &cMusicPlayer::getPan)
        .def("setPitchShift", &cMusicPlayer::setPitchShift)
        .def("getPitchShift", &cMusicPlayer::getPitchShift)
        .def("isSeekable", &cMusicPlayer::isSeekable)
        .def("getLength", &cMusicPlayer::getLength)
        .def("setPosition", &cMusicPlayer::setPosition)
        .def("getPosition", &cMusicPlayer::getPosition)
        .def("setFade", &cMusicPlayer::setFade)
        .def("setFadeTime", &cMusicPlayer::setFadeTime)
        ;

    class_<cSound, SoundPtr>("cSound", no_init)
        .def("play", &cSound::play)
        .def("pause", &cSound::pause)
        .def("stop", &cSound::stop)
        .def("isPlaying", &cSound::isPlaying)
        .def("setVolum", &cSound::setVolum)
        ;
#endif

}
