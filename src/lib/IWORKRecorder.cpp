/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKRecorder.h"

#include <deque>

#include <boost/variant.hpp>

#include "IWORKCollector.h"
#include "IWORKTableRecorder.h"
#include "IWORKText.h"
#include "IWORKTextRecorder.h"

namespace libetonyek
{

using std::shared_ptr;

namespace
{

struct CollectStyle
{
  explicit CollectStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct SetGraphicStyle
{
  explicit SetGraphicStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct CollectGeometry
{
  explicit CollectGeometry(const IWORKGeometryPtr_t &geometry)
    : m_geometry(geometry)
  {
  }

  const IWORKGeometryPtr_t m_geometry;
};

struct CollectPath
{
  explicit CollectPath(const IWORKPathPtr_t &path)
    : m_path(path)
  {
  }

  const IWORKPathPtr_t m_path;
};

struct CollectImage
{
  CollectImage(const IWORKMediaContentPtr_t &image, const IWORKGeometryPtr_t &cropGeometry, const boost::optional<int> &order, bool locked)
    : m_image(image)
    , m_cropGeometry(cropGeometry)
    , m_order(order)
    , m_locked(locked)
  {
  }

  const IWORKMediaContentPtr_t m_image;
  const IWORKGeometryPtr_t m_cropGeometry;
  const boost::optional<int> m_order;
  const bool m_locked;
};

struct CollectLine
{
  explicit CollectLine(const IWORKLinePtr_t &line)
    : m_line(line)
  {
  }

  const IWORKLinePtr_t m_line;
};

struct CollectShape
{
  CollectShape(const boost::optional<int> &order, const boost::optional<unsigned> &resizeFlags, bool locked)
    : m_order(order)
    , m_resizeFlags(resizeFlags)
    , m_locked(locked)
  {
  }
  const boost::optional<int> m_order;
  const boost::optional<unsigned> m_resizeFlags;
  bool m_locked;
};

struct CollectMedia
{
  CollectMedia(const IWORKMediaContentPtr_t &content, const IWORKGeometryPtr_t &cropGeometry, const boost::optional<int> &order)
    : m_content(content)
    , m_cropGeometry(cropGeometry)
    , m_order(order)
  {
  }

  const IWORKMediaContentPtr_t m_content;
  const IWORKGeometryPtr_t m_cropGeometry;
  const boost::optional<int> m_order;
};

struct CollectStylesheet
{
  explicit CollectStylesheet(const IWORKStylesheetPtr_t &stylesheet)
    : m_stylesheet(stylesheet)
  {
  }

  const IWORKStylesheetPtr_t m_stylesheet;
};

struct CollectTable
{
  explicit CollectTable(const std::shared_ptr<IWORKTable> &table)
    : m_table(table)
  {
  }

  const std::shared_ptr<IWORKTable> m_table;
};

struct CollectText
{
  explicit CollectText(const std::shared_ptr<IWORKText> &text)
    : m_text(text)
  {
  }

  const std::shared_ptr<IWORKText> m_text;
};

enum OperationType { Attachment, Attachments, Group, GroupSymbol, Level };
struct StartOp
{
  explicit StartOp(const OperationType &type)
    : m_type(type)
  {
  }

  OperationType m_type;
};

struct EndOp
{
  explicit EndOp(OperationType type)
    : m_type(type)
  {
  }

  OperationType m_type;
};

struct PushStylesheet
{
  explicit PushStylesheet(const IWORKStylesheetPtr_t &stylesheet)
    : m_stylesheet(stylesheet)
  {
  }

  const IWORKStylesheetPtr_t m_stylesheet;
};

struct PopStylesheet
{
};

typedef boost::variant
< CollectStyle
, SetGraphicStyle
, CollectGeometry
, CollectPath
, CollectImage
, CollectLine
, CollectShape
, CollectMedia
, CollectStylesheet
, CollectTable
, CollectText
, EndOp
, StartOp
, PushStylesheet
, PopStylesheet
>
Element_t;

}

namespace
{

struct Sender : public boost::static_visitor<void>
{
  explicit Sender(IWORKCollector &collector)
    : m_collector(collector)
  {
  }

