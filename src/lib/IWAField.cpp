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

IWAUInt32Field &IWAMessageField::uint32(const std::size_t field) const
{
  return get().uint32(field);
}

IWAUInt64Field &IWAMessageField::uint64(const std::size_t field) const
{
  return get().uint64(field);
}

IWASInt32Field &IWAMessageField::sint32(const std::size_t field) const
{
  return get().sint32(field);
}

IWASInt64Field &IWAMessageField::sint64(const std::size_t field) const
{
  return get().sint64(field);
}

IWABoolField &IWAMessageField::bool_(const std::size_t field) const
{
  return get().bool_(field);
}

IWAFixed64Field &IWAMessageField::fixed64(const std::size_t field) const
{
  return get().fixed64(field);
}

IWADoubleField &IWAMessageField::double_(const std::size_t field) const
{
  return get().double_(field);
}

IWAStringField &IWAMessageField::string(const std::size_t field) const
{
  return get().string(field);
}

IWAMessageField &IWAMessageField::message(const std::size_t field) const
{
  return get().message(field);
}

IWAFixed32Field &IWAMessageField::fixed32(const std::size_t field) const
{
  return get().fixed32(field);
}

IWAFloatField &IWAMessageField::float_(const std::size_t field) const
{
  return get().float_(field);
}


}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
