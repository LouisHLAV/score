#pragma once
#include <Process/Execution/ProcessComponent.hpp>

#include <Vst/EffectModel.hpp>

#include <ossia/dataflow/node_process.hpp>

#include <verdigris>

namespace vst
{
class Executor final
    : public Execution::ProcessComponent_T<vst::Model, ossia::node_process>
{
  W_OBJECT(Executor)
  COMPONENT_METADATA("84bb8af9-bfb9-4819-8427-79787de716f3")

public:
  static constexpr bool is_unique = true;

  Executor(vst::Model& proc, const Execution::Context& ctx, QObject* parent);

private:
  template <typename Node_T>
  void setupNode(Node_T& node);
};
using ExecutorFactory = Execution::ProcessComponentFactory_T<Executor>;
}
