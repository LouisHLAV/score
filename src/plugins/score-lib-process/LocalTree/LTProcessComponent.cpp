// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ProcessComponent.hpp"

#include <Process/Process.hpp>

LocalTree::ProcessComponent::ProcessComponent(
    ossia::net::node_base& parentNode, Process::ProcessModel& proc,
    const score::DocumentContext& doc, const QString& name, QObject* parent)
    : Component<Process::GenericProcessComponent<const score::DocumentContext>>{
        parentNode, proc.metadata(), proc, doc, name, parent}
{
  SCORE_ASSERT(!proc.metadata().getName().isEmpty());
}

LocalTree::ProcessComponent::~ProcessComponent() = default;
LocalTree::ProcessComponentFactory::~ProcessComponentFactory() = default;
LocalTree::ProcessComponentFactoryList::~ProcessComponentFactoryList() = default;
