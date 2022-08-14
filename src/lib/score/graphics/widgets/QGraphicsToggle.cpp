#include <score/graphics/widgets/QGraphicsToggle.hpp>
#include <score/model/Skin.hpp>

#include <ossia/detail/math.hpp>

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include <wobjectimpl.h>
W_OBJECT_IMPL(score::QGraphicsToggle);

namespace score
{

QGraphicsToggle::QGraphicsToggle(
    const QString& textUntoggled, const QString& textToggled, QGraphicsItem* parent)
    : QGraphicsItem{parent}
    , m_textToggled(textToggled)
    , m_textUntoggled(textUntoggled)
{
  auto& skin = score::Skin::instance();
  setCursor(skin.CursorPointingHand);
  this->setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}

void QGraphicsToggle::toggle()
{
  m_toggled = !m_toggled;
  update();
}

void QGraphicsToggle::setState(bool toggled)
{
  if(toggled != m_toggled)
  {
    m_toggled = toggled;
    update();
  }
}

void QGraphicsToggle::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  m_toggled = !m_toggled;
  toggled(m_toggled);
  update();
  event->accept();
}

void QGraphicsToggle::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  event->accept();
}

void QGraphicsToggle::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  auto& skin = score::Skin::instance();
  painter->setRenderHint(QPainter::Antialiasing, true);

  constexpr const double margin = 2.;
  const double backgroundRectWidth = m_rect.width() - 2. * margin;
  const double backgroundRectHeight = m_rect.height() - 2. * margin;

  painter->setFont(skin.Medium10Pt);
  if(m_toggled)
  {
    painter->fillRect(
        QRectF{margin, margin, backgroundRectWidth, backgroundRectHeight},
        skin.Emphasis2.main.brush);

    painter->setPen(skin.Base4.main.pen1);
    painter->drawText(
        QRectF{margin, margin, backgroundRectWidth, backgroundRectHeight}, m_textToggled,
        QTextOption(Qt::AlignCenter));
  }
  else
  {
    painter->setPen(skin.Gray.main.pen1);
    painter->drawText(
        QRectF{margin, margin, backgroundRectWidth, backgroundRectHeight},
        m_textUntoggled, QTextOption(Qt::AlignCenter));
  }

  painter->setRenderHint(QPainter::Antialiasing, false);
}

QRectF QGraphicsToggle::boundingRect() const
{
  return m_rect;
}

}
