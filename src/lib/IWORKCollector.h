/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKCOLLECTOR_H_INCLUDED
#define IWORKCOLLECTOR_H_INCLUDED

#include <deque>
#include <stack>
#include <string>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "libetonyek_utils.h"
#include "IWORKPath_fwd.h"
#include "IWORKShape.h"
#include "IWORKStyle.h"
#include "IWORKStyleStack.h"
#include "IWORKStylesheet.h"
#include "IWORKTransformation.h"
#include "IWORKTypes.h"
#include "IWORKOutputManager.h"

namespace libetonyek
{

class IWORKDocumentInterface;
class IWORKLanguageManager;
class IWORKPropertyMap;
class IWORKRecorder;
class IWORKTable;
class IWORKText;
struct IWORKSize;

class IWORKCollector
{
private:
  struct Level
  {
    IWORKGeometryPtr_t m_geometry;
    IWORKStylePtr_t m_graphicStyle;
    glm::dmat3 m_trafo;

    Level();
  };

public:
  explicit IWORKCollector(IWORKDocumentInterface *document);
  ~IWORKCollector();

  void setRecorder(const boost::shared_ptr<IWORKRecorder> &recorder);

  // collector functions

  void collectStyle(const IWORKStylePtr_t &style);

  void setGraphicStyle(const IWORKStylePtr_t &style);

  void collectGeometry(const IWORKGeometryPtr_t &geometry);

  void collectBezier(const IWORKPathPtr_t &path);
  void collectImage(const IWORKImagePtr_t &image);
  void collectLine(const IWORKLinePtr_t &line);
  void collectShape();

  void collectBezierPath();
  void collectPolygonPath(const IWORKSize &size, unsigned edges);
  void collectRoundedRectanglePath(const IWORKSize &size, double radius);
  void collectArrowPath(const IWORKSize &size, double headWidth, double stemRelYPos, bool doubleSided);
  void collectStarPath(const IWORKSize &size, unsigned points, double innerRadius);
  void collectConnectionPath(const IWORKSize &size, double middleX, double middleY);
  void collectCalloutPath(const IWORKSize &size, double radius, double tailSize, double tailX, double tailY, bool quoteBubble);

  void collectMedia(const IWORKMediaContentPtr_t &content);

  void collectStylesheet(const IWORKStylesheetPtr_t &stylesheet);

  void collectMetadata(const IWORKMetadata &metadata);

  void collectHeader(const std::string &name);
  void collectFooter(const std::string &name);

  void collectTable(const boost::shared_ptr<IWORKTable> &table);
  void collectText(const boost::shared_ptr<IWORKText> &text);

  void startDocument();
  void endDocument();

  void startGroup();
  void endGroup();

  void startLevel();
  void endLevel();

  void pushStyle();
  void popStyle();

  void pushStylesheet(const IWORKStylesheetPtr_t &stylesheet);
  void popStylesheet();

  IWORKOutputManager &getOutputManager();

public:
  virtual boost::shared_ptr<IWORKTable> createTable(const IWORKTableNameMapPtr_t &tableNameMap) const;
  virtual boost::shared_ptr<IWORKText> createText(const IWORKLanguageManager &langManager, bool discardEmptyContent = false) const;

protected:
  void fillMetadata(librevenge::RVNGPropertyList &props);

  static void writeFill(const IWORKFill &fill, librevenge::RVNGPropertyList &props);

private:
  void pushStyle(const IWORKStylePtr_t &style);
  void resolveStyle(IWORKStyle &style);

  void collectHeaderFooter(const std::string &name, IWORKHeaderFooterMap_t &map);

  void drawMedia(const IWORKMediaPtr_t &media);
  void drawShape(const IWORKShapePtr_t &shape);

  virtual void drawTable() = 0;
  virtual void drawMedia(double x, double y, double w, double h, const std::string &mimetype, const librevenge::RVNGBinaryData &data) = 0;
  virtual void fillShapeProperties(librevenge::RVNGPropertyList &props) = 0;
  virtual void drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox) = 0;

protected:
  IWORKDocumentInterface *m_document;
  boost::shared_ptr<IWORKRecorder> m_recorder;

  std::stack<Level> m_levelStack;
  IWORKStyleStack m_styleStack;
  std::stack<IWORKStylesheetPtr_t> m_stylesheetStack;
  IWORKOutputManager m_outputManager;

  std::deque<IWORKStylePtr_t> m_newStyles;

  boost::shared_ptr<IWORKTable> m_currentTable;
  boost::shared_ptr<IWORKText> m_currentText;

  IWORKHeaderFooterMap_t m_headers;
  IWORKHeaderFooterMap_t m_footers;

private:
  IWORKPathPtr_t m_currentPath;
  IWORKDataPtr_t m_currentData;
  IWORKMediaContentPtr_t m_currentUnfiltered;
  IWORKMediaContentPtr_t m_currentFiltered;
  IWORKMediaContentPtr_t m_currentLeveled;
  IWORKMediaContentPtr_t m_currentContent;

  IWORKMetadata m_metadata;

  int m_groupLevel;
};

} // namespace libetonyek

#endif // IWORKCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
