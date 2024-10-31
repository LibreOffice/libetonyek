/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libetonyek_xml.h"

#include "IWORKXMLContext.h"

namespace libetonyek
{

IWORKXMLContext::~IWORKXMLContext()
{
}

void IWORKXMLContext::CDATA(const char * /*value*/)
{
  ETONYEK_DEBUG_MSG(("IWORKXMLContext::cData: find unexpected CDATA block\n"));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
