#include "log.hh"

#include <fstream>
#include <iomanip>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/support/date_time.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

#include "util.hh"
#include "ansi_color.hh"

namespace {
  struct null_deleter {
    void operator()(void const *) const {}
  };
}

void coloringFormatter(logging::record_view const& rec,
                       logging::formatting_ostream& strm)
{
  auto severity = rec[logging::trivial::severity];

  // Format the message here...

  typedef logging::formatter formatter;
  formatter f = expr::stream << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S");

  strm << "[" << ansi::bright;

  f(rec, strm);

  strm << " ";

  strm << std::setw(5);

  if(severity)
  {
    // Set the color
    switch (severity.get())
    {
    case logging::trivial::info:
      strm << ansi::green << "info";
      break;
    case logging::trivial::warning:
      strm << ansi::yellow << "warn";
      break;
    case logging::trivial::error:
      strm << ansi::red << "fatal";
      break;
    case logging::trivial::fatal:
      strm << ansi::red << "fatal";
      break;
    case logging::trivial::debug:
      strm << ansi::blue << "debug";
      break;
    default:
      strm << rec[logging::trivial::severity];
      break;
    }
  }

  strm << ansi::reset;

  strm << "] " << rec[expr::smessage];
}

void logInit()
{
  boost::log::add_common_attributes();

  boost::shared_ptr< logging::core > core = logging::core::get();

  if(!debuggingEnabled())
  {
    core->set_filter(logging::trivial::severity >= logging::trivial::info);
  }

  boost::shared_ptr< sinks::text_ostream_backend > backend =
    boost::make_shared< sinks::text_ostream_backend >();

  backend->add_stream(
    boost::shared_ptr< std::ostream >(&std::cerr, null_deleter()));

  typedef sinks::synchronous_sink< sinks::text_ostream_backend > sink_t;
  boost::shared_ptr< sink_t > sink(new sink_t(backend));

  sink->set_formatter(&coloringFormatter);

  core->add_sink(sink);
}
