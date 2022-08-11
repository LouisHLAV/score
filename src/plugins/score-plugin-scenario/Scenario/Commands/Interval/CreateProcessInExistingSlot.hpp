#pragma once
#include <score/command/AggregateCommand.hpp>

namespace Scenario
{
namespace Command
{
class CreateProcessInExistingSlot final : public score::AggregateCommand
{
  SCORE_COMMAND_DECL(
      CommandFactoryName(), CreateProcessInExistingSlot,
      "Create a process in an existing slot")
};
}
}
