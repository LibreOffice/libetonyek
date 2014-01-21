/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <string.h>

#include <boost/shared_ptr.hpp>

#include <librevenge/librevenge.h>
#include <librevenge-generators/librevenge-generators.h>
#include <librevenge-stream/librevenge-stream.h>

#include <libetonyek/libetonyek.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef VERSION
#define VERSION "UNKNOWN VERSION"
#endif

namespace
{

int printUsage()
{
  printf("Usage: pages2raw [OPTION] <Pages Document> | <Pages Directory>\n");
  printf("\n");
  printf("Options:\n");
  printf("--callgraph           Display the call graph nesting level\n");
  printf("--help                Shows this help message\n");
  printf("--version             Output pages2raw version \n");
  return -1;
}

int printVersion()
{
  printf("pages2raw %s\n", VERSION);
  return 0;
}

} // anonymous namespace

using libetonyek::EtonyekDocument;

int main(int argc, char *argv[])
{
  bool printIndentLevel = false;
  char *file = 0;

  if (argc < 2)
    return printUsage();

  for (int i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "--callgraph"))
      printIndentLevel = true;
    else if (!strcmp(argv[i], "--version"))
      return printVersion();
    else if (!file && strncmp(argv[i], "--", 2))
      file = argv[i];
    else
      return printUsage();
  }

  if (!file)
    return printUsage();

  using boost::shared_ptr;

  shared_ptr<librevenge::RVNGInputStream> input;
  if (librevenge::RVNGDirectoryStream::isDirectory(file))
    input.reset(new librevenge::RVNGDirectoryStream(file));
  else
    input.reset(new librevenge::RVNGFileStream(file));

  EtonyekDocument::Type type = EtonyekDocument::TYPE_UNKNOWN;
  const EtonyekDocument::Confidence confidence = EtonyekDocument::isSupported(input.get(), &type);
  if ((EtonyekDocument::CONFIDENCE_NONE == confidence) || (EtonyekDocument::TYPE_PAGES != type))
  {
    fprintf(stderr, "ERROR: Unsupported file format!\n");
    return 1;
  }

  if (EtonyekDocument::CONFIDENCE_SUPPORTED_PART == confidence)
    input.reset(librevenge::RVNGDirectoryStream::createForParent(file));

  librevenge::RVNGRawTextGenerator documentGenerator(printIndentLevel);

  return EtonyekDocument::parse(input.get(), &documentGenerator) ? 0 : 1;
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
