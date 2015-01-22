/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKXMLREADER_H_INCLUDED
#define IWORKXMLREADER_H_INCLUDED

#include <boost/function.hpp>

#include <librevenge-stream/librevenge-stream.h>

namespace libetonyek
{

class IWORKXMLReader
{
  // -Weffc++
  IWORKXMLReader(const IWORKXMLReader &other);
  IWORKXMLReader &operator=(const IWORKXMLReader &other);

  struct Impl;

  enum Type
  {
    TYPE_ELEMENT = 0x1,
    TYPE_TEXT = 0x2
  };

  class NodeIterator
  {
    friend class IWORKXMLReader;

  public:
    NodeIterator(const IWORKXMLReader &reader, int types);

    bool next();

    Impl *getImpl() const;

  private:
    bool test() const;

  private:
    Impl *const m_impl;
    const int m_types;
    int m_level;
    bool m_last;
  };

public:

  class AttributeIterator
  {
    friend class IWORKXMLReader;

  public:
    explicit AttributeIterator(const IWORKXMLReader &reader);

    bool next();

    const char *getName() const;
    const char *getNamespace() const;
    const char *getValue() const;

    int getToken(const char *token) const;

  private:
    bool move();
    void read();
    bool test();

  private:
    Impl *const m_impl;
    bool m_first;
    bool m_last;
  };

  class ElementIterator
  {
  public:
    explicit ElementIterator(const IWORKXMLReader &reader);

    bool next();

    const char *getName() const;
    const char *getNamespace() const;
    const char *getText() const;
    bool isEmpty() const;

    int getToken(const char *token) const;

    const NodeIterator &getNodeIterator() const;

  private:
    NodeIterator m_iterator;
  };

  class MixedIterator
  {
  public:
    explicit MixedIterator(const IWORKXMLReader &reader);

    bool next();

    bool isElement() const;
    bool isText() const;

    const char *getName() const;
    const char *getNamespace() const;
    const char *getText() const;
    bool isEmpty() const;

    int getToken(const char *token) const;

    const NodeIterator &getNodeIterator() const;

  private:
    NodeIterator m_iterator;
  };

  typedef boost::function<int(const char *)> TokenizerFunction_t;

public:
  explicit IWORKXMLReader(librevenge::RVNGInputStream *input);
  IWORKXMLReader(librevenge::RVNGInputStream *input, TokenizerFunction_t tokenizer);
  IWORKXMLReader(const ElementIterator &iterator);
  IWORKXMLReader(const MixedIterator &iterator);
  ~IWORKXMLReader();

  /** Get the local name of the current element.
    *
    * @warning Can only be used before any iterator is used.
    */
  const char *getName() const;

  /** Get the namespace URI of the current element.
    *
    * @warning Can only be used before any iterator is used.
    */
  const char *getNamespace() const;

  int getToken(const char *token) const;

private:
  void initialize(librevenge::RVNGInputStream *input);

private:
  Impl *const m_impl;
  bool m_owner;
};

int getNameId(const IWORKXMLReader::AttributeIterator &attribute);
int getNamespaceId(const IWORKXMLReader::AttributeIterator &attribute);
int getId(const IWORKXMLReader::AttributeIterator &attribute);
int getValueId(const IWORKXMLReader::AttributeIterator &attribute);

int getNameId(const IWORKXMLReader::ElementIterator &element);
int getNamespaceId(const IWORKXMLReader::ElementIterator &element);
int getId(const IWORKXMLReader::ElementIterator &element);
int getTextId(const IWORKXMLReader::ElementIterator &element);

int getNameId(const IWORKXMLReader &reader);
int getNamespaceId(const IWORKXMLReader &reader);
int getId(const IWORKXMLReader &reader);

}

#endif // IWORKXMLREADER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
