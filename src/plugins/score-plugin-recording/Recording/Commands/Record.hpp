#pragma once
#include <Recording/Commands/RecordingCommandFactory.hpp>

#include <score/command/AggregateCommand.hpp>

#include <ossia/detail/ssize.hpp>

namespace Recording
{
class Record final : public score::AggregateCommand
{
  SCORE_COMMAND_DECL(RecordingCommandFactoryName(), Record, "Record")
public:
  void undo(const score::DocumentContext& ctx) const override
  {
    const int N = std::ssize(m_cmds);
    // Undo 1
    if(N >= 2)
    {
      auto it = m_cmds.begin();
      std::advance(it, 1);
      (*it)->undo(ctx);
    }

    if(N >= 1)
    {
      // Undo 0
      (*m_cmds.begin())->undo(ctx);
    }
  }
};
}
