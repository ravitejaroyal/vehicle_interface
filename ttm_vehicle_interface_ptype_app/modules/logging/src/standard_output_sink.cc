


#include "logging/standard_output_sink.h"
#include "logging/additional_logging_levels.h"

#include <iostream>

using namespace logging;

static const char BLACK_TEXT_SEQ[] = "\033[30m";
static const char RED_TEXT_SEQ[] = "\033[31m";
static const char GREEN_TEXT_SEQ[] = "\033[32m";
static const char YELLOW_TEXT_SEQ[] = "\033[33m"; 
static const char WHITE_BACKG_SEQ[] = "\033[47m"; 
static const char RESET_SEQ[] = "\033[0m"; 
static const char BOLD_SEQ[] = "\033[1m";
static const char BOLD_BLACK_SEQ[] = "\u001b[30;1m";

const char* getColorSeq(const LEVELS& level) {
  
  if ( level.value == WARNING.value ) 
  { 
    return YELLOW_TEXT_SEQ; 
  }
  if ( level.value == ERROR.value )
  {
    return RED_TEXT_SEQ;
  }
  if ( level.value == DEBUG.value )
  {
    return GREEN_TEXT_SEQ;
  }
  if ( g3::internal::wasFatal(level)) 
  { 
    return RED_TEXT_SEQ; 
  }

  return RESET_SEQ;
}

void StandardOutputSink::logMessageCallback(g3::LogMessageMover mover)
{
  g3::LogMessage msg = mover.get();

  std::cout << BOLD_BLACK_SEQ << "[" << msg.file() << ":" 
    << msg.line() << "] " << getColorSeq(msg._level) 
    << msg.level() << ": " << msg.message() << "\n" << RESET_SEQ;
}

