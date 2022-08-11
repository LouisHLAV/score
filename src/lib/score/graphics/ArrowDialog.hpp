#pragma once
#include <QGraphicsItem>
#include <QPainterPath>

#include <score_lib_base_export.h>

#include <vector>

namespace score
{
class SCORE_LIB_BASE_EXPORT ArrowDialog
    : public QObject
    , public QGraphicsItem
{
public:
  ArrowDialog(QGraphicsScene* parent);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
      override;

private:
  const QPainterPath& getPath(QSizeF sz);

  QPainterPath createPath(QSizeF sz);
  static inline std::vector<std::pair<QSizeF, QPainterPath>> paths;
};
}
