/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWAObjectIndex.h"

#include <cassert>

#include "IWAMessage.h"
#include "IWASnappyStream.h"

namespace libetonyek
{

using boost::optional;

using std::deque;
using std::make_pair;
using std::make_shared;
using std::string;

IWAObjectIndex::ObjectRecord::ObjectRecord()
  : m_stream()
  , m_type(0)
  , m_headerRange(0, 0)
  , m_dataRange(0, 0)
{
}

IWAObjectIndex::ObjectRecord::ObjectRecord(const RVNGInputStreamPtr_t &stream, const unsigned type,
                                           const long pos, const unsigned long headerLen, const unsigned long dataLen)
  : m_stream(stream)
  , m_type(type)
  , m_headerRange(pos, pos + long(headerLen))
  , m_dataRange(m_headerRange.second, m_headerRange.second + long(dataLen))
{
}

IWAObjectIndex::IWAObjectIndex(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package)
  : m_fragments(fragments)
  , m_package(package)
  , m_unparsedFragments()
  , m_fragmentObjectMap()
  , m_fileMap()
{
}

void IWAObjectIndex::parse()
{
  m_unparsedFragments[2] = "Index/Metadata.iwa";
  m_fragmentObjectMap[2] = make_pair(2, ObjectRecord());
  scanFragment(2);
  const auto indexIt = m_fragmentObjectMap.find(2);
  if (indexIt == m_fragmentObjectMap.end() || !indexIt->second.second.m_stream)
  {
    // TODO: scan all fragment files
    ETONYEK_DEBUG_MSG(("IWAObjectIndex::parseObjectIndex: object index is broken, nothing will be parsed\n"));
  }
  else
  {
    const ObjectRecord &rec = indexIt->second.second;
    const IWAMessage objectIndex(rec.m_stream, rec.m_dataRange.first, rec.m_dataRange.second);
    const deque<IWAMessage> &fragments = objectIndex.message(3).repeated();
    for (const auto &fragment : fragments)
    {
      if (fragment.uint32(1) && (fragment.string(2) || fragment.string(3)))
      {
        const unsigned pathIdx = fragment.string(3) ? 3 : 2;
        m_unparsedFragments[fragment.uint32(1).get()] = "Index/" + fragment.string(pathIdx).get() + ".iwa";
        m_fragmentObjectMap[fragment.uint32(1).get()] = make_pair(fragment.uint32(1).get(), ObjectRecord());
      }
      const deque<IWAMessage> &refs = fragment.message(6).repeated();
      for (const auto &ref : refs)
      {
        if (ref.uint32(1) && ref.uint32(2))
          m_fragmentObjectMap[ref.uint32(2).get()] = make_pair(ref.uint32(1).get(), ObjectRecord());
      }
    }
    const deque<IWAMessage> &files = objectIndex.message(4).repeated();
    for (const auto &file : files)
    {
      if (file.uint32(1) && m_package)
      {
        const string virtualPath(file.string(3) ? ("Data/" + get(file.string(3))) : "");
        const string internalPath(file.string(4) ? ("Data/" + get(file.string(4))) : "");
        string path;
        if (!internalPath.empty() && m_package->existsSubStream(internalPath.c_str()))
          path = internalPath;
        else if (!virtualPath.empty() && m_package->existsSubStream(virtualPath.c_str()))
          path = virtualPath;
        if (!path.empty())
          m_fileMap[file.uint32(1).get()] = make_pair(path, RVNGInputStreamPtr_t());
      }
    }
  }
}

void IWAObjectIndex::queryObject(const unsigned id, unsigned &type, boost::optional<IWAMessage> &msg) const
{
  const auto recIt = m_fragmentObjectMap.find(id);
  if (recIt == m_fragmentObjectMap.end())
  {
    ETONYEK_DEBUG_MSG(("IWAObjectIndex::queryObject: object %u not found\n", id));
    return;
  }
  if (!recIt->second.second.m_stream)
    const_cast<IWAObjectIndex *>(this)->scanFragment(recIt->second.first);
  if (recIt->second.second.m_stream)
  {
    const ObjectRecord &objRecord = recIt->second.second;
    msg = IWAMessage(objRecord.m_stream, objRecord.m_dataRange.first, objRecord.m_dataRange.second);
    type = objRecord.m_type;
  }
}

const RVNGInputStreamPtr_t IWAObjectIndex::queryFile(const unsigned id) const
{
  const auto it = m_fileMap.find(id);

  if (it == m_fileMap.end())
  {
    ETONYEK_DEBUG_MSG(("IWAObjectIndex::queryFile: file %u not found\n", id));
    return RVNGInputStreamPtr_t();
  }

  if (!it->second.second && m_package)
  {
    assert(m_package->existsSubStream(it->second.first.c_str())); // we already checked for its presence
    it->second.second.reset(m_package->getSubStreamByName(it->second.first.c_str()));
  }

  return it->second.second;
}

void IWAObjectIndex::scanFragment(const unsigned id)
{
  // scan the fragment file
  const auto fragmentIt = m_unparsedFragments.find(id);
  if (fragmentIt != m_unparsedFragments.end())
  {
    const RVNGInputStreamPtr_t stream(m_fragments->getSubStreamByName(fragmentIt->second.c_str()));
    if (stream)
    {
      const auto fragment = make_shared<IWASnappyStream>(stream);
      scanFragment(fragmentIt->first, fragment);
    }
    else
    {
      ETONYEK_DEBUG_MSG(("IWAObjectIndex::scanFragment: file %s does not exist\n", fragmentIt->second.first.c_str()));
    }
    m_unparsedFragments.erase(fragmentIt);
  }
}

void IWAObjectIndex::scanFragment(const unsigned id, const RVNGInputStreamPtr_t &stream)
{
  try
  {
    while (!stream->isEnd())
    {
      // scan a single object
      const uint64_t headerLen = readUVar(stream);
      const long start = stream->tell();
      const IWAMessage header(stream, headerLen);
      if (!header.message(2) || !header.message(2).uint64(3))
        break;
      const uint64_t dataLen = header.message(2).uint64(3).get();
      if (header.uint32(1))
      {
        const optional<unsigned> type = header.message(2).uint32(1).optional();
        const ObjectRecord rec(stream, get_optional_value_or(type, 0), start, long(headerLen), long(dataLen));
        m_fragmentObjectMap[header.uint32(1).get()] = make_pair(id, rec);
      }
      if (stream->seek(start + long(headerLen) + long(dataLen), librevenge::RVNG_SEEK_SET) != 0)
        break;
    }
  }
  catch (...)
  {
    // just read as much as possible
  }

  // remove all objects from the fragment that have not been found
  auto it = m_fragmentObjectMap.begin();
  while (it != m_fragmentObjectMap.end())
  {
    const auto curIt = it;
    ++it;
    if ((curIt->second.first == id) && !curIt->second.second.m_stream)
    {
      ETONYEK_DEBUG_MSG(("IWAObjectIndex::scanFragment: object with ID %u was not found\n", curIt->first));
      m_fragmentObjectMap.erase(curIt);
    }
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
