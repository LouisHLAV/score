#pragma once
#include <Process/Execution/ProcessComponent.hpp>

#include <Automation/AutomationModel.hpp>

#include <ossia/dataflow/node_process.hpp>
#include <ossia/network/value/destination.hpp>
#include <ossia/network/value/value.hpp>

#include <memory>
namespace ossia
{
class curve_abstract;
}

namespace Automation
{
namespace RecreateOnPlay
{
class Component final
    : public ::Execution::ProcessComponent_T<
          Automation::ProcessModel, ossia::node_process>
{
  COMPONENT_METADATA("f759eacd-5a67-4627-bbe8-c649e0f9b6c5")
public:
  Component(
      Automation::ProcessModel& element, const ::Execution::Context& ctx,
      QObject* parent);

  ~Component() override;

private:
  void recompute();

  std::shared_ptr<ossia::curve_abstract>
  on_curveChanged(ossia::val_type, const std::optional<ossia::destination>&);

  template <typename T>
  std::shared_ptr<ossia::curve_abstract>
  on_curveChanged_impl(const std::optional<ossia::destination>&);
};
using ComponentFactory = ::Execution::ProcessComponentFactory_T<Component>;
}
}

SCORE_CONCRETE_COMPONENT_FACTORY(
    Execution::ProcessComponentFactory, Automation::RecreateOnPlay::ComponentFactory)
