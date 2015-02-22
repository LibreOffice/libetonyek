/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKXMLCONTEXTBASE_H_INCLUDED
#define IWORKXMLCONTEXTBASE_H_INCLUDED

#include <boost/enable_shared_from_this.hpp>
#include <boost/optional.hpp>

#include "IWORKXMLContext.h"
#include "IWORKTypes_fwd.h"

namespace libetonyek
{

class IWORKXMLParserState;

class KEYCollector;
class KEYDefaults;

class IWORKXMLContextBase : public IWORKXMLContext
{
protected:
  explicit IWORKXMLContextBase(IWORKXMLParserState &state);

  virtual void startOfElement();
  virtual void endOfAttributes();
  virtual void endOfElement();

  KEYCollector *getCollector() const;
  const KEYDefaults &getDefaults() const;
  int getToken(const char *value) const;

  IWORKXMLParserState &getState();

protected:
  IWORKXMLParserState &m_state;
};

class IWORKXMLElementContextBase : public IWORKXMLContextBase, public boost::enable_shared_from_this<IWORKXMLElementContextBase>
{
protected:
  explicit IWORKXMLElementContextBase(IWORKXMLParserState &state);

  virtual void attribute(int name, const char *value);
  virtual void text(const char *value);

  const boost::optional<ID_t> &getId() const;

private:
  boost::optional<ID_t> m_id;
};

class IWORKXMLTextContextBase : public IWORKXMLContextBase
{
protected:
  explicit IWORKXMLTextContextBase(IWORKXMLParserState &state);

  virtual IWORKXMLContextPtr_t element(int token);
};

class IWORKXMLMixedContextBase : public IWORKXMLContextBase, public boost::enable_shared_from_this<IWORKXMLMixedContextBase>
{
protected:
  explicit IWORKXMLMixedContextBase(IWORKXMLParserState &state);
};

class IWORKXMLEmptyContextBase : public IWORKXMLContextBase
{
protected:
  explicit IWORKXMLEmptyContextBase(IWORKXMLParserState &state);

  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int token);
  virtual void text(const char *value);

  const boost::optional<ID_t> &getId() const;
  const boost::optional<ID_t> &getRef() const;

private:
  boost::optional<ID_t> m_id;
  boost::optional<ID_t> m_ref;
};

}

#endif // IWORKXMLCONTEXTBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