  void operator()(const CollectStyle &value) const
  {
    m_collector.collectStyle(value.m_style);
  }

  void operator()(const SetGraphicStyle &value) const
  {
    m_collector.setGraphicStyle(value.m_style);
  }

  void operator()(const CollectGeometry &value) const
  {
    m_collector.collectGeometry(value.m_geometry);
  }

  void operator()(const CollectPath &value) const
  {
    m_collector.collectBezier(value.m_path);
  }

  void operator()(const CollectImage &value) const
  {
    m_collector.collectImage(value.m_image, value.m_cropGeometry, value.m_order, value.m_locked);
  }

  void operator()(const CollectLine &value) const
  {
    m_collector.collectLine(value.m_line);
  }

  void operator()(const CollectShape &value) const
  {
    m_collector.collectShape(value.m_order, value.m_resizeFlags, value.m_locked);
  }

  void operator()(const CollectMedia &value) const
  {
    m_collector.collectMedia(value.m_content, value.m_cropGeometry, value.m_order);
  }

  void operator()(const CollectStylesheet &value) const
  {
    m_collector.collectStylesheet(value.m_stylesheet);
  }

  void operator()(const CollectTable &value) const
  {
    const shared_ptr<IWORKTableRecorder> recorder(value.m_table->getRecorder());
    value.m_table->setRecorder(shared_ptr<IWORKTableRecorder>());
    if (bool(recorder))
      recorder->replay(*value.m_table);
    m_collector.collectTable(value.m_table);
  }

  void operator()(const CollectText &value) const
  {
    const shared_ptr<IWORKTextRecorder> recorder(value.m_text->getRecorder());
    value.m_text->setRecorder(shared_ptr<IWORKTextRecorder>());
    if (bool(recorder))
      recorder->replay(*value.m_text);
    m_collector.collectText(value.m_text);
  }

  void operator()(const EndOp &value) const
  {
    switch (value.m_type)
    {
    case OperationType::Attachment:
      m_collector.endAttachment();
      break;
    case OperationType::Attachments:
      m_collector.endAttachments();
      break;
    case OperationType::Group:
      m_collector.endGroup();
      break;
    case OperationType::GroupSymbol:
      m_collector.closeGroup();
      break;
    case OperationType::Level:
      m_collector.endLevel();
      break;
    default:
      ETONYEK_DEBUG_MSG(("Sender::operator(EndOp)[IWORKRecorder.cpp]: unexpected type\n"));
      break;
    }
  }

  void operator()(const StartOp &value) const
  {
    switch (value.m_type)
    {
    case OperationType::Attachment:
      m_collector.startAttachment();
      break;
    case OperationType::Attachments:
      m_collector.startAttachments();
      break;
    case OperationType::Group:
      m_collector.startGroup();
      break;
    case OperationType::GroupSymbol:
      m_collector.openGroup();
      break;
    case OperationType::Level:
      m_collector.startLevel();
      break;
    default:
      ETONYEK_DEBUG_MSG(("Sender::operator(StartOp)[IWORKRecorder.cpp]: unexpected type\n"));
      break;
    }
  }

  void operator()(const PushStylesheet &value) const
  {
    m_collector.pushStylesheet(value.m_stylesheet);
  }

  void operator()(const PopStylesheet &) const
  {
    m_collector.popStylesheet();
  }

private:
  IWORKCollector &m_collector;
};

}

struct IWORKRecorder::Impl
{
  Impl();

