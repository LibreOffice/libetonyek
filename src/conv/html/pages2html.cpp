/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <memory>
#include <stdio.h>
#include <string.h>

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

#define TOOL "pages2html"

namespace
{

int printUsage()
{
  printf("`" TOOL "' converts Apple Pages documents to HTML.\n");
  printf("\n");
  printf("Usage: " TOOL " [OPTION] INPUT\n");
  printf("\n");
  printf("Options:\n");
  printf("\t--help                show this help message\n");
  printf("\t--version             show version information\n");
  printf("\n");
  printf("Report bugs to <https://bugs.documentfoundation.org/>.\n");
  return -1;
}

int printVersion()
{
  printf(TOOL " " VERSION "\n");
  return 0;
}

} // anonymous namespace

using libetonyek::EtonyekDocument;

int main(int argc, char *argv[])
{
  if (argc < 2)
    return printUsage();

  char *file = 0;

  for (int i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "--version"))
      return printVersion();
    else if (!file && strncmp(argv[i], "--", 2))
      file = argv[i];
    else
      return printUsage();
  }

  if (!file)
    return printUsage();

  using std::shared_ptr;

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

  librevenge::RVNGString output;
  librevenge::RVNGHTMLTextGenerator documentGenerator(output);

  if (!EtonyekDocument::parse(input.get(), &documentGenerator))
  {
    fprintf(stderr, "ERROR: Parsing failed!\n");
    return 1;
  }

  if (output.empty())
  {
    fprintf(stderr, "ERROR: No output was produced!\n");
    return 1;
  }

  printf("%s", output.cstr());

  return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
