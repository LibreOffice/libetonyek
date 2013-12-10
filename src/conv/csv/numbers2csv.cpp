/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/shared_ptr.hpp>

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>

#include <librevenge-generators/librevenge-generators.h>
#include <librevenge-stream/librevenge-stream.h>
#include <librevenge/librevenge.h>
#include <libetonyek/libetonyek.h>

namespace
{

int printUsage()
{
  printf("Usage: numbers2csv [OPTION] <Numbers Document> | <Numbers Directory>\n");
  printf("\n");
  printf("Options:\n");
  printf("--help                Shows this help message\n");
  return -1;
}

} // anonymous namespace

int main(int argc, char *argv[]) try
{
  if (argc < 2)
    return printUsage();

  char *file = 0;

  for (int i = 1; i < argc; i++)
  {
    if (!file && strncmp(argv[i], "--", 2))
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

  libetonyek::NUMBERSDocumentType type = libetonyek::NUMBERS_DOCUMENT_TYPE_UNKNOWN;
  if (!libetonyek::NUMBERSDocument::isSupported(input.get(), &type))
  {
    std::cerr << "ERROR: Unsupported file format!" << std::endl;
    return 1;
  }

  if (libetonyek::NUMBERS_DOCUMENT_TYPE_MAIN_FILE == type)
    input.reset(librevenge::RVNGDirectoryStream::createForParent(file));

  librevenge::RVNGStringVector output;
  librevenge::RVNGCSVSpreadsheetGenerator generator(output);
  if (!libetonyek::NUMBERSDocument::parse(input.get(), &generator))
  {
    std::cerr << "ERROR: CSV Generation failed!" << std::endl;
    return 1;
  }

  if (output.empty())
  {
    std::cerr << "ERROR: No SVG document generated!" << std::endl;
    return 1;
  }

  for (unsigned k = 0; k<output.size(); ++k)
    std::cout << output[k].cstr() << std::endl;

  return 0;
}
catch (...)
{
  fprintf(stderr, "ERROR: uncaught exception!\n");
  return 1;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
