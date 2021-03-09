/// @author     John Brooks (jbroo163@ford.com)
/// @brief      g3log library wrapper 
/// @date       02/15/2021
/// @copyright  Ford Motor Company (c) 2019-2021

#include "logging/log.h"
#include <iostream>
#include <memory>
#include <g3log/g3log.hpp>

using namespace logging;

void Logger::initialize()
{
  getInstance();
}

Logger::Logger()
{
  log_worker_ = g3::LogWorker::createLogWorker();

  if(standard_output_enabled_)
  {
    standard_out_sink_handle_ = log_worker_->addSink(std::make_unique<StandardOutputSink>(), 
      &StandardOutputSink::logMessageCallback);  
  }

  initializeLogging(log_worker_.get());
}

void Logger::enableFileOutput(std::string prefix, std::string file_path)
{
  if (file_sink_handle_)
  {
    log_worker_->removeSink(std::move(file_sink_handle_));
  }
  
  if (!log_worker_)
    initialize();
  
  file_sink_handle_ = log_worker_->addDefaultLogger(prefix, file_path);
}

Logger& Logger::getInstance()
{
  static Logger logger_singleton;

  return logger_singleton;
}
