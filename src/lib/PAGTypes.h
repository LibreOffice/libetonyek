/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAGTYPES_H_INCLUDED
#define PAGTYPES_H_INCLUDED

#include <deque>
#include <string>

#include <boost/optional.hpp>

namespace libetonyek
{

struct PAGPublicationInfo
{
  PAGPublicationInfo();

  boost::optional<std::string> m_creationDate;
};

struct PAGMetadata
{
  PAGMetadata();

  boost::optional<std::string> m_title;
  std::deque<std::string> m_authors;
  std::deque<std::string> m_projects;
  std::deque<std::string> m_keywords;
  boost::optional<std::string> m_comment;
};

}

#endif //  PAGTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
