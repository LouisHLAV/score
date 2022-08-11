#pragma once
#include <Process/LayerView.hpp>
#include <Process/TimeValue.hpp>
#include <Process/ZoomHelper.hpp>

#include <Media/AudioArray.hpp>
#include <Media/MediaFileHandle.hpp>
#include <Media/Sound/WaveformComputer.hpp>

#include <score/graphics/GraphicsItem.hpp>

#include <ossia/detail/pod_vector.hpp>

#include <verdigris>
namespace Media
{
namespace Sound
{
class ProcessModel;
class LayerView final
    : public Process::LayerView
    , public Nano::Observer
{
public:
  explicit LayerView(const ProcessModel& model, QGraphicsItem* parent);
  ~LayerView();

  void setData(const std::shared_ptr<AudioFile>& data);
  void setFrontColors(bool);
  void setTempoRatio(double);
  void recompute(ZoomRatio ratio);
  void recompute() const;

  void on_finishedDecoding();

private:
  void paint_impl(QPainter*) const override;
  void mousePressEvent(QGraphicsSceneMouseEvent*) override;
  void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragLeaveEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  void heightChanged(qreal) override;
  void widthChanged(qreal) override;

  void scrollValueChanged(int);

  void on_newData();

  std::shared_ptr<AudioFile> m_data;
  int m_numChan{};
  int m_sampleRate{};

  ZoomRatio m_zoom{};
  double m_tempoRatio{1.};

  QVector<QImage*> m_images;
  WaveformComputer* m_cpt{};

  ComputedWaveform m_wf{};
  const ProcessModel& m_model;

  bool m_frontColors{true};
  mutable bool m_recomputed{false};
  mutable bool m_renderAll{true};
};
}
}
