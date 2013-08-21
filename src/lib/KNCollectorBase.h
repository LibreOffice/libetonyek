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

#include <stack>

#include "KNCollector.h"
#include "KNObject.h"

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

  virtual void collectCharacterStyle(const ID_t &id, const KNStylePtr_t &style);
  virtual void collectGraphicStyle(const ID_t &id, const KNStylePtr_t &style);
  virtual void collectHeadlineStyle(const ID_t &id, const KNStylePtr_t &style);
  virtual void collectLayoutStyle(const ID_t &id, const KNStylePtr_t &style);
  virtual void collectParagraphStyle(const ID_t &id, const KNStylePtr_t &style);

  virtual void collectGeometry(const ID_t &id, const KNGeometryPtr_t &geometry);
  virtual void collectGroup(const ID_t &id, const KNGroupPtr_t &group);
  virtual void collectImage(const ID_t &id, const KNImagePtr_t &image);
  virtual void collectLine(const ID_t &id, const KNLinePtr_t &line);
  virtual void collectMedia(const ID_t &id, const KNMediaPtr_t &media);
  virtual void collectPath(const ID_t &id, const KNPathPtr_t &path);

  virtual void collectLayer(const ID_t &id, bool ref);

  virtual void startLayer();
  virtual void endLayer();
  virtual void startGroup();
  virtual void endGroup();

  bool getCollecting() const;
  void setCollecting(bool collecting);

  const KNLayerPtr_t &getLayer() const;

private:
  KNDictionary &m_dict;
  std::stack<KNObjectList_t> m_objectsStack;
  KNGeometryPtr_t m_currentGeometry;
  KNLayerPtr_t m_currentLayer;

  bool m_collecting;
  bool m_layerOpened;
  int m_groupLevel;
};

} // namespace libkeynote

#endif // KNCOLLECTORBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
