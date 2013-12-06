/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYXMLREADER_H_INCLUDED
#define KEYXMLREADER_H_INCLUDED

#include <boost/function.hpp>

#include <librevenge-stream/librevenge-stream.h>

namespace libetonyek
{

class KEYXMLReader
{
  // -Weffc++
  KEYXMLReader(const KEYXMLReader &other);
  KEYXMLReader &operator=(const KEYXMLReader &other);

  struct Impl;

  enum Type
  {
    TYPE_ELEMENT = 0x1,
    TYPE_TEXT = 0x2
  };

  class NodeIterator
  {
    friend class KEYXMLReader;

  public:
    NodeIterator(const KEYXMLReader &reader, int types);

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
    friend class KEYXMLReader;

  public:
    explicit AttributeIterator(const KEYXMLReader &reader);

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
    explicit ElementIterator(const KEYXMLReader &reader);

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
    explicit MixedIterator(const KEYXMLReader &reader);

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
  explicit KEYXMLReader(librevenge::RVNGInputStream *input);
  KEYXMLReader(librevenge::RVNGInputStream *input, TokenizerFunction_t tokenizer);
  KEYXMLReader(const ElementIterator &iterator);
  KEYXMLReader(const MixedIterator &iterator);
  ~KEYXMLReader();

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

int getNameId(const KEYXMLReader::AttributeIterator &attribute);
int getNamespaceId(const KEYXMLReader::AttributeIterator &attribute);
int getId(const KEYXMLReader::AttributeIterator &attribute);
int getValueId(const KEYXMLReader::AttributeIterator &attribute);

int getNameId(const KEYXMLReader::ElementIterator &element);
int getNamespaceId(const KEYXMLReader::ElementIterator &element);
int getId(const KEYXMLReader::ElementIterator &element);
int getTextId(const KEYXMLReader::ElementIterator &element);

int getNameId(const KEYXMLReader &reader);
int getNamespaceId(const KEYXMLReader &reader);
int getId(const KEYXMLReader &reader);

}

#endif // KEYXMLREADER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
