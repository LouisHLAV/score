// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "CreateCommentBlock.hpp"

#include <Process/TimeValueSerialization.hpp>

#include <Scenario/Document/CommentBlock/CommentBlockModel.hpp>
#include <Scenario/Process/Algorithms/StandardCreationPolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <score/model/path/PathSerialization.hpp>
#include <score/tools/IdentifierGeneration.hpp>

namespace Scenario
{
namespace Command
{
CreateCommentBlock::CreateCommentBlock(
    const Scenario::ProcessModel& scenar, TimeVal date, double yPosition)
    : m_path{scenar}
    , m_date{std::move(date)}
    , m_y{yPosition}
{
  m_id = getStrongId(scenar.comments);
}

void CreateCommentBlock::undo(const score::DocumentContext& ctx) const
{
  auto& scenar = m_path.find(ctx);
  ScenarioCreate<CommentBlockModel>::undo(m_id, scenar);
}

void CreateCommentBlock::redo(const score::DocumentContext& ctx) const
{
  auto& scenar = m_path.find(ctx);
  ScenarioCreate<CommentBlockModel>::redo(m_id, m_date, m_y, scenar);
}

void CreateCommentBlock::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_date << m_y << m_id;
}

void CreateCommentBlock::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_date >> m_y >> m_id;
}

RemoveCommentBlock::RemoveCommentBlock(
    const Scenario::ProcessModel& sc, const Scenario::CommentBlockModel& cb)
    : m_path{sc}
    , m_id{cb.id()}
    , m_block{score::marshall<DataStream>(cb)}
{
}

void RemoveCommentBlock::undo(const score::DocumentContext& ctx) const
{
  auto& scenar = m_path.find(ctx);

  auto comment = new CommentBlockModel{DataStreamWriter{m_block}, &scenar};
  scenar.comments.add(comment);
}

void RemoveCommentBlock::redo(const score::DocumentContext& ctx) const
{
  auto& scenar = m_path.find(ctx);
  ScenarioCreate<CommentBlockModel>::undo(m_id, scenar);
}

void RemoveCommentBlock::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_id << m_block;
}

void RemoveCommentBlock::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_id >> m_block;
}
}
}
