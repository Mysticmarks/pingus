// Pingus - A free Lemmings clone
// Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "engine/display/font_description.hpp"

#include <stdexcept>

#include <logmich/log.hpp>
#include <prio/reader.hpp>

#include "util/reader.hpp"

namespace pingus {

GlyphDescription::GlyphDescription() :
  image(0),
  unicode(0),
  offset(),
  advance(0),
  rect()
{
}

GlyphDescription::GlyphDescription(prio::ReaderMapping const& reader) :
  image(0),
  unicode(0),
  offset(),
  advance(0),
  rect()
{
  int lazy = 0; // FIXME: implement read_uint32
  reader.read("unicode", lazy);
  unicode = static_cast<uint32_t>(lazy);
  reader.read("offset", offset);
  reader.read("advance", advance);
  reader.read("rect", rect);
}

FontDescription::FontDescription(Pathname const& pathname_) :
  pathname(pathname_),
  char_spacing(),
  vertical_spacing(),
  size(),
  images()
{
  char_spacing     = 1.0f;
  vertical_spacing = 1.0f;

  auto doc = prio::ReaderDocument::from_file(pathname.get_sys_path());

  if (doc.get_root().get_name() != "pingus-font")
  {
    throw std::runtime_error(fmt::format("{}: not a pingus-font file", fmt::streamed(pathname)));
  }
  else
  {
    prio::ReaderMapping reader = doc.get_root().get_mapping();

    reader.read("char-spacing", char_spacing);
    reader.read("vertical-spacing", vertical_spacing);
    reader.read("size", size);

    prio::ReaderCollection images_reader;
    if (reader.read("images", images_reader))
    {
      auto images_lst = images_reader.get_objects();

      for(auto i = images_lst.begin(); i != images_lst.end(); ++i)
      {
        prio::ReaderMapping mapping = i->get_mapping();

        GlyphImageDescription image_desc;
        mapping.read("filename", image_desc.pathname);

        prio::ReaderCollection glyph_collection;
        if (mapping.read("glyphs", glyph_collection))
        {
          std::vector<prio::ReaderObject> glyph_reader = glyph_collection.get_objects();
          for(auto j = glyph_reader.begin(); j != glyph_reader.end(); ++j)
          {
            if (j->get_name() == "glyph")
            {
              image_desc.glyphs.push_back(GlyphDescription(j->get_mapping()));
            }
            else
            {
              log_warn("unknown tag: {}", j->get_name());
            }
          }
        }
        images.push_back(image_desc);
      }
    }
  }
}

} // namespace pingus

/* EOF */
