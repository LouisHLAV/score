#pragma once
#include <Device/Address/AddressSettings.hpp>

#include <Curve/Palette/CurvePoint.hpp>

#include <Automation/Commands/AutomationCommandFactory.hpp>

#include <score/command/Command.hpp>
#include <score/model/path/Path.hpp>

struct DataStreamInput;
struct DataStreamOutput;
namespace State
{
struct Address;
} // namespace score

namespace Automation
{
class ProcessModel;
class ChangeAddress final : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), ChangeAddress, "ChangeAddress")
public:
  ChangeAddress(const ProcessModel& autom, const State::AddressAccessor& newval);
  ChangeAddress(const ProcessModel& autom, Device::FullAddressAccessorSettings newval);
  ChangeAddress(const ProcessModel& autom, const Device::FullAddressSettings& newval);

public:
  void undo(const score::DocumentContext& ctx) const override;
  void redo(const score::DocumentContext& ctx) const override;

protected:
  void serializeImpl(DataStreamInput&) const override;
  void deserializeImpl(DataStreamOutput&) override;

private:
  Path<ProcessModel> m_path;
  Device::FullAddressAccessorSettings m_old, m_new;
};
}
