#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>

#include <Mapping/Inspector/MappingInspectorWidget.hpp>
#include <Mapping/MappingModel.hpp>

namespace Mapping
{
class MappingInspectorFactory final
    : public Process::InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
  SCORE_CONCRETE("14b3dc85-6152-4526-8d61-6b038ec5d676")
};
}
