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
  CollectStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct SetGraphicStyle
{
  SetGraphicStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct CollectGeometry
{
  CollectGeometry(const IWORKGeometryPtr_t &geometry)
    : m_geometry(geometry)
  {
  }

  const IWORKGeometryPtr_t m_geometry;
};

struct CollectPath
{
  CollectPath(const IWORKPathPtr_t &path)
    : m_path(path)
  {
  }

  const IWORKPathPtr_t m_path;
};

struct CollectImage
{
  CollectImage(const IWORKMediaContentPtr_t &image, const IWORKGeometryPtr_t &cropGeometry, bool locked)
    : m_image(image)
    , m_cropGeometry(cropGeometry)
    , m_locked(locked)
  {
  }

  const IWORKMediaContentPtr_t m_image;
  const IWORKGeometryPtr_t m_cropGeometry;
  bool m_locked;
};

struct CollectLine
{
  CollectLine(const IWORKLinePtr_t &line)
    : m_line(line)
  {
  }

  const IWORKLinePtr_t m_line;
};

struct CollectShape
{
  CollectShape(bool locked)
    : m_locked(locked)
  {
  }
  bool m_locked;
};

struct CollectMedia
{
  CollectMedia(const IWORKMediaContentPtr_t &content, const IWORKGeometryPtr_t &cropGeometry)
    : m_content(content)
    , m_cropGeometry(cropGeometry)
  {
  }

  const IWORKMediaContentPtr_t m_content;
  const IWORKGeometryPtr_t m_cropGeometry;
};

struct CollectStylesheet
{
  CollectStylesheet(const IWORKStylesheetPtr_t &stylesheet)
    : m_stylesheet(stylesheet)
  {
  }

  const IWORKStylesheetPtr_t m_stylesheet;
};

struct CollectTable
{
  CollectTable(const std::shared_ptr<IWORKTable> &table)
    : m_table(table)
  {
  }

  const std::shared_ptr<IWORKTable> m_table;
};

struct CollectText
{
  CollectText(const std::shared_ptr<IWORKText> &text)
    : m_text(text)
  {
  }

  const std::shared_ptr<IWORKText> m_text;
};

struct StartGroup
{
};

struct EndGroup
{
};

struct StartLevel
{
};

struct EndLevel
{
};

struct PushStylesheet
{
  PushStylesheet(const IWORKStylesheetPtr_t &stylesheet)
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
, StartGroup
, EndGroup
, StartLevel
, EndLevel
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
    m_collector.collectImage(value.m_image, value.m_cropGeometry, value.m_locked);
  }

  void operator()(const CollectLine &value) const
  {
    m_collector.collectLine(value.m_line);
  }

  void operator()(const CollectShape &value) const
  {
    m_collector.collectShape(value.m_locked);
  }

  void operator()(const CollectMedia &value) const
  {
    m_collector.collectMedia(value.m_content, value.m_cropGeometry);
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

  void operator()(const StartGroup &) const
  {
    m_collector.startGroup();
  }

  void operator()(const EndGroup &) const
  {
    m_collector.endGroup();
  }

  void operator()(const StartLevel &) const
  {
    m_collector.startLevel();
  }

  void operator()(const EndLevel &) const
  {
    m_collector.endLevel();
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

void IWORKRecorder::collectImage(const IWORKMediaContentPtr_t &image, const IWORKGeometryPtr_t &cropGeometry, bool locked)
{
  m_impl->m_elements.push_back(CollectImage(image, cropGeometry, locked));
}

void IWORKRecorder::collectLine(const IWORKLinePtr_t &line)
{
  m_impl->m_elements.push_back(CollectLine(line));
}

void IWORKRecorder::collectShape(bool locked)
{
  m_impl->m_elements.push_back(CollectShape(locked));
}

void IWORKRecorder::collectMedia(const IWORKMediaContentPtr_t &content, const IWORKGeometryPtr_t &cropGeometry)
{
  m_impl->m_elements.push_back(CollectMedia(content, cropGeometry));
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

void IWORKRecorder::startGroup()
{
  m_impl->m_elements.push_back(StartGroup());
}

void IWORKRecorder::endGroup()
{
  m_impl->m_elements.push_back(EndGroup());
}

void IWORKRecorder::startLevel()
{
  m_impl->m_elements.push_back(StartLevel());
}

void IWORKRecorder::endLevel()
{
  m_impl->m_elements.push_back(EndLevel());
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
