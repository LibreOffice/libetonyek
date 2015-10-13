/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWAMESSAGE_H_INCLUDED
#define IWAMESSAGE_H_INCLUDED

#include <map>
#include <utility>

#include "IWAField.h"

namespace libetonyek
{

class IWAMessage
{
public:
  IWAMessage(const RVNGInputStreamPtr_t &input, unsigned long length);
  IWAMessage(const RVNGInputStreamPtr_t &input, long start, long end);

  const IWAUInt32Field &uint32(std::size_t field) const;
  const IWAUInt64Field &uint64(std::size_t field) const;
  const IWASInt32Field &sint32(std::size_t field) const;
  const IWASInt64Field &sint64(std::size_t field) const;
  const IWABoolField &bool_(std::size_t field) const;

  const IWAFixed64Field &fixed64(std::size_t field) const;
  const IWADoubleField &double_(std::size_t field) const;

  const IWAStringField &string(std::size_t field) const;
  const IWAMessageField &message(std::size_t field) const;

  const IWAFixed32Field &fixed32(std::size_t field) const;
  const IWAFloatField &float_(std::size_t field) const;

private:
  enum WireType
  {
    WIRE_TYPE_VARINT,
    WIRE_TYPE_64_BIT,
    WIRE_TYPE_LENGTH_DELIMITED,
    WIRE_TYPE_32_BIT = 5
  };

  typedef std::pair<long, long> InputRange_t;

  struct Field
  {
    explicit Field(WireType wireType);

    WireType m_wireType;
    std::deque<InputRange_t> m_pieces;
    IWAFieldPtr_t m_realField;
  };

  typedef std::map<unsigned, Field> FieldList_t;

private:
  void parse(unsigned long length);

  template<typename FieldT>
  const FieldT &getField(std::size_t field, WireType wireType, IWAField::Tag tag) const;

private:
  RVNGInputStreamPtr_t m_input;
  mutable FieldList_t m_fields;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
