# Logging

The logging module provides a convienent interface for outputting runtime information to the terminal, a local file, or other outputs. It wraps the g3log library.

https://github.com/KjellKod/g3log/

## Usage in applications

In order for the logging functionality to work correctly each application should call the initialize function of the logging library.

```
#include "modules/logging/Log.h"

int main(int argc, char** argv)
{
  logging::Logger::initialize();
  
  ...
}
```

#### Logging to a file

Calling `Logger::getInstance().enableFileOutput(prefix, file_path)` will enable file output. See [Log.h](https://github.ford.com/VDS-Research/terminal-traffic-controller/blob/jbroo163-TTM-1253-implement-logging-module/modules/logging/Log.h) for more information.

## Usage in modules

Simply include the `Log.h` header in your source file. From there you can use the g3log library macros to print to the log file.

The currently supported log levels in order of verbosity are...
* DEBUG
* INFO
* WARNING
* ERROR
* FATAL

#### Stream style logging
```
LOG(DEBUG) << "This is a debug message.";
LOG(ERROR) << "This is an error message.";
```

#### Printf style logging
```
LOGF(WARNING, "Printf-style syntax is also %s", "available");
```

#### Conditional logging
```
int less = 1; int more = 2
LOG_IF(INFO, (less<more)) <<"If [true], then this text will be logged";
```
or with printf-like syntax
```
LOGF_IF(INFO, (less<more), "if %d<%d then this text will be logged", less,more);
```

#### Design-by-Contract
```
CHECK(false) will trigger a "fatal" message. It will be logged, and then the application will exit.

CHECK(less != more); // not FATAL
CHECK(less > more) << "CHECK(false) triggers a FATAL message";
```



