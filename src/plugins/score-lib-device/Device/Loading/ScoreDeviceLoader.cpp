// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ScoreDeviceLoader.hpp"

#include <State/Domain.hpp>
#include <State/Value.hpp>

#include <Device/Address/AddressSettings.hpp>
#include <Device/Address/ClipMode.hpp>
#include <Device/Address/IOType.hpp>
#include <Device/Node/DeviceNode.hpp>

#include <score/model/tree/TreeNodeSerialization.hpp>
#include <score/tools/File.hpp>

#include <ossia/network/domain/domain.hpp>

#include <QDebug>
#include <QFile>
#include <QIODevice>

namespace Device
{

bool loadDeviceFromScoreJSON(const rapidjson::Document& json, Node& node)
{
  if(!json.IsObject())
  {
    return false;
  }

  if(!json.HasMember("Device"))
  {
    return false;
  }

  JSONObject::Deserializer des{json};
  des.writeTo(node);
  return true;
}

bool loadDeviceFromScoreJSON(const QString& filePath, Device::Node& node)
{
  QFile doc{filePath};
  if(!doc.open(QIODevice::ReadOnly))
  {
    qDebug() << "Unable to load the OSC device" << filePath;
    return false;
  }

  return loadDeviceFromScoreJSON(readJson(score::mapAsByteArray(doc)), node);
}

}
