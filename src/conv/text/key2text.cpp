/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <memory>
#include <stdio.h>
#include <string.h>

#include <librevenge-generators/librevenge-generators.h>
#include <librevenge-stream/librevenge-stream.h>
#include <librevenge/librevenge.h>
#include <libetonyek/libetonyek.h>

#ifndef VERSION
#define VERSION "UNKNOWN VERSION"
#endif

#define TOOL "key2text"

namespace
{

int printUsage()
{
  printf("`" TOOL "' converts Apple Keynote presentations to plain text.\n");
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

int main(int argc, char *argv[]) try
{
  if (argc < 2)
    return printUsage();

  char *file = nullptr;

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
  using libetonyek::EtonyekDocument;

  shared_ptr<librevenge::RVNGInputStream> input;
  if (librevenge::RVNGDirectoryStream::isDirectory(file))
    input.reset(new librevenge::RVNGDirectoryStream(file));
  else
    input.reset(new librevenge::RVNGFileStream(file));

  EtonyekDocument::Type type = EtonyekDocument::TYPE_UNKNOWN;
  const EtonyekDocument::Confidence confidence = EtonyekDocument::isSupported(input.get(), &type);
  if ((EtonyekDocument::CONFIDENCE_NONE == confidence) || (EtonyekDocument::TYPE_KEYNOTE != type))
  {
    fprintf(stderr, "ERROR: Unsupported file format!\n");
    return 1;
  }

  if (EtonyekDocument::CONFIDENCE_SUPPORTED_PART == confidence)
    input.reset(librevenge::RVNGDirectoryStream::createForParent(file));

  librevenge::RVNGStringVector output;
  librevenge::RVNGTextPresentationGenerator painter(output);

  if (!EtonyekDocument::parse(input.get(), &painter))
  {
    fprintf(stderr, "ERROR: Parsing failed!\n");
    return 1;
  }

  if (output.empty())
  {
    fprintf(stderr, "ERROR: No text document generated!\n");
    return 1;
  }

  for (unsigned i = 0; output.size() != i; ++i)
    printf("%s\n", output[i].cstr());

  return 0;
}
catch (...)
{
  fprintf(stderr, "ERROR: uncaught exception!\n");
  return 1;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
