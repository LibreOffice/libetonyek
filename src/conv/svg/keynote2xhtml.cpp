/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <libwpd-stream/libwpd-stream.h>
#include <libwpd/libwpd.h>
#include <libkeynote/libkeynote.h>

#include "KNDirectoryStream.h"

namespace
{

int printUsage()
{
  printf("Usage: keynote2xhtml [OPTION] <KeyNote Document> | <KeyNote Directory>\n");
  printf("\n");
  printf("Options:\n");
  printf("--help                Shows this help message\n");
  return -1;
}

} // anonymous namespace

int main(int argc, char *argv[])
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
  shared_ptr<WPXInputStream> input;
  if (is_directory(path))
    input.reset(new conv::KNDirectoryStream(path));
  else
    input.reset(new WPXFileStream(file));

  libkeynote::KNDocumentType type = libkeynote::KN_DOCUMENT_TYPE_UNKNOWN;
  if (!libkeynote::KNDocument::isSupported(input.get(), &type))
  {
    std::cerr << "ERROR: Unsupported file format!" << std::endl;
    return 1;
  }

  if (libkeynote::KN_DOCUMENT_TYPE_APXL_FILE == type)
  {
    path.remove_filename();
    input.reset(new conv::KNDirectoryStream(path));
  }

  libkeynote::KNStringVector output;
  if (!libkeynote::KNDocument::generateSVG(input.get(), output))
  {
    std::cerr << "ERROR: SVG Generation failed!" << std::endl;
    return 1;
  }

  if (output.empty())
  {
    std::cerr << "ERROR: No SVG document generated!" << std::endl;
    return 1;
  }

  std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  std::cout << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" << std::endl;
  std::cout << "<html xmlns=\"http://www.w3.org/1999/xhtml\" xmlns:svg=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << std::endl;
  std::cout << "<body>" << std::endl;
  std::cout << "<?import namespace=\"svg\" urn=\"http://www.w3.org/2000/svg\"?>" << std::endl;

  for (unsigned k = 0; k<output.size(); ++k)
  {
    if (k>0)
      std::cout << "<hr/>\n";

    std::cout << "<!-- \n";
    std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    std::cout << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"";
    std::cout << " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    std::cout << " -->\n";

    std::cout << output[k].cstr() << std::endl;
  }

  std::cout << "</body>" << std::endl;
  std::cout << "</html>" << std::endl;

  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
