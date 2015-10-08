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
#include <stdexcept>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

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

  virtual void parse(const RVNGInputStreamPtr_t &input, unsigned long length) = 0;
};

typedef boost::shared_ptr<IWAField> IWAFieldPtr_t;

namespace detail
{

template<IWAField::Tag TagV, typename ValueT, typename Reader>
class IWAFieldImpl : public IWAField
{
public:
  typedef ValueT value_type;
  typedef ValueT &reference_type;
  typedef const ValueT &const_reference_type;

public:
  virtual IWAField::Tag tag() const
  {
    return TagV;
  }

  virtual bool empty() const
  {
    return m_values.empty();
  }

  virtual std::size_t size() const
  {
    return m_values.size();
  }

  operator const std::deque<value_type> &() const
  {
    return m_values;
  }

  virtual bool is() const
  {
    return !m_values.empty();
  }

  const_reference_type operator[](const std::size_t index) const
  {
    if (index >= m_values.size())
      throw std::out_of_range("index is out of range");
    return m_values[index];
  }

  const_reference_type get() const
  {
    if (m_values.empty())
      throw std::logic_error("the field is unset");
    return m_values[0];
  }

  operator boost::optional<value_type>() const
  {
    return m_values.empty() ? boost::none : boost::make_optional(m_values.front());
  }

  template<typename U>
  operator boost::optional<U>() const
  {
    return m_values.empty() ? boost::none : boost::optional<U>(m_values.front());
  }

  virtual void parse(const RVNGInputStreamPtr_t &input, const unsigned long length)
  {
    const long start = input->tell();
    while (!input->isEnd() && (length > static_cast<unsigned long>(input->tell() - start)))
    {
      const value_type value(Reader::read(input, length));
      m_values.push_back(value);
    }
  }

private:
  std::deque<ValueT> m_values;
};

}

typedef detail::IWAFieldImpl<IWAField::TAG_UINT32, uint32_t, IWAReader::UInt32> IWAUInt32Field;
typedef detail::IWAFieldImpl<IWAField::TAG_UINT64, uint64_t, IWAReader::UInt64> IWAUInt64Field;
typedef detail::IWAFieldImpl<IWAField::TAG_SINT32, int32_t, IWAReader::SInt32> IWASInt32Field;
typedef detail::IWAFieldImpl<IWAField::TAG_SINT64, int64_t, IWAReader::SInt64> IWASInt64Field;
typedef detail::IWAFieldImpl<IWAField::TAG_BOOL, bool, IWAReader::Bool> IWABoolField;

typedef detail::IWAFieldImpl<IWAField::TAG_FIXED64, uint64_t, IWAReader::Fixed64> IWAFixed64Field;
typedef detail::IWAFieldImpl<IWAField::TAG_DOUBLE, double, IWAReader::Double> IWADoubleField;

typedef detail::IWAFieldImpl<IWAField::TAG_STRING, std::string, IWAReader::String> IWAStringField;

typedef detail::IWAFieldImpl<IWAField::TAG_FIXED32, uint32_t, IWAReader::Fixed32> IWAFixed32Field;
typedef detail::IWAFieldImpl<IWAField::TAG_FLOAT, float, IWAReader::Float> IWAFloatField;

class IWAMessageField : public detail::IWAFieldImpl<IWAField::TAG_MESSAGE, IWAMessage, IWAReader::Message>
{
public:
  IWAUInt32Field &uint32(std::size_t field) const;
  IWAUInt64Field &uint64(std::size_t field) const;
  IWASInt32Field &sint32(std::size_t field) const;
  IWASInt64Field &sint64(std::size_t field) const;
  IWABoolField &bool_(std::size_t field) const;

  IWAFixed64Field &fixed64(std::size_t field) const;
  IWADoubleField &double_(std::size_t field) const;

  IWAStringField &string(std::size_t field) const;
  IWAMessageField &message(std::size_t field) const;

  IWAFixed32Field &fixed32(std::size_t field) const;
  IWAFloatField &float_(std::size_t field) const;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
