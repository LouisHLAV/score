#pragma once
#include <score/actions/Action.hpp>
#include <score/application/GUIApplicationContext.hpp>
#include <score/command/Command.hpp>
#include <score/command/CommandGeneratorMap.hpp>
#include <score/plugins/Addon.hpp>
#include <score/plugins/Interface.hpp>
#include <score/tools/std/HashMap.hpp>

#include <QObject>

#include <score_lib_base_export.h>

#include <utility>
namespace score
{
class DocumentDelegateFactory;
class InterfaceListBase;
struct GUIApplicationContext;
class GUIApplicationPlugin;
class PanelDelegateFactory;
class SettingsDelegateFactory;
struct ApplicationComponentsData;
class View;
class Settings;
class Plugin_QtInterface;

/**
 * @brief Registers the plug-in elements.
 *
 * Used at the plug-in loading time : stores the
 * classes of the plug-in and performs minor initialization steps.
 */

class SCORE_LIB_BASE_EXPORT ApplicationRegistrar : public QObject
{
public:
  ApplicationRegistrar(ApplicationComponentsData&);
  ~ApplicationRegistrar();

  void registerAddons(std::vector<score::Addon> vec);
  void registerApplicationPlugin(ApplicationPlugin*);

  void registerCommands(CommandStore&& cmds);
  void registerCommands(const CommandGroupKey& k, CommandGeneratorMap&& cmds);

  void registerFactories(
      score::hash_map<score::InterfaceKey, std::unique_ptr<InterfaceListBase>>&& cmds);
  void registerFactory(std::unique_ptr<InterfaceListBase> cmds);

  ApplicationComponentsData& components() const { return m_components; }

protected:
  ApplicationComponentsData& m_components;
};

class SCORE_LIB_BASE_EXPORT GUIApplicationRegistrar : public ApplicationRegistrar
{
public:
  GUIApplicationRegistrar(
      ApplicationComponentsData&, const score::GUIApplicationContext&, MenuManager&,
      ToolbarManager&, ActionManager&);
  ~GUIApplicationRegistrar();

  // Register data from plugins
  void registerGUIApplicationPlugin(GUIApplicationPlugin*);
  void registerPanel(PanelDelegateFactory&);

private:
  const score::GUIApplicationContext& m_context;

  MenuManager& m_menuManager;
  ToolbarManager& m_toolbarManager;
  ActionManager& m_actionManager;
};
}
