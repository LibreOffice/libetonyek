/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <string.h>

#include <librevenge-generators/librevenge-generators.h>
#include <librevenge-stream/librevenge-stream.h>
#include <librevenge/librevenge.h>
#include <libetonyek/libetonyek.h>

#include "KEYDirectoryStream.h"

namespace
{

int printUsage()
{
  printf("Usage: key2text [OPTION] <KeyNote Document> | <KeyNote Directory>\n");
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
  namespace fs = boost::filesystem;

  fs::path path(file);
  shared_ptr<librevenge::RVNGInputStream> input;
  if (is_directory(path))
    input.reset(new conv::KEYDirectoryStream(path));
  else
    input.reset(new librevenge::RVNGFileStream(file));

  libetonyek::KEYDocumentType type = libetonyek::KEY_DOCUMENT_TYPE_UNKNOWN;
  if (!libetonyek::KEYDocument::isSupported(input.get(), &type))
  {
    fprintf(stderr, "ERROR: Unsupported file format!\n");
    return 1;
  }

  if (libetonyek::KEY_DOCUMENT_TYPE_APXL_FILE == type)
  {
    path.remove_filename();
    input.reset(new conv::KEYDirectoryStream(path));
  }

  librevenge::RVNGStringVector output;
  librevenge::RVNGTextPresentationGenerator painter(output);

  if (!libetonyek::KEYDocument::parse(input.get(), &painter))
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
