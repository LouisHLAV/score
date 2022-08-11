#pragma once
#include <Process/ExecutionAction.hpp>

#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/detail/lockfree_queue.hpp>

#include <score_plugin_audio_export.h>

namespace ossia
{
class audio_engine;
class audio_protocol;
}

namespace Audio
{

class SCORE_PLUGIN_AUDIO_EXPORT AudioPreviewExecutor : public Execution::ExecutionAction
{
  static inline AudioPreviewExecutor* m_instance{};
  SCORE_CONCRETE("333d0fab-a399-40e4-beea-c98ea79c10fa")
public:
  static AudioPreviewExecutor& instance();

  explicit AudioPreviewExecutor();

  void endTick(const ossia::audio_tick_state& st) override;

  struct sound
  {
    ossia::audio_handle handle;
    int channels{};
    int rate{};
  };

  sound current_sound{};
  int64_t currentPos{};
  bool playing{};
  ossia::spsc_queue<sound> queue;
};

}
