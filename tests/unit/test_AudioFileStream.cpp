#include <doctest/doctest.h>

#include "audio/AudioFileStream.h"
#include "audio/StreamManager.h"
#include "audio/Track.h"
#include "audio/Transport.h"

TEST_CASE("AudioFileStream single thread functionality") {
    juce::AudioFormatManager format_manager;
    format_manager.registerBasicFormats();

    Transport transport;
    std::unordered_map<TrackId, std::unique_ptr<Track>> tracks;

    juce::File file("~/Desktop/Test/Test/Projects/LightDAW/tests/unit/data/adventure.wav");
    REQUIRE(file.existsAsFile());

    juce::AudioFormatReader* reader = format_manager.createReaderFor(file);
    REQUIRE(reader != nullptr);

    tracks.emplace(0, std::make_unique<Track>(transport, 0));
    StreamManager stream_manager(transport, format_manager, tracks);
    Clip clip(0, file, 0, 0, reader->lengthInSamples);

    AudioFileStream stream(2, 4096, format_manager, clip, stream_manager);

    juce::AudioBuffer<float> buffer(2, 4096);
    CHECK(stream.read(buffer.getArrayOfWritePointers(), 1024) == 0);

    stream.fill();

    juce::AudioBuffer<float> expected(2, 4096);

    CHECK(stream.read(buffer.getArrayOfWritePointers(), 1024) == 1024);
    CHECK(stream.read(buffer.getArrayOfWritePointers(), 1024) == 1024);
    CHECK(stream.read(buffer.getArrayOfWritePointers(), 1024) == 1024);
    CHECK(stream.read(buffer.getArrayOfWritePointers(), 1024) == 1024);
    CHECK(stream.read(buffer.getArrayOfWritePointers(), 1024) == 0);

    stream.fill();
    CHECK(stream.read(buffer.getArrayOfWritePointers(), 1024) == 1024);
    CHECK(stream.read(buffer.getArrayOfWritePointers(), 1024) == 1024);
    stream.fill();
    CHECK(stream.read(buffer.getArrayOfWritePointers(), 4096) == 4096);

    reader->read(expected.getArrayOfWritePointers(), 2, 6144, 4096);
    for (int channel = 0; channel < 2; ++channel) {
        const float* buffer1 = buffer.getReadPointer(channel);
        const float* buffer2 = expected.getReadPointer(channel);
        bool equal = true;
        for (size_t i = 0; i < 4096; ++i) {
            if (buffer1[i] != doctest::Approx(buffer2[i])) {
                DBG("sample " + std::to_string(i) + ": " + std::to_string(buffer1[i]) + " vs " + std::to_string(buffer2[i]));
                equal = false;
                break;
            }
        }
        CHECK(equal);
    }

    stream.fill();

    while (stream.read(buffer.getArrayOfWritePointers(), 3000) > 0) {
        if (stream.check_refill()) {
            stream.fill();
        }
    }
    CHECK(stream.is_done());
    delete reader;
}
