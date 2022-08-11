// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "InspectorWidgetList.hpp"

#include "InspectorWidgetBase.hpp"

#include <Inspector/InspectorWidgetFactoryInterface.hpp>

#include <score/document/DocumentInterface.hpp>
#include <score/model/IdentifiedObjectAbstract.hpp>

class QWidget;

namespace Inspector
{
InspectorWidgetList::~InspectorWidgetList() { }

bool InspectorWidgetList::update(
    QWidget* cur, const QList<const IdentifiedObjectAbstract*>& models) const
{
  for(const InspectorWidgetFactory& factory : *this)
  {
    if(factory.update(cur, models))
      return true;
  }
  return false;
}

QList<QWidget*> InspectorWidgetList::make(
    const score::DocumentContext& doc,
    const QList<const IdentifiedObjectAbstract*>& models, QWidget* parent) const
{
  QList<QWidget*> widgs;
  for(const InspectorWidgetFactory& factory : *this)
  {
    if(factory.matches(models))
    {
      auto widg = factory.make(models, doc, parent);
      if(widg)
      {
        widgs.push_back(widg);
        return widgs;
      }
    }
  }

  return widgs;
}
}
