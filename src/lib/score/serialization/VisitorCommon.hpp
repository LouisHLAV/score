#pragma once
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

#include <type_traits>

/**
 * \file VisitorCommon.hpp
 * This file contains useful functions
 * for simple serialization / deserialization of the common types
 * used in score, especially polymorphic types.
 *
 * The important point is the recursion for serialization.
 * This allows if we have a class hierarchy such as:
 *
 * IdentifiedObject <- A <- B <- C <- D
 *
 * to correctly serialize A, B, C, D without
 * needing to call the parent function's serialization function.
 */

namespace score
{

template <typename T>
void serialize_dyn_impl(DataStream::Serializer& v, const T& t)
{
  if constexpr(is_custom_serialized<T>::value)
    TSerializer<DataStream, T>{}.readFrom(v, t);
  else
    v.read(t);
}

template <typename T>
void serialize_dyn_impl(JSONObject::Serializer& v, const T& t)
{
  if constexpr(is_custom_serialized<T>::value)
    TSerializer<JSONObject, T>{}.readFrom(v, t);
  else
    v.read(t);
}

template <typename TheClass>
void serialize_dyn(const VisitorVariant& vis, const TheClass& s)
{
  if (vis.identifier == DataStream::type())
  {
    serialize_dyn_impl(static_cast<DataStream::Serializer&>(vis.visitor), s);
    return;
  }
  else if (vis.identifier == JSONObject::type())
  {
    serialize_dyn_impl(static_cast<JSONObject::Serializer&>(vis.visitor), s);
    return;
  }

  SCORE_ABORT;
}

template <typename TheClass>
TheClass& deserialize_dyn(const VisitorVariant& vis, TheClass& s)
{
  switch (vis.identifier)
  {
    case DataStream::type():
    {
      static_cast<DataStream::Deserializer&>(vis.visitor).writeTo(s);
      break;
    }
    case JSONObject::type():
    {
      static_cast<JSONObject::Deserializer&>(vis.visitor).writeTo(s);
      break;
    }
    default:
      SCORE_ABORT;
  }

  return s;
}

template <typename TheClass>
TheClass deserialize_dyn(const VisitorVariant& vis)
{
  TheClass s;

  switch (vis.identifier)
  {
    case DataStream::type():
    {
      static_cast<DataStream::Deserializer&>(vis.visitor).writeTo(s);
      break;
    }
    case JSONObject::type():
    {
      static_cast<JSONObject::Deserializer&>(vis.visitor).writeTo(s);
      break;
    }
    default:
      SCORE_ABORT;
  }
  return s;
}

template <typename Functor>
auto deserialize_dyn(const VisitorVariant& vis, Functor&& fun)
{
  switch (vis.identifier)
  {
    case DataStream::type():
    {
      return fun(static_cast<DataStream::Deserializer&>(vis.visitor));
      break;
    }
    case JSONObject::type():
    {
      return fun(static_cast<JSONObject::Deserializer&>(vis.visitor));
      break;
    }
    default:
      SCORE_ABORT;
      throw;
  }
}

/**
 * @brief marshall Serializes a single object
 * @param obj The object to serialize.
 *
 * This will create the relevant serialization datatype for Type.
 * For instance, a QByteArray if it is DataStream
 * and a QJSonObject if it is JSONObject
 */
template <typename Type, typename Object>
auto marshall(const Object& obj)
{
  return Type::Serializer::marshall(obj);
}

template <typename Object>
auto unmarshall(const rapidjson::Value& obj)
{
  Object data;
  JSONWriter wrt{obj};
  wrt.writeTo(data);
  return data;
}

template <typename Object>
auto unmarshall(const QByteArray& arr)
{
  return DataStreamWriter::unmarshall<Object>(arr);
}
}
