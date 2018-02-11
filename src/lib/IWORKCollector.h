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
#include <memory>
#include <stack>
#include <string>

#include <boost/optional.hpp>

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
    glm::dmat3 m_previousTrafo;

    Level();
  };

public:
  explicit IWORKCollector(IWORKDocumentInterface *document);
  virtual ~IWORKCollector();

  void setRecorder(const std::shared_ptr<IWORKRecorder> &recorder);

  // collector functions

  void collectStyle(const IWORKStylePtr_t &style);

  void setGraphicStyle(const IWORKStylePtr_t &style);

  void collectGeometry(const IWORKGeometryPtr_t &geometry);
  void setAccumulateTransformTo(bool accumulate);

  void collectBezier(const IWORKPathPtr_t &path);
  void collectLine(const IWORKLinePtr_t &line);
  void collectShape(const boost::optional<int> &order=boost::none, bool locked=false);

  void collectBezierPath();
  void collectPolygonPath(const IWORKSize &size, unsigned edges);
  void collectRoundedRectanglePath(const IWORKSize &size, double radius);
  void collectArrowPath(const IWORKSize &size, double headWidth, double stemRelYPos, bool doubleSided);
  void collectStarPath(const IWORKSize &size, unsigned points, double innerRadius);
  void collectConnectionPath(const IWORKSize &size, const boost::optional<IWORKPosition> &middle);
  void collectCalloutPath(const IWORKSize &size, double radius, double tailSize, double tailX, double tailY, bool quoteBubble);

  void collectImage(const IWORKMediaContentPtr_t &image, const IWORKGeometryPtr_t &cropGeometry=IWORKGeometryPtr_t(),
                    const boost::optional<int> &order=boost::none, bool locked=false);
  void collectMedia(const IWORKMediaContentPtr_t &content, const IWORKGeometryPtr_t &cropGeometry=IWORKGeometryPtr_t(), const boost::optional<int> &order=boost::none);

  void collectStylesheet(const IWORKStylesheetPtr_t &stylesheet);

  void collectMetadata(const IWORKMetadata &metadata);

  void collectHeader(const std::string &name);
  void collectFooter(const std::string &name);

  void collectTable(const std::shared_ptr<IWORKTable> &table);
  void collectText(const std::shared_ptr<IWORKText> &text);

  void startDocument();
  void endDocument();

  void startAttachment();
  void endAttachment();

  void startAttachments();
  void endAttachments();

  void startGroup();
  void endGroup();

  // really add a group in the final file
  void openGroup();
  void closeGroup();

  void startLevel();
  void endLevel();

  void pushStyle();
  void popStyle();

  void pushStylesheet(const IWORKStylesheetPtr_t &stylesheet);
  void popStylesheet();
  IWORKStylesheetPtr_t getStylesheet() const
  {
    if (m_stylesheetStack.empty())
      return IWORKStylesheetPtr_t();
    return m_stylesheetStack.top();
  }

  IWORKOutputManager &getOutputManager();

public:
  virtual std::shared_ptr<IWORKTable> createTable(const IWORKTableNameMapPtr_t &tableNameMap, const IWORKLanguageManager &langManager) const;
  virtual std::shared_ptr<IWORKText> createText(const IWORKLanguageManager &langManager, bool discardEmptyContent = false, bool allowListInsertion=true) const;

protected:
  void fillMetadata(librevenge::RVNGPropertyList &props);

  static void fillWrapProps(const IWORKStylePtr_t style, librevenge::RVNGPropertyList &props,
                            const boost::optional<int> &order);
  static void fillGraphicProps(const IWORKStylePtr_t style, librevenge::RVNGPropertyList &props,
                               bool isSurface=true, bool isFrame=false);
  static void writeFill(const IWORKFill &fill, librevenge::RVNGPropertyList &props);
  virtual void drawShape(const IWORKShapePtr_t &shape);

private:
  void pushStyle(const IWORKStylePtr_t &style);

  void collectHeaderFooter(const std::string &name, IWORKHeaderFooterMap_t &map);

  void drawLine(const IWORKLinePtr_t &line);
  void drawMedia(const IWORKMediaPtr_t &media);
  virtual void drawTable() = 0;
  virtual void drawMedia(double x, double y, const librevenge::RVNGPropertyList &data) = 0;
  virtual void fillShapeProperties(librevenge::RVNGPropertyList &props) = 0;
  virtual bool createFrameStylesForTextBox() const = 0;
  virtual void drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox, const librevenge::RVNGPropertyList &style) = 0;

protected:
  IWORKCollector(const IWORKCollector &);
  IWORKCollector &operator=(const IWORKCollector &);

  IWORKDocumentInterface *m_document;
  std::shared_ptr<IWORKRecorder> m_recorder;

  std::stack<Level> m_levelStack;
  IWORKStyleStack m_styleStack;
  std::stack<IWORKStylesheetPtr_t> m_stylesheetStack;
  IWORKOutputManager m_outputManager;

  std::deque<IWORKStylePtr_t> m_newStyles;

  std::shared_ptr<IWORKTable> m_currentTable;
  std::shared_ptr<IWORKText> m_currentText;

  IWORKHeaderFooterMap_t m_headers;
  IWORKHeaderFooterMap_t m_footers;

  std::stack<IWORKPathPtr_t> m_pathStack;
  IWORKPathPtr_t m_currentPath;

  std::stack<bool> m_attachmentStack;
  bool m_inAttachment;
  bool m_inAttachments;

private:
  IWORKDataPtr_t m_currentData;
  IWORKMediaContentPtr_t m_currentUnfiltered;
  IWORKMediaContentPtr_t m_currentFiltered;
  IWORKMediaContentPtr_t m_currentLeveled;
  IWORKMediaContentPtr_t m_currentContent;

  IWORKMetadata m_metadata;

  bool m_accumulateTransform;
  int m_groupLevel;
};

} // namespace libetonyek

#endif // IWORKCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
