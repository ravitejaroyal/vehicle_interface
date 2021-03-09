/// @author     John Brooks (jbroo163@ford.com)
/// @brief      g3log library wrapper 
/// @date       02/15/2021
/// @copyright  Ford Motor Company (c) 2019-2021

#pragma once

#include "logging/standard_output_sink.h"
#include "logging/additional_logging_levels.h"

#include <string>
#include <memory>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

namespace logging {
  
  ///
  /// @brief Singleton class which provides an interface to the g3log library
  /// 
  class Logger {
    public:
      Logger(Logger &other) = delete;
      void operator=(const Logger &rhs) = delete;

      ///
      /// @brief intializes the g3log library. This must be called prior to any
      /// LOG(...) calls. It's recommended to add this as the first line in the 
      /// top level application's main() function.
      ///
      static void initialize();

      ///
      /// @brief returns a reference to the Logger singleton, through which, changes 
      /// to the g3log settings can be made.
      ///
      static Logger& getInstance();

      ///
      /// @brief Enables the storing of log output to a local file.
      /// 
      /// The filename will be of the format
      /// "[file_path]/[prefix].g3log.[timestamp].log" where timestamp is formatted
      /// as "yyyymmdd-hhmmss" of the log creation time in UTC.
      ///
      /// @param prefix string that is added to the beginning of the filename. Suggested
      /// usage is the application name such as "map-validator"#include "StandardOutputSink.h"
      /// @param file_path location where the file should be stored. For example; 
      /// "/home/me/my_app/logs", argv[0] from main(), or std::filesystem::current_path().
      /// 
      void enableFileOutput(std::string prefix, std::string file_path);

    private:
      Logger();

      bool file_output_enabled_ = true;
      bool standard_output_enabled_ = true;
      std::unique_ptr<g3::LogWorker> log_worker_;
      std::unique_ptr<g3::SinkHandle<StandardOutputSink>> standard_out_sink_handle_;
      std::unique_ptr<g3::FileSinkHandle> file_sink_handle_;
  };
};

