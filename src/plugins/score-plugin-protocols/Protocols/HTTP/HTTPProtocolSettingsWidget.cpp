// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "HTTPProtocolSettingsWidget.hpp"

#include "HTTPProtocolFactory.hpp"
#include "HTTPSpecificSettings.hpp"

#include <State/Widgets/AddressFragmentLineEdit.hpp>

#include <Device/Protocol/ProtocolSettingsWidget.hpp>

#include <Process/Script/ScriptWidget.hpp>

#include <score/widgets/TextLabel.hpp>

#include <QCodeEditor>
#include <QGridLayout>
#include <QLabel>
#include <QVariant>
class QWidget;

namespace Protocols
{
HTTPProtocolSettingsWidget::HTTPProtocolSettingsWidget(QWidget* parent)
    : ProtocolSettingsWidget(parent)
{
  QLabel* deviceNameLabel = new TextLabel(tr("Name"), this);
  m_deviceNameEdit = new State::AddressFragmentLineEdit{this};
  checkForChanges(m_deviceNameEdit);

  m_codeEdit = Process::createScriptWidget("JS");
  checkForChanges(m_codeEdit);

  QGridLayout* gLayout = new QGridLayout;

  gLayout->addWidget(deviceNameLabel, 0, 0, 1, 1);
  gLayout->addWidget(m_deviceNameEdit, 0, 1, 1, 1);
  gLayout->addWidget(m_codeEdit, 3, 0, 1, 2);

  setLayout(gLayout);

  setDefaults();
}

void HTTPProtocolSettingsWidget::setDefaults()
{
  SCORE_ASSERT(m_deviceNameEdit);
  SCORE_ASSERT(m_codeEdit);

  m_deviceNameEdit->setText("newDevice");
  m_codeEdit->setPlainText("");
}

Device::DeviceSettings HTTPProtocolSettingsWidget::getSettings() const
{
  SCORE_ASSERT(m_deviceNameEdit);

  Device::DeviceSettings s;
  s.name = m_deviceNameEdit->text();
  s.protocol = HTTPProtocolFactory::static_concreteKey();

  HTTPSpecificSettings specific;
  specific.text = m_codeEdit->toPlainText();

  s.deviceSpecificSettings = QVariant::fromValue(specific);
  return s;
}

void HTTPProtocolSettingsWidget::setSettings(const Device::DeviceSettings& settings)
{
  m_deviceNameEdit->setText(settings.name);
  HTTPSpecificSettings specific;
  if(settings.deviceSpecificSettings.canConvert<HTTPSpecificSettings>())
  {
    specific = settings.deviceSpecificSettings.value<HTTPSpecificSettings>();
    m_codeEdit->setPlainText(specific.text);
  }
}
}
