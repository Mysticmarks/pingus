// Pingus - A free Lemmings clone
// Copyright (C) 2005-2008 Ingo Ruhnke <grumbel@gmail.com>
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

#include "engine/display/sprite_impl.hpp"

#include <logmich/log.hpp>

#include "engine/display/display.hpp"
#include "engine/display/framebuffer.hpp"
#include "engine/display/sprite_description.hpp"

namespace pingus {

namespace {

FramebufferSurface load_framebuffer_surface(Pathname const& filename, ResourceModifier::Enum modifier)
{
  // FIXME: Implement proper cache
  try
  {
    Surface surface(filename);
    if (modifier != ResourceModifier::ROT0)
    {
      surface = surface.mod(modifier);
    }
    return Display::get_framebuffer()->create_surface(surface);
  }
  catch(std::exception const& err)
  {
    // return a dummy surface for cases where the image file can't be found
    log_error("{}: exception on load: {}", fmt::streamed(filename), err.what());
    Surface surface(Pathname("images/core/misc/404.png", Pathname::DATA_PATH));
    return Display::get_framebuffer()->create_surface(surface);
  }
}

} // namespace

SpriteImpl::SpriteImpl() :
  filename(),
  framebuffer_surface(),
  offset(),
  frame_pos(),
  frame_size(),
  frame_delay(),
  array(),
  loop(),
  loop_last_cycle(),
  finished(),
  frame(),
  tick_count()
{
}

SpriteImpl::SpriteImpl(SpriteDescription const& desc, ResourceModifier::Enum mod) :
  filename(desc.filename),
  framebuffer_surface(),
  offset(),
  frame_pos(),
  frame_size(),
  frame_delay(),
  array(),
  loop(),
  loop_last_cycle(),
  finished(false),
  frame(0),
  tick_count(0)
{
  framebuffer_surface = load_framebuffer_surface(desc.filename, mod);

  frame_pos = desc.frame_pos;

  array = desc.array;

  frame_size = geom::isize((desc.frame_size.width()  == -1) ? framebuffer_surface.get_width()/array.width()   : desc.frame_size.width(),
                    (desc.frame_size.height() == -1) ? framebuffer_surface.get_height()/array.height() : desc.frame_size.height());

  frame_delay  = desc.speed;

  loop = desc.loop;
  loop_last_cycle = false;

  offset = (-geom::anchor_offset(frame_size, desc.origin)).as_vec() - desc.offset.as_vec();

}

SpriteImpl::SpriteImpl(Surface const& surface) :
  filename(),
  framebuffer_surface(Display::get_framebuffer()->create_surface(surface)),
  offset(0,0),
  frame_pos(0,0),
  frame_size(surface.get_width(), surface.get_height()),
  frame_delay(0),
  array(1,1),
  loop(true),
  loop_last_cycle(false),
  finished(false),
  frame(0),
  tick_count(0)
{
}

SpriteImpl::~SpriteImpl()
{
}

void
SpriteImpl::update(float delta)
{
  if (finished || frame_delay == 0)
    return;

  int total_time = frame_delay * (array.width() * array.height());
  tick_count += int(delta * 1000.0f);
  if (tick_count >= total_time)
  {
    if (loop)
    {
      loop_last_cycle = true;
      tick_count = tick_count % total_time;
      frame = tick_count / frame_delay;
    }
    else
    {
      finished = true;
    }
  }
  else
  {
    loop_last_cycle = false;
    frame = tick_count / frame_delay;
  }
}

void
SpriteImpl::render(int x, int y, Framebuffer& fb)
{
  fb.draw_surface(framebuffer_surface,
                  geom::irect(frame_pos + geom::ioffset(frame_size.width()  * (frame%array.width()),
                                                 frame_size.height() * (frame/array.width())),
                       frame_size),
                  geom::ipoint(x - offset.x(), y - offset.y()));
}

void
SpriteImpl::restart()
{
  finished = false;
  loop_last_cycle = false;
  frame = 0;
  tick_count = 0;
}

void
SpriteImpl::finish()
{
  finished = true;
}

} // namespace pingus

/* EOF */
