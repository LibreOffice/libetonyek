# Hacking

## Coding style

### C++ version

We use C++ 98/03. No C++11/14, please.

### Indentation

We indent with spaces. No tabs, please. Try to keep lines less than 120 columns wide. Please run

    make astyle

before committing.

### Naming

Names are in camel case. Names of classes, structs and enums start with a capital letter, names of functions and variables with a lower-case letter. Member variables should have `m_` prefix. Short-hand for variable names is allowed, but don't overdo it (e.g., `len` instead of `length` or `para` instead of `paragraph` is OK).

Names of all non-local classes, enumerations and constants should have a prefix, depending on their use. This is:

* `IWORK`: for symbols used by all (or more than one) parsers;
* `KEY`: symbols specific to Keynote;
* `PAG`: symbols specific to Pages;
* `NUM`: symbols specific to Numbers.

Furthermore, if a symbol is specific for a specific version of a format, the prefix should be followed by the version number, e.g., `KEY2Parser`.

If you want to add a typedef for a smart pointer or a container of something, add `Ptr_t`, `List_t` or `Map_t` suffix to the name.

### Source files

Every class should be declared in a separate header file and defined in a separate source file. The names of these files should be the same as that of the class itself, including the camel case. Exception: structs that only contain data or enums can be grouped together; they can be put into one of the existing `*Type.h` and `*Enum.h` files.

All the sources are in `src/lib`, except for XML parser contexts, which are in `src/lib/contexts`.

### Memory management

Manual memory management is strongly discouraged. Use smart pointers or pointer containers from Boost.

### Data structures

Use data structures from the standard library or Boost, if possible. You may use either the C++ standard strings or our very own UTF-8-compliant `librevenge::RVNGString`. Use of C strings is strongly discouraged.

### Compiler warnings

Configure with ``--enable-werror --enable-weffc``. Do not ignore compiler warnings.

### External code

Do not add additional dependencies without a good reason. If you have to, prefer projects that are already used by other DLP library or LibreOffice. Use any Boost library you like, as long as it is header-only and is available in all supported versions of Boost (some of us still build with Boost 1.47).

Do not include external code unless it has got a clear (and compatible) license. Exception: picking from other DLP libraries is typically not a problem and is even encouraged.

### Fun

Remember, the important thing is to have fun. :-) These rules are a means, not an end. Happy hacking!

## Hacking notes

### Main entities

* *Parser: a parser :-) Passes raw data to collector for further processing.
* *Collector: an output producer; uses the data passed to it from parser.
* IWORKPropertyMap: a type-safe property map. A property map is typically not used directly, except in the parser. Instead, it is wrapped in an IWORKStyle.
* IWORKStyleStack: a hierarchy of currently applied styles. It allows lookup of properties without checking multiple styles explicitly.
* IWORKDocumentInterface: an amalgamation of librevenge document interfaces, to allow common handling of some parts of the file format.

See the documentation for concrete classes for more details.

### Tokenizer

We use gperf to generate lookup maps for XML tokens. These are split by namespace: names in `sf` and `sfa` namespaces are in IWORKToken, `key` in in KEYToken, `ls` in NUMToken and `sl` in PAGToken. To add a new token, add it into the enum in appropriate `*Token.h` and define it in `*Token.gperf`.

### Type-safe property map

To add a new property, declare it in `IWORKProperties.h` and define in `IWORKProperties.cpp`.

### XML Parser

Parsing is done by parser context classes. These handle a specific XML element or a class of elements (e.g., references or styles). The contexts have access to a global state (which is format-specific), but most of the results are passed either to the collector or up to the parent context. In the later case, the output is saved through references to `boost::optional<Foo>` passed in the constructor.

The context classes employ various suffixes for better orientation. `Context` is common for several elements, `Element` is for a single element, `Base` is a base class adding a common functionality, which is not supposed to be used directly.
