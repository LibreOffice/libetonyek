/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWAFIELD_H_INCLUDED
#define IWAFIELD_H_INCLUDED

#include <deque>
#include <memory>
#include <stdexcept>

#include <boost/container/deque.hpp>
#include <boost/optional.hpp>

#include "IWAReader.h"
#include "libetonyek_utils.h"

namespace libetonyek
{

class IWAField
{
public:
  enum Tag
  {
    TAG_INT32,
    TAG_INT64,
    TAG_UINT32,
    TAG_UINT64,
    TAG_SINT32,
    TAG_SINT64,
    TAG_BOOL,
    TAG_ENUM,
    TAG_FIXED64,
    TAG_SFIXED64,
    TAG_DOUBLE,
    TAG_STRING,
    TAG_BYTES,
    TAG_MESSAGE,
    TAG_FIXED32,
    TAG_SFIXED32,
    TAG_FLOAT
  };

public:
  virtual ~IWAField() = 0;

  virtual Tag tag() const = 0;

  // repeated
  virtual bool empty() const = 0;
  virtual std::size_t size() const = 0;

  // optional
  virtual bool is() const = 0;
  operator bool() const;
  bool operator!() const;

  virtual void parse(const RVNGInputStreamPtr_t &input, unsigned long length, bool allowEmpty) = 0;
};

typedef std::shared_ptr<IWAField> IWAFieldPtr_t;

namespace detail
{

template<IWAField::Tag TagV, typename ValueT, typename Reader>
class IWAFieldImpl : public IWAField
{
  typedef boost::container::deque<ValueT> container_type;

public:
  typedef ValueT value_type;
  typedef ValueT &reference_type;
  typedef const ValueT &const_reference_type;
  typedef typename container_type::const_iterator const_iterator;
  typedef typename container_type::const_reverse_iterator const_reverse_iterator;

public:
  // classification

  virtual IWAField::Tag tag() const
  {
    return TagV;
  }

  // optional interface

  virtual bool is() const
  {
    return !m_values.empty();
  }

  const_reference_type get() const
  {
    if (m_values.empty())
      throw std::logic_error("the field is unset");
    return m_values[0];
  }

  // container interface

  virtual bool empty() const
  {
    return m_values.empty();
  }

  virtual std::size_t size() const
  {
    return m_values.size();
  }

  const_reference_type operator[](const std::size_t index) const
  {
    if (index >= m_values.size())
      throw std::out_of_range("index is out of range");
    return m_values[index];
  }

  const_iterator begin() const
  {
    return m_values.begin();
  }

  const_iterator end() const
  {
    return m_values.end();
  }

  const_reverse_iterator rbegin() const
  {
    return m_values.rbegin();
  }

  const_reverse_iterator rend() const
  {
    return m_values.rend();
  }

  // conversions

  // TODO: remove this or replace direct use of std::deque by a typedef
  const std::deque<value_type> repeated() const
  {
    const std::deque<value_type> values(m_values.begin(), m_values.end());
    return values;
  }

  const boost::optional<value_type> optional() const
  {
    return m_values.empty() ? boost::none : boost::make_optional(m_values.front());
  }

  // initialization

  virtual void parse(const RVNGInputStreamPtr_t &input, const unsigned long length, const bool allowEmpty)
  {
    if (length != 0)
    {
      const long start = input->tell();
      while (!input->isEnd() && (length > static_cast<unsigned long>(input->tell() - start)))
      {
        const value_type value(Reader::read(input, length));
        m_values.push_back(value);
      }
    }
    else if (allowEmpty)
    {
      m_values.push_back(value_type());
    }
  }

private:
  container_type m_values;
};

}

template<IWAField::Tag TagV, typename ValueT, typename Reader>
const ValueT &get(const detail::IWAFieldImpl<TagV, ValueT, Reader> &field)
{
  return field.get();
}

template<IWAField::Tag TagV, typename ValueT, typename Reader>
const ValueT &get_optional_value_or(const detail::IWAFieldImpl<TagV, ValueT, Reader> &field, const ValueT &value)
{
  return bool(field) ? field.get() : value;
}

template<IWAField::Tag TagV, typename ValueT, typename Reader, typename DefaultValueT>
const ValueT get_optional_value_or(const detail::IWAFieldImpl<TagV, ValueT, Reader> &field, const DefaultValueT &value)
{
  return bool(field) ? field.get() : ValueT(value);
}

typedef detail::IWAFieldImpl<IWAField::TAG_UINT32, uint32_t, IWAReader::UInt32> IWAUInt32Field;
typedef detail::IWAFieldImpl<IWAField::TAG_UINT64, uint64_t, IWAReader::UInt64> IWAUInt64Field;
typedef detail::IWAFieldImpl<IWAField::TAG_SINT32, int32_t, IWAReader::SInt32> IWASInt32Field;
typedef detail::IWAFieldImpl<IWAField::TAG_SINT64, int64_t, IWAReader::SInt64> IWASInt64Field;
typedef detail::IWAFieldImpl<IWAField::TAG_BOOL, bool, IWAReader::Bool> IWABoolField;

typedef detail::IWAFieldImpl<IWAField::TAG_FIXED64, uint64_t, IWAReader::Fixed64> IWAFixed64Field;
typedef detail::IWAFieldImpl<IWAField::TAG_DOUBLE, double, IWAReader::Double> IWADoubleField;

typedef detail::IWAFieldImpl<IWAField::TAG_STRING, std::string, IWAReader::String> IWAStringField;
typedef detail::IWAFieldImpl<IWAField::TAG_BYTES, RVNGInputStreamPtr_t, IWAReader::Bytes> IWABytesField;

typedef detail::IWAFieldImpl<IWAField::TAG_FIXED32, uint32_t, IWAReader::Fixed32> IWAFixed32Field;
typedef detail::IWAFieldImpl<IWAField::TAG_FLOAT, float, IWAReader::Float> IWAFloatField;

class IWAMessageField : public detail::IWAFieldImpl<IWAField::TAG_MESSAGE, IWAMessage, IWAReader::Message>
{
public:
  const IWAUInt32Field &uint32(std::size_t field) const;
  const IWAUInt64Field &uint64(std::size_t field) const;
  const IWASInt32Field &sint32(std::size_t field) const;
  const IWASInt64Field &sint64(std::size_t field) const;
  const IWABoolField &bool_(std::size_t field) const;

  const IWAFixed64Field &fixed64(std::size_t field) const;
  const IWADoubleField &double_(std::size_t field) const;

  const IWAStringField &string(std::size_t field) const;
  const IWABytesField &bytes(std::size_t field) const;
  const IWAMessageField &message(std::size_t field) const;

  const IWAFixed32Field &fixed32(std::size_t field) const;
  const IWAFloatField &float_(std::size_t field) const;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
