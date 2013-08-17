/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNXMLREADER_H_INCLUDED
#define KNXMLREADER_H_INCLUDED

#include <boost/function.hpp>

class WPXInputStream;

namespace libkeynote
{

class KNXMLReader
{
  // -Weffc++
  KNXMLReader(const KNXMLReader &other);
  KNXMLReader &operator=(const KNXMLReader &other);

  struct Impl;

  typedef boost::function<int(const char *)> TokenizerFunction_t;

  enum Type
  {
    TYPE_ELEMENT = 0x1,
    TYPE_TEXT = 0x2,
  };

  class NodeIterator
  {
    friend class KNXMLReader;

  public:
    NodeIterator(const KNXMLReader &reader, int types);

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
    friend class KNXMLReader;

  public:
    explicit AttributeIterator(const KNXMLReader &reader);

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
    explicit ElementIterator(const KNXMLReader &reader);

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
    explicit MixedIterator(const KNXMLReader &reader);

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

public:
  explicit KNXMLReader(WPXInputStream *input);
  KNXMLReader(WPXInputStream *input, TokenizerFunction_t tokenizer);
  KNXMLReader(const ElementIterator &iterator);
  KNXMLReader(const MixedIterator &iterator);
  ~KNXMLReader();

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
  void initialize(WPXInputStream *input);

private:
  Impl *const m_impl;
  bool m_owner;
};

int getNameId(const KNXMLReader::AttributeIterator &attribute);
int getNamespaceId(const KNXMLReader::AttributeIterator &attribute);
int getValueId(const KNXMLReader::AttributeIterator &attribute);

int getNameId(const KNXMLReader::ElementIterator &element);
int getNamespaceId(const KNXMLReader::ElementIterator &element);
int getTextId(const KNXMLReader::ElementIterator &element);

int getNameId(const KNXMLReader &reader);
int getNamespaceId(const KNXMLReader &reader);

}

#endif // KNXMLREADER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
