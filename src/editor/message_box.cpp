// Pingus - A free Lemmings clone
// Copyright (C) 1998-2011 Ingo Ruhnke <grumbel@gmail.com>
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

#include "editor/message_box.hpp"

#include <functional>

#include "editor/gui_style.hpp"
#include "pingus/gettext.h"
#include "pingus/fonts.hpp"

namespace pingus::editor {

MessageBox::MessageBox(Rect const& rect_) :
  GroupComponent(rect_),
  m_ok_button(),
  m_cancel_button(),
  m_title(),
  m_text(),
  on_ok()
{
  m_cancel_button = create<Button>(Rect(geom::ipoint(rect.width() - 4 - 210, rect.height() - 4 - 30),
                                        Size(100, 30)), _("Cancel"));
  m_ok_button = create<Button>(Rect(geom::ipoint(rect.width() - 4 - 100, rect.height() - 4 - 30),
                                    Size(100, 30)), _("Ok"));

  m_ok_button->on_click.connect(std::bind(&MessageBox::on_ok_button, this));
  m_cancel_button->on_click.connect(std::bind(&MessageBox::on_cancel_button, this));
}

void
MessageBox::set_title(std::string const& text)
{
  m_title = text;
}

void
MessageBox::set_text(std::string const& text)
{
  m_text = text;
}

void
MessageBox::set_ok_text(std::string const& text)
{
  m_ok_button->set_text(text);
}

void
MessageBox::draw_background(DrawingContext& gc)
{
  // Window border and title
  GUIStyle::draw_raised_box(gc, Rect(0,0,rect.width(), rect.height()));
  gc.draw_fillrect(Rect(4,4,rect.width()-4, 30), Color(77,130,180));
  gc.print_center(pingus::fonts::pingus_small, Vector2i(rect.width()/2, 2),
                  m_title);

  // main text
  gc.print_center(pingus::fonts::verdana11, Vector2i(rect.width()/2, 42),
                  m_text);
}

void
MessageBox::update_layout()
{
  gui::GroupComponent::update_layout();

  m_cancel_button->set_rect(Rect(geom::ipoint(rect.width() - 4 - 210, rect.height() - 4 - 30),
                                 Size(100, 30)));
  m_ok_button->set_rect(Rect(geom::ipoint(rect.width() - 4 - 100, rect.height() - 4 - 30),
                             Size(100, 30)));
}

void
MessageBox::on_ok_button()
{
  hide();
  on_ok();
}

void
MessageBox::on_cancel_button()
{
  hide();
}

} // namespace pingus::editor

/* EOF */
