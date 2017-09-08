// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <Process/Process.hpp>
#include <QList>
#include <QObject>
#include <Scenario/Commands/Interval/AddOnlyProcessToInterval.hpp>
#include <Scenario/Commands/Interval/Rack/AddSlotToRack.hpp>
#include <Scenario/Commands/Interval/Rack/Slot/AddLayerModelToSlot.hpp>
#include <Scenario/Commands/Interval/Rack/Slot/ResizeSlotVertically.hpp>
#include <Scenario/Commands/Scenario/ShowRackInViewModel.hpp>
#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentPresenter.hpp>
#include <Scenario/Document/Interval/Slot.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Scenario/Process/ScenarioProcessMetadata.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <iscore/document/DocumentInterface.hpp>
#include <iscore/tools/std/Optional.hpp>

#include "ScenarioDocumentModel.hpp"
#include <Scenario/Document/Interval/IntervalDurations.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/DisplayedElements/DisplayedElementsModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/ScenarioDocument/ProcessFocusManager.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>

#include <iscore/application/ApplicationContext.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/plugins/documentdelegate/DocumentDelegateModel.hpp>
#include <iscore/selection/SelectionDispatcher.hpp>
#include <iscore/selection/SelectionStack.hpp>
#include <iscore/model/EntityMap.hpp>
#include <iscore/model/IdentifiedObject.hpp>
#include <iscore/model/Identifier.hpp>
#include <iscore/tools/Todo.hpp>

#include <Process/LayerPresenter.hpp>
#include <QFileInfo>
#include <Scenario/Document/DisplayedElements/DisplayedElementsProviderList.hpp>
#include <Scenario/Settings/ScenarioSettingsModel.hpp>
#include <algorithm>
#include <chrono>
#include <QApplication>
#include <core/document/Document.hpp>
#include <iscore/tools/IdentifierGeneration.hpp>

namespace Process
{
class LayerPresenter;
}
namespace Scenario
{
ScenarioDocumentModel::ScenarioDocumentModel(
    const iscore::DocumentContext& ctx, QObject* parent)
  : iscore::
    DocumentDelegateModel{Id<iscore::DocumentDelegateModel>(
                            iscore::id_generator::getFirstId()),
                          "Scenario::ScenarioDocumentModel",
                          parent}
  , m_baseScenario{new BaseScenario{Id<BaseScenario>{0}, this}}
{
  auto dur
      = ctx.app.settings<Scenario::Settings::Model>().getDefaultDuration();

  m_baseScenario->interval().duration.setRigid(false);
  IntervalDurations::Algorithms::changeAllDurations(
      m_baseScenario->interval(), dur);
  m_baseScenario->interval().duration.setMaxInfinite(true);
  m_baseScenario->endEvent().setDate(
      m_baseScenario->interval().duration.defaultDuration());
  m_baseScenario->endTimeSync().setDate(
      m_baseScenario->interval().duration.defaultDuration());

  auto& doc_metadata
      = iscore::IDocument::documentContext(*parent).document.metadata();
  m_baseScenario->interval().metadata().setName(doc_metadata.fileName());

  connect(
      &doc_metadata, &iscore::DocumentMetadata::fileNameChanged, this,
      [&](const QString& newName) {
        QFileInfo info(newName);

        m_baseScenario->interval().metadata().setName(info.baseName());
      });

  using namespace Scenario::Command;

  AddOnlyProcessToInterval cmd1{
      m_baseScenario->interval(),
      Metadata<ConcreteKey_k, Scenario::ProcessModel>::get()};
  cmd1.redo(ctx);
  m_baseScenario->interval().processes.begin()->setSlotHeight(1500);

  init();

  // Select the first state
  iscore::SelectionDispatcher d{ctx.selectionStack};
  auto scenar = dynamic_cast<Scenario::ProcessModel*>(
      &*m_baseScenario->interval().processes.begin());
  if (scenar)
    d.setAndCommit({&scenar->startEvent()});
}

void ScenarioDocumentModel::init()
{
}

void ScenarioDocumentModel::initializeNewDocument(
    const IntervalModel& interval_model) {
}

IntervalModel& ScenarioDocumentModel::baseInterval() const
{
  return m_baseScenario->interval();
}


}
