// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <score/model/tree/InvisibleRootNode.hpp>
#include <score/model/tree/TreeNodeItemModel.hpp>
#include <score/model/tree/TreePath.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

#include <ossia/detail/ssize.hpp>

QModelIndex TreeModel::convertPathToIndex(const TreePath& path) const
{
  QModelIndex iter;
  const int pathSize = std::ssize(path);

  for(int i = 0; i < pathSize; ++i)
  {
    iter = index(path.at(i), 0, iter);
  }

  return iter;
}

template <>
SCORE_LIB_BASE_EXPORT void DataStreamReader::read(const InvisibleRootNode&)
{
  insertDelimiter();
}

template <>
SCORE_LIB_BASE_EXPORT void DataStreamWriter::write(InvisibleRootNode&)
{
  checkDelimiter();
}

template <>
SCORE_LIB_BASE_EXPORT void JSONReader::read(const InvisibleRootNode&)
{
  stream.Null();
}

template <>
SCORE_LIB_BASE_EXPORT void JSONWriter::write(InvisibleRootNode&)
{
}
