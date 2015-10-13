/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWAField.h"
#include "IWAMessage.h"

namespace libetonyek
{

IWAField::~IWAField()
{
}

IWAField::operator bool() const
{
  return is();
}

bool IWAField::operator!() const
{
  return !is();
}

const IWAUInt32Field &IWAMessageField::uint32(const std::size_t field) const
{
  return get().uint32(field);
}

const IWAUInt64Field &IWAMessageField::uint64(const std::size_t field) const
{
  return get().uint64(field);
}

const IWASInt32Field &IWAMessageField::sint32(const std::size_t field) const
{
  return get().sint32(field);
}

const IWASInt64Field &IWAMessageField::sint64(const std::size_t field) const
{
  return get().sint64(field);
}

const IWABoolField &IWAMessageField::bool_(const std::size_t field) const
{
  return get().bool_(field);
}

const IWAFixed64Field &IWAMessageField::fixed64(const std::size_t field) const
{
  return get().fixed64(field);
}

const IWADoubleField &IWAMessageField::double_(const std::size_t field) const
{
  return get().double_(field);
}

const IWAStringField &IWAMessageField::string(const std::size_t field) const
{
  return get().string(field);
}

const IWAMessageField &IWAMessageField::message(const std::size_t field) const
{
  return get().message(field);
}

const IWAFixed32Field &IWAMessageField::fixed32(const std::size_t field) const
{
  return get().fixed32(field);
}

const IWAFloatField &IWAMessageField::float_(const std::size_t field) const
{
  return get().float_(field);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