  std::deque<Element_t> m_elements;
};

IWORKRecorder::Impl::Impl()
  : m_elements()
{
}

IWORKRecorder::IWORKRecorder()
  : m_impl(new Impl())
{
}

void IWORKRecorder::replay(IWORKCollector &collector) const
{
  Sender sender(collector);
  for (std::deque<Element_t>::const_iterator it = m_impl->m_elements.begin(); it != m_impl->m_elements.end(); ++it)
    boost::apply_visitor(sender, *it);
}

void IWORKRecorder::collectStyle(const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(CollectStyle(style));
}

void IWORKRecorder::setGraphicStyle(const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(SetGraphicStyle(style));
}

void IWORKRecorder::collectGeometry(const IWORKGeometryPtr_t &geometry)
{
  m_impl->m_elements.push_back(CollectGeometry(geometry));
}

void IWORKRecorder::collectPath(const IWORKPathPtr_t &path)
{
  m_impl->m_elements.push_back(CollectPath(path));
}

void IWORKRecorder::collectImage(const IWORKMediaContentPtr_t &image, const IWORKGeometryPtr_t &cropGeometry, const boost::optional<int> &order, bool locked)
{
  m_impl->m_elements.push_back(CollectImage(image, cropGeometry, order, locked));
}

void IWORKRecorder::collectLine(const IWORKLinePtr_t &line)
{
  m_impl->m_elements.push_back(CollectLine(line));
}

void IWORKRecorder::collectShape(const boost::optional<int> &order, const boost::optional<unsigned> &resizeFlags, bool locked)
{
  m_impl->m_elements.push_back(CollectShape(order,resizeFlags,locked));
}

void IWORKRecorder::collectMedia(const IWORKMediaContentPtr_t &content, const IWORKGeometryPtr_t &cropGeometry, const boost::optional<int> &order)
{
  m_impl->m_elements.push_back(CollectMedia(content, cropGeometry, order));
}

void IWORKRecorder::collectStylesheet(const IWORKStylesheetPtr_t &stylesheet)
{
  m_impl->m_elements.push_back(CollectStylesheet(stylesheet));
}

void IWORKRecorder::collectTable(const std::shared_ptr<IWORKTable> &table)
{
  m_impl->m_elements.push_back(CollectTable(table));
}

void IWORKRecorder::collectText(const std::shared_ptr<IWORKText> &text)
{
  m_impl->m_elements.push_back(CollectText(text));
}


void IWORKRecorder::startAttachment()
{
  m_impl->m_elements.push_back(StartOp(OperationType::Attachment));
}

void IWORKRecorder::endAttachment()
{
  m_impl->m_elements.push_back(EndOp(OperationType::Attachment));
}

void IWORKRecorder::startAttachments()
{
  m_impl->m_elements.push_back(StartOp(OperationType::Attachments));
}

void IWORKRecorder::endAttachments()
{
  m_impl->m_elements.push_back(EndOp(OperationType::Attachments));
}

void IWORKRecorder::openGroup()
{
  m_impl->m_elements.push_back(StartOp(OperationType::GroupSymbol));
}

void IWORKRecorder::closeGroup()
{
  m_impl->m_elements.push_back(EndOp(OperationType::GroupSymbol));
}

void IWORKRecorder::startGroup()
{
  m_impl->m_elements.push_back(StartOp(OperationType::Group));
}

void IWORKRecorder::endGroup()
{
  m_impl->m_elements.push_back(EndOp(OperationType::Group));
}

void IWORKRecorder::startLevel()
{
  m_impl->m_elements.push_back(StartOp(OperationType::Level));
}

void IWORKRecorder::endLevel()
{
  m_impl->m_elements.push_back(EndOp(OperationType::Level));
}

void IWORKRecorder::pushStylesheet(const IWORKStylesheetPtr_t &stylesheet)
{
  m_impl->m_elements.push_back(PushStylesheet(stylesheet));
}

void IWORKRecorder::popStylesheet()
{
  m_impl->m_elements.push_back(PopStylesheet());
}

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
