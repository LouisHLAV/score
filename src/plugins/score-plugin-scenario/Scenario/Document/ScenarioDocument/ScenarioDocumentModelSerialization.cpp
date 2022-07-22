// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ScenarioDocumentModel.hpp"

#include <Process/Dataflow/Cable.hpp>

#include <score/model/EntityMapSerialization.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <score/serialization/VisitorCommon.hpp>

#include <core/document/Document.hpp>

#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <Scenario/Document/DisplayedElements/DisplayedElementsProviderList.hpp>

template <>
void DataStreamReader::read(const Scenario::ScenarioDocumentModel& model)
{
  readFrom(*model.m_baseScenario);

  if (model.cables.empty() && !model.m_savedCables.empty())
  {
    m_stream << model.m_savedCables;
  }
  else
  {
    std::vector<QByteArray> arr;
    for (auto& cable : model.cables)
      arr.push_back(DataStreamReader::marshall(cable));
    m_stream << arr;
  }

  std::vector<Path<Scenario::IntervalModel>> buses;
  for (auto& bus : model.busIntervals)
    buses.emplace_back(*bus);
  m_stream << buses;

  m_stream << model.baseInterval().duration.speed();

  insertDelimiter();
}

template <>
void DataStreamWriter::write(Scenario::ScenarioDocumentModel& model)
{
  model.m_baseScenario
      = new Scenario::BaseScenario{*this, model.m_context, &model};

  m_stream >> model.m_savedCables;

  auto& ctx = safe_cast<score::Document*>(model.parent()->parent())->context();

  std::vector<Path<Scenario::IntervalModel>> buses;
  m_stream >> buses;
  for (auto& path : buses)
  {
    model.busIntervals.push_back(&path.find(ctx));
  }

  double speed{1.};
  m_stream >> speed;
  model.baseInterval().duration.setSpeed(speed);

  checkDelimiter();
}

template <>
void JSONReader::read(const Scenario::ScenarioDocumentModel& model)
{
  obj["BaseScenario"] = *model.m_baseScenario;
  obj["Speed"] = model.baseInterval().duration.speed();
  obj["Cables"] = model.cables;

  stream.Key("BusIntervals");
  stream.StartArray();

  for (auto& bus : model.busIntervals)
  {
    readFrom(Path{*bus});
  }
  stream.EndArray();
}

template <>
void JSONWriter::write(Scenario::ScenarioDocumentModel& model)
{
  model.m_baseScenario = new Scenario::BaseScenario(
      JSONObject::Deserializer{obj["BaseScenario"]}, model.m_context, &model);

  if(auto cb = obj.tryGet("Cables"))
    model.m_savedCablesJson = clone(cb->obj);

  if(auto speed = obj.tryGet("Speed"); speed && speed->isDouble()) {
    model.baseInterval().duration.setSpeed(speed->toDouble());
  }
  auto& ctx = safe_cast<score::Document*>(model.parent()->parent())->context();

  if(auto b = obj.tryGet("BusIntervals"))
  {
    const auto& buses = b->toArray();
    for (const auto& bus : buses)
    {
      auto path = JsonValue{bus}.to<Path<Scenario::IntervalModel>>();
      model.busIntervals.push_back(&path.find(ctx));
    }
  }
}

void Scenario::ScenarioDocumentModel::serialize(
    const VisitorVariant& vis) const
{
  serialize_dyn(vis, *this);
}
