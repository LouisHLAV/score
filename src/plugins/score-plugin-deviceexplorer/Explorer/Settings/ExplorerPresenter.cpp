// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ExplorerPresenter.hpp"

#include "ExplorerModel.hpp"
#include "ExplorerView.hpp"

#include <score/command/Command.hpp>
#include <score/command/Dispatchers/ICommandDispatcher.hpp>
#include <score/command/SettingsCommand.hpp>
#include <score/tools/Bind.hpp>
#include <score/widgets/SetIcons.hpp>

#include <QApplication>
#include <QStyle>

#include <wobjectimpl.h>
W_OBJECT_IMPL(Explorer::Settings::Model)
W_OBJECT_IMPL(Explorer::Settings::View)
W_OBJECT_IMPL(Explorer::ProjectSettings::Model)
W_OBJECT_IMPL(Explorer::ProjectSettings::View)
namespace Explorer::Settings
{
Presenter::Presenter(Model& m, View& v, QObject* parent)
    : score::GlobalSettingsPresenter{m, v, parent}
{
  SETTINGS_PRESENTER(LogLevel);

  con(v, &View::localTreeChanged, this, [&](auto val) {
    if(val != m.getLocalTree())
    {
      m_disp.submitDeferredCommand<SetModelLocalTree>(this->model(this), val);
    }
  });

  con(m, &Model::LocalTreeChanged, &v, &View::setLocalTree);
  v.setLocalTree(m.getLocalTree());
}

QString Presenter::settingsName()
{
  return tr("Device explorer");
}

QIcon Presenter::settingsIcon()
{
  return makeIcons(
      QStringLiteral(":/icons/settings_device_explorer_on.png"),
      QStringLiteral(":/icons/settings_device_explorer_off.png"),
      QStringLiteral(":/icons/settings_device_explorer_off.png"));
}
}

namespace Explorer::ProjectSettings
{
Presenter::Presenter(Model& m, View& v, QObject* parent)
    : score::ProjectSettingsPresenter{m, v, parent}
{
  SETTINGS_PRESENTER(RefreshOnStart);
  SETTINGS_PRESENTER(ReconnectOnStart);
  SETTINGS_PRESENTER(MidiImportRatio);
}

QString Presenter::settingsName()
{
  return tr("Device explorer");
}

QIcon Presenter::settingsIcon()
{
  return makeIcons(
      QStringLiteral(":/icons/settings_device_explorer_on.png"),
      QStringLiteral(":/icons/settings_device_explorer_off.png"),
      QStringLiteral(":/icons/settings_device_explorer_off.png"));
}
}
