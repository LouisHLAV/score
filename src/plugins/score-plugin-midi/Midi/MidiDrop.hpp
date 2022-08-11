#pragma once
#include <Process/Drop/ProcessDropHandler.hpp>
#include <Process/TimeValue.hpp>

#include <Midi/MidiNote.hpp>
namespace Midi
{

class DropHandler final : public Process::ProcessDropHandler
{
  SCORE_CONCRETE("8F162598-9E4E-4865-A861-81DF01D2CDF0")

public:
  QSet<QString> mimeTypes() const noexcept override;
  QSet<QString> fileExtensions() const noexcept override;
  void dropData(
      std::vector<ProcessDrop>& drops, const DroppedFile& data,
      const score::DocumentContext& ctx) const noexcept override;
};

struct MidiTrack
{
  QString name;

  std::vector<Midi::NoteData> notes;
  int min{127}, max{0};

  struct MidiSong
  {
    std::vector<MidiTrack> tracks;
    double duration{};
    float tempo{};
    float tickPerBeat{};

    double durationInMs{};
  };
  static std::vector<MidiTrack::MidiSong>
  parse(const QMimeData& dat, const score::DocumentContext& ctx);
  static MidiSong parse(const QByteArray& dat, const score::DocumentContext& ctx);
};
}
