// Pingus - A free Lemmings clone
// Copyright (C) 2007 Ingo Ruhnke <grumbel@gmail.com>
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

#include "pingus/screens/font_test_screen.hpp"

#include <logmich/log.hpp>
#include <strut/to_string.hpp>

#include "engine/display/display.hpp"
#include "engine/display/drawing_context.hpp"
#include "engine/display/font_description.hpp"
#include "engine/input/event.hpp"
#include "pingus/event_name.hpp"
#include "pingus/fonts.hpp"

namespace pingus {

FontTestScreen::FontTestScreen(Pathname const& fontfile) :
  Screen(Display::get_size()),
  font(),
  reference(),
  scroll(),
  dark(true)
{
  log_info("### Loading font file: {}", fmt::streamed(fontfile));
  font = Font(FontDescription(fontfile));
  reference = Font(FontDescription(Pathname("images/fonts/reference-iso-8859-1.font", Pathname::DATA_PATH)));
}

void
FontTestScreen::draw(DrawingContext& gc)
{
  int checker = 40;
  for(int y = 0; y < gc.get_height()/checker; y += 1)
    for(int x = 0; x < gc.get_width()/checker; x += 1)
    {
      if ((x+y) % 2 != 0)
        gc.draw_fillrect(geom::irect(x*checker, y*checker,
                              x*checker + checker, y*checker + checker),
                         dark ? Color(50, 50, 50) : Color(255, 255, 255));
      else
        gc.draw_fillrect(geom::irect(x*checker, y*checker,
                              x*checker + checker, y*checker + checker),
                         dark ? Color(0, 0, 0) : Color(200,200,200));
    }

  gc.print_left(pingus::fonts::chalk_large, geom::ipoint(10, 10), "Pingus - Font Test");

  gc.push_modelview();
  gc.translate(scroll.x(), scroll.y());

  for(uint32_t i = 0; i < 256; ++i)
  {
    int x = 64 + (static_cast<int>(i) % 20)*(font.get_height() + 24);
    int y = 64 + (static_cast<int>(i) / 20)*(font.get_height() + reference.get_height() * 3);

    // print the actual character
    gc.print_left(reference,
                  geom::ipoint(x,
                           y - reference.get_height()),
                  std::string(1, char(i)));
    // print the index number
    gc.print_left(reference,
                  geom::ipoint(x,
                           y + font.get_height()),
                  strut::to_string(i));

    if (font.get_width(i) > 0.0f)
    {
      gc.draw_rect(geom::irect(x, y,
                        x+font.get_height(),
                        y+font.get_height()),
                   Color(0,0,255));
      gc.draw_rect(geom::irect(x, y,
                        x + static_cast<int>(font.get_width(i)),
                        y + font.get_height()),
                   Color(255,0,255));
      gc.print_left(font, geom::ipoint(x, y),
                    std::string(1, static_cast<char>(i)));
    }
    else
    {
      gc.draw_rect(geom::irect(x, y,
                        x+font.get_height(),
                        y+font.get_height()),
                   Color(255,0,0));
    }
  }
  gc.pop_modelview();
}

void
FontTestScreen::update_input(pingus::input::Event const& event)
{
  switch (event.type)
  {
    case pingus::input::BUTTON_EVENT_TYPE:
      if (event.button.state == pingus::input::BUTTON_PRESSED &&
          event.button.name == PRIMARY_BUTTON)
        dark = !dark;
      break;

    case pingus::input::SCROLLER_EVENT_TYPE:
      scroll += geom::ioffset(static_cast<int>(event.scroll.x_delta),
                              static_cast<int>(event.scroll.y_delta));
      break;

    default:
      break;
  }
}

} // namespace pingus

/* EOF */
