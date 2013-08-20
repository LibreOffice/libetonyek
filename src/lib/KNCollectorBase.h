/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNCOLLECTORBASE_H_INCLUDED
#define KNCOLLECTORBASE_H_INCLUDED

#include "KNCollector.h"

namespace libkeynote
{

struct KNDictionary;

/** Implementation of common parts of styles and content collector.
  *
  * It collects styles and drawable objects.
  */
class KNCollectorBase : public KNCollector
{
public:
  explicit KNCollectorBase(KNDictionary &dict);
  ~KNCollectorBase();

  // collector functions

  virtual void collectCharacterStyle(const ID_t &id, const KNStyle &style);
  virtual void collectGraphicStyle(const ID_t &id, const KNStyle &style);
  virtual void collectHeadlineStyle(const ID_t &id, const KNStyle &style);
  virtual void collectLayoutStyle(const ID_t &id, const KNStyle &style);
  virtual void collectParagraphStyle(const ID_t &id, const KNStyle &style);

  virtual void collectGeometry(const ID_t &id, const KNGeometry &geometry);
  virtual void collectGroup(const ID_t &id, const KNGroup &group);
  virtual void collectImage(const ID_t &id, const KNImage &image);
  virtual void collectLine(const ID_t &id, const KNLine &line);
  virtual void collectMedia(const ID_t &id, const KNMedia &media);
  virtual void collectPath(const ID_t &id, const KNPath &path);

  virtual void collectLayer(const ID_t &id, bool ref);

  bool getCollecting() const;
  void setCollecting(bool collecting);

  void fillLayer(KNLayer &layer);

private:
  KNDictionary &m_dict;

  bool m_collecting;
};

} // namespace libkeynote

#endif // KNCOLLECTORBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
