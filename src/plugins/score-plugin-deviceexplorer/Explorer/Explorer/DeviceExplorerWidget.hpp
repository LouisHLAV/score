#pragma once

#include <Device/Node/DeviceNode.hpp>

#include <Explorer/Explorer/ListeningManager.hpp>

#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/model/tree/TreePath.hpp>

#include <QWidget>

#include <memory>
#include <verdigris>

class QAction;
class QContextMenuEvent;
class QProgressIndicator;
class QStackedLayout;

class QToolButton;
class QComboBox;
class QLineEdit;
class QTableView;
namespace score
{
struct GUIApplicationContext;
}
namespace Device
{
class ProtocolFactoryList;
class AddressDialog;
}

namespace Explorer
{
class ExplorerSearchLineEdit;
class AddressItemModel;
class ListeningHandler;
class DeviceEditDialog;
class DeviceExplorerFilterProxyModel;
class DeviceExplorerModel;
class DeviceExplorerView;
class SCORE_PLUGIN_DEVICEEXPLORER_EXPORT DeviceExplorerWidget final : public QWidget
{
  W_OBJECT(DeviceExplorerWidget)

public:
  explicit DeviceExplorerWidget(const Device::ProtocolFactoryList&, QWidget* parent);

  void setModel(DeviceExplorerModel* model);
  DeviceExplorerModel* model() const;
  DeviceExplorerView* view() const;

  // Will block the GUI when refreshing.
  void blockGUI(bool);

  // Enable / disable actions that may change the devices.
  void setEditable(bool);

  QModelIndex sourceIndex(QModelIndex) const;
  QModelIndex proxyIndex(QModelIndex) const;

  ListeningManager& listeningManager() const { return *m_listeningManager; }

private:
  friend class ExplorerSearchLineEdit;
  QSize sizeHint() const override;
  // User commands
  void edit();
  void refresh();
  void refreshValue();

  void disconnect();
  void reconnect();

  void addAddress(InsertMode insertType);
  void addDevice();
  void exportDevice();
  void addChild();
  void addSibling();

  void removeNodes();
  void learn();
  void findUsage();

  // Answer to user interaction
  void updateActions();
  void updateAddressView();

  bool editable() const noexcept;

  // Utilities
  DeviceExplorerFilterProxyModel* proxyModel();

  void buildGUI();
  void populateColumnCBox();

  void contextMenuEvent(QContextMenuEvent* event) override;

  const Device::ProtocolFactoryList& m_protocolList;

  DeviceExplorerView* m_ntView{};
  DeviceExplorerFilterProxyModel* m_proxyModel{};
  QPointer<DeviceEditDialog> m_deviceDialog{};
  AddressItemModel* m_addressModel{};
  QTableView* m_addressView{};

  QAction* m_disconnect{};
  QAction* m_reconnect{};

  QAction* m_editAction{};
  QAction* m_refreshAction{};
  QAction* m_refreshValueAction{};
  QAction* m_addDeviceAction{};
  QAction* m_addSiblingAction{};
  QAction* m_addChildAction{};
  QAction* m_exportDeviceAction{};

  QAction* m_removeNodeAction{};
  QAction* m_learnAction{};
  QAction* m_findUsageAction{};

  QComboBox* m_columnCBox{};
  ExplorerSearchLineEdit* m_nameLEdit{};

  std::unique_ptr<CommandDispatcher<>> m_cmdDispatcher;

  QProgressIndicator* m_refreshIndicator{};
  QStackedLayout* m_lay{};

  QToolButton* m_openMenu{};

  std::unique_ptr<ListeningManager> m_listeningManager;
  QMetaObject::Connection m_modelCon;

  QMetaObject::Connection m_addressCon;

public:
  void findAddresses(QStringList strlst)
      E_SIGNAL(SCORE_PLUGIN_DEVICEEXPLORER_EXPORT, findAddresses, strlst)

private:
  void do_addAddress(
      InsertMode insert, QModelIndex index, Device::Node* parent,
      Device::AddressSettings& stgs);
};

SCORE_PLUGIN_DEVICEEXPLORER_EXPORT
DeviceExplorerWidget*
findDeviceExplorerWidgetInstance(const score::GUIApplicationContext& ctx) noexcept;
}
