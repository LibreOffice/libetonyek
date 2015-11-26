/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWAMessage.h"

#include <cassert>

#include <boost/make_shared.hpp>

namespace libetonyek
{

using std::make_pair;

namespace
{

struct AccessError {};
struct ParseError {};

}

IWAMessage::Field::Field(const IWAMessage::WireType wireType)
  : m_wireType(wireType)
  , m_pieces()
  , m_realField()
{
}

IWAMessage::IWAMessage()
  : m_input()
  , m_fields()
{
}

IWAMessage::IWAMessage(const RVNGInputStreamPtr_t &input, unsigned long length)
  : m_input(input)
  , m_fields()
{
  assert(length > 0);

  parse(length);
}

IWAMessage::IWAMessage(const RVNGInputStreamPtr_t &input, const long start, const long end)
  : m_input(input)
  , m_fields()
{
  assert(end > start);

  if (input->seek(start, librevenge::RVNG_SEEK_SET) == 0)
    parse(static_cast<unsigned long>(end - start));
}

void IWAMessage::parse(const unsigned long length) try
{
  assert(bool(m_input));

  const long startPos = m_input->tell();
  while (!m_input->isEnd() && (length > static_cast<unsigned long>(m_input->tell() - startPos)))
  {
    const unsigned spec = unsigned(readUVar(m_input));
    const unsigned wireType = spec & 0x7;

    long start = m_input->tell();

    switch (wireType)
    {
    case 0:
      readUVar(m_input);
      break;
    case 1:
      readU64(m_input);
      break;
    case 2:
    {
      const uint64_t len = readUVar(m_input);
      start = m_input->tell(); // the field parser expects just the actual data
      if (m_input->seek(len, librevenge::RVNG_SEEK_CUR) != 0)
        throw ParseError();
      break;
    }
    case 5:
      readU32(m_input);
      break;
    default:
      ETONYEK_DEBUG_MSG(("IWAMessage::IWAMessage: unexpected wire type %d\n", wireType));
      throw ParseError();
    }

    const long end = m_input->tell();
    if (length >= static_cast<unsigned long>(end - startPos))
    {
      const unsigned field = spec >> 3;
      FieldList_t::iterator it = m_fields.find(field);
      if ((it != m_fields.end()) && (it->second.m_wireType != WireType(wireType)))
      {
        ETONYEK_DEBUG_MSG(("IWAMessage::IWAMessage: wire type %d of field %d does not match previously seen %d\n", wireType, field, it->second.m_wireType));
        continue;
      }
      if (it == m_fields.end())
        it = m_fields.insert(make_pair(field, Field(WireType(wireType)))).first;
      assert(it != m_fields.end());
      it->second.m_pieces.push_back(make_pair(start, end));
    }
  }
}
catch (...)
{
  // The format is quite robust: a small damage like a bit flip
  // cannot break more than a single message and there is a good
  // chance that it will not affect parsing significantly. So we try
  // to get as much data as possible, ignoring parsing errors.
}

const IWAUInt32Field &IWAMessage::uint32(const std::size_t field) const
{
  return getField<IWAUInt32Field>(field, WIRE_TYPE_VARINT, IWAField::TAG_UINT32);
}

const IWAUInt64Field &IWAMessage::uint64(const std::size_t field) const
{
  return getField<IWAUInt64Field>(field, WIRE_TYPE_VARINT, IWAField::TAG_UINT64);
}

const IWASInt32Field &IWAMessage::sint32(const std::size_t field) const
{
  return getField<IWASInt32Field>(field, WIRE_TYPE_VARINT, IWAField::TAG_SINT32);
}

const IWASInt64Field &IWAMessage::sint64(const std::size_t field) const
{
  return getField<IWASInt64Field>(field, WIRE_TYPE_VARINT, IWAField::TAG_SINT64);
}

const IWABoolField &IWAMessage::bool_(const std::size_t field) const
{
  return getField<IWABoolField>(field, WIRE_TYPE_VARINT, IWAField::TAG_BOOL);
}

const IWAFixed64Field &IWAMessage::fixed64(const std::size_t field) const
{
  return getField<IWAFixed64Field>(field, WIRE_TYPE_64_BIT, IWAField::TAG_FIXED64);
}

const IWADoubleField &IWAMessage::double_(const std::size_t field) const
{
  return getField<IWADoubleField>(field, WIRE_TYPE_64_BIT, IWAField::TAG_DOUBLE);
}

const IWAStringField &IWAMessage::string(const std::size_t field) const
{
  return getField<IWAStringField>(field, WIRE_TYPE_LENGTH_DELIMITED, IWAField::TAG_STRING);
}

const IWABytesField &IWAMessage::bytes(const std::size_t field) const
{
  return getField<IWABytesField>(field, WIRE_TYPE_LENGTH_DELIMITED, IWAField::TAG_BYTES);
}

const IWAMessageField &IWAMessage::message(const std::size_t field) const
{
  return getField<IWAMessageField>(field, WIRE_TYPE_LENGTH_DELIMITED, IWAField::TAG_MESSAGE);
}

const IWAFixed32Field &IWAMessage::fixed32(const std::size_t field) const
{
  return getField<IWAFixed32Field>(field, WIRE_TYPE_32_BIT, IWAField::TAG_FIXED32);
}

const IWAFloatField &IWAMessage::float_(const std::size_t field) const
{
  return getField<IWAFloatField>(field, WIRE_TYPE_32_BIT, IWAField::TAG_FLOAT);
}

template<typename FieldT>
const FieldT &IWAMessage::getField(const std::size_t field, const WireType wireType, const IWAField::Tag tag) const
{
  const FieldList_t::iterator fieldIt = m_fields.find(field);

  if (fieldIt == m_fields.end())
  {
    static FieldT dummy;
    return dummy;
  }

  if (fieldIt->second.m_wireType != wireType)
  {
    if (fieldIt->second.m_wireType != WIRE_TYPE_LENGTH_DELIMITED)
      throw AccessError();
  }

  if (bool(fieldIt->second.m_realField))
  {
    if (fieldIt->second.m_realField->tag() != tag)
      throw AccessError();
  }
  else
  {
    fieldIt->second.m_realField = boost::make_shared<FieldT>();
    for (std::deque<InputRange_t>::const_iterator it = fieldIt->second.m_pieces.begin(); it != fieldIt->second.m_pieces.end(); ++it)
    {
      assert(bool(m_input));
      m_input->seek(it->first, librevenge::RVNG_SEEK_SET);
      fieldIt->second.m_realField->parse(m_input, static_cast<unsigned long>(it->second - m_input->tell()), wireType == WIRE_TYPE_LENGTH_DELIMITED);
    }
  }

  return static_cast<FieldT &>(*fieldIt->second.m_realField);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
