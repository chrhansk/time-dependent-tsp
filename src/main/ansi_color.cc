#include "ansi_color.hh"

#include <unistd.h>   // For isatty

namespace ansi
{
  const bool can_colorize(isatty(STDOUT_FILENO));
}
