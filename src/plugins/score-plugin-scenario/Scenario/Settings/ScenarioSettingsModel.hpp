#pragma once
#include <Process/TimeValue.hpp>

#include <score/plugins/settingsdelegate/SettingsDelegateModel.hpp>

#include <score_plugin_scenario_export.h>

#include <verdigris>

namespace Process
{
class ProcessModel;
}
namespace Scenario
{
namespace Settings
{
class SCORE_PLUGIN_SCENARIO_EXPORT Model final : public score::SettingsDelegateModel
{
  W_OBJECT(Model)
  QString m_Skin;
  QString m_DefaultEditor;
  double m_GraphicZoom{};
  qreal m_SlotHeight{};
  TimeVal m_DefaultDuration{TimeVal::fromMsecs(30000)};
  int m_UpdateRate{60};
  int m_ExecutionRefreshRate{60};
  bool m_SnapshotOnCreate{};
  bool m_AutoSequence{};
  bool m_TimeBar{false};
  bool m_MeasureBars{true};
  bool m_MagneticMeasures{true};

public:
  Model(QSettings& set, const score::ApplicationContext& ctx);

  QString getSkin() const;
  void setSkin(const QString&);

  SCORE_SETTINGS_PARAMETER_HPP(SCORE_PLUGIN_SCENARIO_EXPORT, QString, DefaultEditor)
  SCORE_SETTINGS_PARAMETER_HPP(SCORE_PLUGIN_SCENARIO_EXPORT, double, GraphicZoom)
  SCORE_SETTINGS_PARAMETER_HPP(SCORE_PLUGIN_SCENARIO_EXPORT, qreal, SlotHeight)
  SCORE_SETTINGS_PARAMETER_HPP(SCORE_PLUGIN_SCENARIO_EXPORT, TimeVal, DefaultDuration)
  SCORE_SETTINGS_PARAMETER_HPP(SCORE_PLUGIN_SCENARIO_EXPORT, bool, SnapshotOnCreate)
  SCORE_SETTINGS_PARAMETER_HPP(SCORE_PLUGIN_SCENARIO_EXPORT, bool, AutoSequence)
  SCORE_SETTINGS_PARAMETER_HPP(SCORE_PLUGIN_SCENARIO_EXPORT, bool, TimeBar)
  SCORE_SETTINGS_PARAMETER_HPP(SCORE_PLUGIN_SCENARIO_EXPORT, bool, MeasureBars)
  SCORE_SETTINGS_PARAMETER_HPP(SCORE_PLUGIN_SCENARIO_EXPORT, bool, MagneticMeasures)
  SCORE_SETTINGS_PARAMETER_HPP(SCORE_PLUGIN_SCENARIO_EXPORT, int, UpdateRate)
  SCORE_SETTINGS_PARAMETER_HPP(SCORE_PLUGIN_SCENARIO_EXPORT, int, ExecutionRefreshRate)

public:
  void SkinChanged(const QString& arg_1)
      E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, SkinChanged, arg_1)
  PROPERTY(QString, Skin READ getSkin WRITE setSkin NOTIFY SkinChanged, W_Final)
};

SCORE_SETTINGS_PARAMETER(Model, DefaultEditor)
SCORE_SETTINGS_PARAMETER(Model, Skin)
SCORE_SETTINGS_PARAMETER(Model, GraphicZoom)
SCORE_SETTINGS_PARAMETER(Model, SlotHeight)
SCORE_SETTINGS_PARAMETER(Model, DefaultDuration)
SCORE_SETTINGS_PARAMETER(Model, SnapshotOnCreate)
SCORE_SETTINGS_PARAMETER(Model, AutoSequence)
SCORE_SETTINGS_PARAMETER(Model, TimeBar)
SCORE_SETTINGS_PARAMETER(Model, MeasureBars)
SCORE_SETTINGS_PARAMETER(Model, MagneticMeasures)
SCORE_SETTINGS_PARAMETER(Model, UpdateRate)
SCORE_SETTINGS_PARAMETER(Model, ExecutionRefreshRate)
}

double getNewLayerHeight(
    const score::ApplicationContext& ctx, const Process::ProcessModel& proc) noexcept;
}
