#pragma once
#include <Process/Dataflow/Port.hpp>
#include <Process/GenericProcessFactory.hpp>
#include <Process/Process.hpp>

#include <Nodal/Metadata.hpp>

#include <score/model/ObjectEditor.hpp>

namespace Nodal
{

class Model final : public Process::ProcessModel
{
  SCORE_SERIALIZE_FRIENDS
  PROCESS_METADATA_IMPL(Nodal::Model)
  W_OBJECT(Model)

public:
  std::unique_ptr<Process::AudioInlet> inlet;
  std::unique_ptr<Process::AudioOutlet> outlet;

  Model(
      const TimeVal& duration, const Id<Process::ProcessModel>& id,
      const score::DocumentContext& ctx, QObject* parent);

  template <typename Impl>
  Model(Impl& vis, const score::DocumentContext& ctx, QObject* parent)
      : Process::ProcessModel{vis, parent}
      , m_context{ctx}
  {
    vis.writeTo(*this);
    init();
  }

  void init()
  {
    m_inlets.push_back(inlet.get());
    m_outlets.push_back(outlet.get());
  }

  ~Model() override;

  score::EntityMap<Process::ProcessModel> nodes;

private:
  void loadPreset(const Process::Preset& preset) override;
  Process::Preset savePreset() const noexcept override;

  QString prettyName() const noexcept override;

  void setDurationAndScale(const TimeVal& newDuration) noexcept override;
  void setDurationAndGrow(const TimeVal& newDuration) noexcept override;
  void setDurationAndShrink(const TimeVal& newDuration) noexcept override;

  void ancestorStartDateChanged() override;
  void ancestorTempoChanged() override;

  const score::DocumentContext& m_context;
};

using ProcessFactory = Process::ProcessFactory_T<Nodal::Model>;

class NodeEditor : public score::ObjectEditor
{
  SCORE_CONCRETE("5e1c7e92-5beb-4313-92c8-f690089ff340")

  bool
  copy(JSONReader& r, const Selection& s, const score::DocumentContext& ctx) override;
  bool paste(
      QPoint pos, QObject* focusedObject, const QMimeData& mime,
      const score::DocumentContext& ctx) override;
  bool remove(const Selection& s, const score::DocumentContext& ctx) override;
};
}
