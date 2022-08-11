#pragma once
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentViewConstants.hpp>
#include <Scenario/Document/VerticalExtent.hpp>

#include <score/graphics/TextItem.hpp>

#include <QGraphicsItem>

#include <score_plugin_scenario_export.h>
class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace Scenario
{
class TimeSyncPresenter;

class SCORE_PLUGIN_SCENARIO_EXPORT TimeSyncView final : public QGraphicsItem
{
public:
  TimeSyncView(TimeSyncPresenter& presenter, QGraphicsItem* parent);
  ~TimeSyncView();

  static const constexpr int Type = ItemType::TimeSync;
  int type() const final override { return Type; }

  const TimeSyncPresenter& presenter() const { return m_presenter; }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
      override;

  // QGraphicsItem interface
  QRectF boundingRect() const override
  {
    return {-3., 0., 6., m_extent.bottom() - m_extent.top()};
  }

  void setExtent(const VerticalExtent& extent);
  void setExtent(VerticalExtent&& extent);
  void setTriggerActive(bool);
  void addPoint(int newY);

  void setMoving(bool);
  void setSelected(bool selected);

  bool isSelected() const { return m_selected; }

  void changeColor(const score::Brush&);
  void setLabel(const QString& label);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  const QPixmap& pixmap(const score::Brush& b);
  TimeSyncPresenter& m_presenter;
  VerticalExtent m_extent;

  QPointF m_clickedPoint{};
  const score::Brush* m_color{};
  bool m_selected{};

  score::SimpleTextItem m_text;
};
}
