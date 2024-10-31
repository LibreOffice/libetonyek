# libetonyek

`libetonyek` is a library and a set of tools for reading and converting [Apple iWork documents]((https://en.wikipedia.org/wiki/IWork)) ([Keynote](https://en.wikipedia.org/wiki/Keynote_(presentation_software)), [Pages](https://en.wikipedia.org/wiki/Pages_(word_processor)) and [Numbers](https://en.wikipedia.org/wiki/Numbers_(spreadsheet))). Supported versions are Keynote 2-6, Pages 1-4 and Numbers 1-2.  

See [FEATURES.md](FEATURES.md) for more details about what is currently supported.

## Documentation

[Keynote](https://en.wikipedia.org/wiki/Keynote_(presentation_software)) documentation is, sadly, scarce. There are two official Apple documents: The first one is [Technical Note 2073 (TN2073)](https://web.archive.org/web/20031205125426/http://developer.apple.com/technotes/tn2002/tn2073.html), describing basic parts of the Keynote 1 format. The second is [iWork Programming Guide](https://blog.zamzar.com/wp-content/uploads/2017/09/iwork2-0_xml.pdf), containing an overview of the Keynote 2 structure. Unfortunately it mostly documents things that do not need to be documented: I mean, anyone can guess what elements like slide, master-slide, stylesheet or layer mean, right? But how exactly does style inheritance work?

Which elements can be saved as a reference? What do all these 1-3 letter elements and attributes in table model mean?

These and a lot of other questions remain unanswered. And that is just the first incarnation of the version 2 format; nobody knows what has changed since then.

Based on the previous paragraph, it should be clear that most of the code in this library is based on guesses, sample presentations created in `Keynote '09` and "real-life" presentations collected on the Internet.

`libetonyek` requires `boost`, `glm`, `liblangtag`, `librevenge`, `libxml2`, `mdds`
and `zlib` to build.

The information how to contribute to project is available [here](HACKING.md).

## License

The library is available under [MPL 2.0+](https://www.mozilla.org/en-US/MPL/2.0/).
