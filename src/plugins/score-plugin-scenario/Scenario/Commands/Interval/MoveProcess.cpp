#include "MoveProcess.hpp"

#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Scenario/Process/Algorithms/ProcessPolicy.hpp>
#include <Scenario/Process/ScenarioInterface.hpp>
#include <Scenario/Settings/ScenarioSettingsModel.hpp>

#include <Dataflow/Commands/CableHelpers.hpp>

#include <score/application/ApplicationContext.hpp>
#include <score/application/GUIApplicationContext.hpp>
#include <score/document/ChangeId.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/model/path/ObjectPath.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <score/tools/IdentifierGeneration.hpp>

#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/pod_vector.hpp>

namespace Scenario::Command
{

MoveProcess::MoveProcess(
    const IntervalModel& src, const IntervalModel& tgt,
    Id<Process::ProcessModel> processId, bool addSlot)
    : m_src{src}
    , m_tgt{tgt}
    , m_oldId{processId}
    , m_newId{getStrongId(tgt.processes)}
    , m_oldSmall{src.smallView()}
    , m_oldFull{src.fullView()}
    , m_addedSlot{addSlot}
{
  int i = 0;
  for(auto& e : src.fullView())
  {
    if(e.process == m_oldId)
    {
      m_oldPos = i;
      break;
    }
    i++;
  }
}

static void moveProcess(
    Scenario::IntervalModel& src, Scenario::IntervalModel& tgt,
    Process::ProcessModel& proc, const Id<Process::ProcessModel>& old_id,
    const Id<Process::ProcessModel>& new_id, const score::DocumentContext& ctx)
{
  auto cables = Dataflow::saveCables({&proc}, ctx);
  Dataflow::removeCables(cables, ctx);

  const auto old_path = score::IDocument::unsafe_path(proc);

  // 1. Remove the process from the old interval
  {
    ossia::int_vector slots_to_remove;
    int i = 0;

    const bool nodal = proc.flags() & Process::ProcessFlags::TimeIndependent;
    if(!nodal)
    {
      for(const Slot& slot : src.smallView())
      {
        if(slot.processes.size() == 1 && slot.processes[0] == proc.id())
          slots_to_remove.push_back(i);
        i++;
      }
    }

    EraseProcess(src, proc.id());
    if(!nodal)
    {
      for(auto it = slots_to_remove.rbegin(); it != slots_to_remove.rend(); ++it)
        src.removeSlot(*it);
    }
  }

  // 2. Add the process to its new parent
  {
    proc.setParent(&tgt);
    score::IDocument::changeObjectId(proc, new_id);

    AddProcess(tgt, &proc);
  }

  // 3. Re-add the cables
  {
    const auto new_path = score::IDocument::path(proc).unsafePath();

    Dataflow::loadCables(old_path, new_path, cables, ctx);
  }
}

void MoveProcess::undo(const score::DocumentContext& ctx) const
{
  auto& src = m_src.find(ctx);
  auto& tgt = m_tgt.find(ctx);

  Process::ProcessModel& proc = tgt.processes.at(m_newId);
  moveProcess(tgt, src, proc, m_newId, m_oldId, ctx);

  const bool nodal = proc.flags() & Process::ProcessFlags::TimeIndependent;
  if(!nodal)
  {
    src.replaceSmallView(m_oldSmall);
    src.replaceFullView(m_oldFull);
    src.setSmallViewVisible(true);
  }

  // slot will be removed if it is empty
}

void MoveProcess::redo(const score::DocumentContext& ctx) const
{
  auto& src = m_src.find(ctx);
  auto& tgt = m_tgt.find(ctx);

  Process::ProcessModel& proc = src.processes.at(m_oldId);
  moveProcess(src, tgt, proc, m_oldId, m_newId, ctx);

  const bool nodal = proc.flags() & Process::ProcessFlags::TimeIndependent;
  if(!nodal)
  {
    if(m_addedSlot)
    {
      auto h = ctx.app.settings<Scenario::Settings::Model>().getSlotHeight();
      tgt.addSlot(Slot{{m_newId}, m_newId, h});
      tgt.setSmallViewVisible(true);
    }
  }
}

void MoveProcess::serializeImpl(DataStreamInput& s) const
{
  s << m_src << m_tgt << m_oldId << m_newId << m_oldSmall << m_oldFull << m_oldPos
    << m_addedSlot;
}

void MoveProcess::deserializeImpl(DataStreamOutput& s)
{
  s >> m_src >> m_tgt >> m_oldId >> m_newId >> m_oldSmall >> m_oldFull >> m_oldPos
      >> m_addedSlot;
}
}
