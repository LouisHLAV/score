#pragma once
#include <Process/Focus/FocusDispatcher.hpp>
#include <Process/LayerPresenter.hpp>

#include <Audio/Settings/Model.hpp>
#include <Media/Step/Commands.hpp>
#include <Media/Step/View.hpp>

#include <score/application/GUIApplicationContext.hpp>
#include <score/command/Dispatchers/SingleOngoingCommandDispatcher.hpp>
#include <score/tools/Bind.hpp>

#include <ossia/detail/math.hpp>
namespace Media
{
namespace Step
{
class Model;
class Presenter final : public Process::LayerPresenter
{
public:
  explicit Presenter(
      const Process::ProcessModel& model, View* view, const Process::Context& ctx,
      QObject* parent)
      : LayerPresenter{model, view, ctx, parent}
      , m_view{view}
      , m_disp{m_context.context.commandStack}
  {
    putToFront();
    auto& m = static_cast<const Step::Model&>(model);

    connect(view, &View::change, this, [&](std::size_t num, float v) {
      updateSteps(m, m_disp, num, v);
    });

    connect(view, &View::released, this, [&] { m_disp.commit(); });

    con(m, &Step::Model::stepsChanged, this, [&] { m_view->update(); });
    con(m, &Step::Model::stepCountChanged, this, [&] { m_view->update(); });
    con(m, &Step::Model::stepDurationChanged, this,
        [&] { on_zoomRatioChanged(m_ratio); });

    auto& audio_settings = context().context.app.settings<Audio::Settings::Model>();
    con(audio_settings, &Audio::Settings::Model::RateChanged, this,
        [&] { on_zoomRatioChanged(m_ratio); });
  }

  void setWidth(qreal width, qreal defaultWidth) override { m_view->setWidth(width); }
  void setHeight(qreal val) override { m_view->setHeight(val); }

  void putToFront() override { m_view->setVisible(true); }

  void putBehind() override { m_view->setVisible(false); }

  void on_zoomRatioChanged(ZoomRatio r) override
  {
    auto samplerate
        = 0.001 * context().context.app.settings<Audio::Settings::Model>().getRate();
    m_ratio = r;
    auto& m = static_cast<const Step::Model&>(m_process);
    auto v = TimeVal::fromMsecs(m.stepDuration() / samplerate).toPixels(r);
    m_view->setBarWidth(v);
  }

  void parentGeometryChanged() override { }

private:
  View* m_view{};
  SingleOngoingCommandDispatcher<Media::ChangeSteps> m_disp;
  ZoomRatio m_ratio{};
};
}
}
