/// @author     John Brooks (jbroo163@ford.com)
/// @brief      Standard output logging sink for g3log library 
/// @date       02/15/2021
/// @copyright  Ford Motor Company (c) 2019-2021

#pragma once

#include <g3log/logmessage.hpp>

namespace logging 
{
  ///
  /// @brief Standard output logging class that provides the sink callback function 
  /// for g3log
  /// 
  class StandardOutputSink
  {
    public:
      ///
      /// @brief Callback function which can be registered to the g3log sink. 
      /// 
      /// @param mover populated by the g3log library with the content and context 
      /// of the log message.
      void logMessageCallback(g3::LogMessageMover mover);
  };
}

