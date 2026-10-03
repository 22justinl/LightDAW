#include "gui/FileImportComponent.h"
#include "juce_gui_basics/juce_gui_basics.h"

#include <juce_events/juce_events.h>

FileImportWindow::FileImportWindow(AudioEngine& audio_engine_, FileImportWindow*& window_ptr_)
    :   juce::DocumentWindow("Import Clip", juce::Colours::grey, juce::DocumentWindow::closeButton), window_ptr(window_ptr_)
{
    setContentOwned(new FileImportComponent(audio_engine_), true);
    centreWithSize(500, 220);
    setResizable(false, false);
    setVisible(true);
    window_ptr = this;
}
void FileImportWindow::closeButtonPressed() {
    window_ptr = nullptr;
    delete this;
}

FileImportComponent::FileImportComponent(AudioEngine& audio_engine_): audio_engine(audio_engine_) {
    file_label.setText("File : ", juce::dontSendNotification);
    file_select_button.onClick = [&]() { 
        audio_file_chooser = std::make_unique<juce::FileChooser>("Select an Audio File", juce::File(), "*.wav;*.aiff;*.mp3"); // TODO: other file formats
        audio_file_chooser->launchAsync(
                juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [&](const juce::FileChooser& file_chooser) {
            const auto file = file_chooser.getResult();
            valid_file = file.existsAsFile();
            if (valid_file) {
                clip_file = file;
                file_name_label.setText(clip_file.getFileName(), juce::dontSendNotification);
            } else {
                clip_file = juce::File();
                file_name_label.setText("No file selected", juce::dontSendNotification);
            }
        });
    };

    track_label.setText("Track : ", juce::dontSendNotification);
    auto& tracks = audio_engine.get_tracks();
    auto& track_ids = audio_engine.get_track_ids();
    for (size_t i = 0; i < track_ids.size(); ++i) {
        track_box.addItem(tracks[track_ids[i]]->get_name(), static_cast<int>(track_ids[i]+1));
    }

    position_label.setText("Position: ", juce::dontSendNotification);
    position_editor.setInputRestrictions(0, "0123456789");

    import_button.onClick = [&]() {
        clip_pos = position_editor.getText().getLargeIntValue();

        if (!valid_file) {
            show_status("Please select a valid file");
        } else if (track_box.getSelectedId() <= 0) {
            show_status("Please select a track");
        } else if (clip_pos < 0) {
            show_status("Please select a valid position");
        } else {
            show_status("Added clip successfully");
            audio_engine.track_import_file(static_cast<TrackId>(track_box.getSelectedId())-1, clip_file, clip_pos);
            reset();
        }
    };

    reset();

    addAndMakeVisible(file_label);
    addAndMakeVisible(file_name_label);
    addAndMakeVisible(file_select_button);
    addAndMakeVisible(track_label);
    addAndMakeVisible(track_box);
    addAndMakeVisible(position_label);
    addAndMakeVisible(position_editor);
    addAndMakeVisible(import_button);
    addAndMakeVisible(status_label);
}

void FileImportComponent::paint (juce::Graphics& g) {
    g.fillAll(juce::Colours::grey);
}

void FileImportComponent::resized() {
    auto area = getLocalBounds().reduced(10);
    auto main_area = area.removeFromTop(125);
    auto left_col = main_area.removeFromLeft(75);
    auto right_col = main_area.removeFromRight(75);

    file_label.setBounds(left_col.removeFromTop(25));
    track_label.setBounds(left_col.removeFromTop(25));
    position_label.setBounds(left_col.removeFromTop(25));
    file_select_button.setBounds(right_col.removeFromTop(25));

    file_name_label.setBounds(main_area.removeFromTop(25));
    track_box.setBounds(main_area.removeFromTop(25));
    position_editor.setBounds(main_area.removeFromTop(25));
    import_button.setBounds(main_area.removeFromTop(25));

    status_label.setBounds(area.removeFromBottom(25));
}

void FileImportComponent::reset() {
    clip_file = juce::File();
    file_name_label.setText("No file selected", juce::dontSendNotification);
    position_editor.setText("0");
    track_box.setSelectedId(0);
}

void FileImportComponent::show_status(const juce::String& message) {
    status_label.setText(message, juce::dontSendNotification);
    startTimer(3000);
}

void FileImportComponent::timerCallback() {
    status_label.setText("", juce::dontSendNotification);
    stopTimer();
}
