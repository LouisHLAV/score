// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "CommentBlockView.hpp"

#include <Process/Style/ScenarioStyle.hpp>

#include <Scenario/Document/CommentBlock/CommentBlockPresenter.hpp>

#include <score/graphics/TextItem.hpp>

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QTextCursor>
#include <QTextDocument>
#include <QWidget>

#include <cmath>
namespace Scenario
{
CommentBlockView::CommentBlockView(
    CommentBlockPresenter& presenter, QGraphicsItem* parent)
    : QGraphicsItem{parent}
    , m_presenter{presenter}
{
  this->setParentItem(parent);
  this->setZValue(ZPos::Comment);
  this->setAcceptHoverEvents(true);
  this->setToolTip(
      tr("Comment box\nPut the text you want in here by double-clicking !"));

  m_textItem = new score::TextItem{"", this};

  connect(m_textItem->document(), &QTextDocument::contentsChanged, this, [&]() {
    this->prepareGeometryChange();
  });
  connect(m_textItem, &score::TextItem::focusOut, this, &CommentBlockView::focusOut);
  focusOut();
}

void CommentBlockView::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  auto& skin = Process::Style::instance();

  if(!m_selected)
    painter->setPen(skin.CommentBlockPen());
  else
    painter->setPen(skin.CommentBlockSelectedPen());
  painter->setBrush(skin.TransparentBrush());
  painter->drawRoundedRect(boundingRect(), 5., 5.);
}

QRectF CommentBlockView::boundingRect() const
{
  if(m_textItem)
  {
    auto rect = m_textItem->boundingRect();
    rect.translate(-3., -3.);
    rect.setWidth(rect.width() + 6.);
    rect.setHeight(rect.height() + 6.);
    return rect;
  }
  else
    return {-1., -1., 2., 2.};
}

void CommentBlockView::setHtmlContent(QString htmlText)
{
  m_textItem->setHtml(htmlText);
}

void CommentBlockView::setSelected(bool b)
{
  if(b != m_selected)
  {
    m_selected = b;
    update();
  }
}

void CommentBlockView::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if(event->button() == Qt::MouseButton::LeftButton)
  {
    m_clickedPoint = event->scenePos() - this->pos();
    m_clickedScenePoint = event->scenePos();
  }
}

void CommentBlockView::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  m_presenter.moved(event->scenePos() - m_clickedPoint);
}

void CommentBlockView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  auto p = event->scenePos();
  auto d = (m_clickedScenePoint - p).manhattanLength();
  if(std::abs(d) < 5)
    m_presenter.selected();

  m_presenter.released(event->scenePos());
}

void CommentBlockView::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* evt)
{
  m_presenter.selected();

  focusOnText();
}

void CommentBlockView::focusOnText()
{
  if(m_textItem->textInteractionFlags() == Qt::NoTextInteraction)
  {
    m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_textItem->setFocus(Qt::MouseFocusReason);
    QTextCursor c = m_textItem->textCursor();
    c.select(QTextCursor::Document);
    m_textItem->setTextCursor(c);
  }
}

void CommentBlockView::focusOut()
{
  m_textItem->setTextInteractionFlags(Qt::NoTextInteraction);
  QTextCursor c = m_textItem->textCursor();
  c.clearSelection();
  m_textItem->setTextCursor(c);
  clearFocus();
  m_presenter.editFinished(m_textItem->toHtml());
}
}
