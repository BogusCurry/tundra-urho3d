/**
    For conditions of distribution and use, see copyright notice in LICENSE

    @file   LoggingFunctions.h
    @brief  Tundra logging utility functions. */

#pragma once

#include "TundraCoreApi.h"
#include "CoreTypes.h"

#include <Str.h>
#include <string>

namespace Tundra
{

/// Specifies the different available log levels. Each log level includes all the output from the levels above it.
enum LogChannel
{
    LogChannelError = 1,
    LogChannelWarning = 2,
    LogChannelInfo = 4,
    LogChannelDebug = 8,

    // The following are bit combinations of the above primitive channels:
    // Do not use these in calls to PrintLogMessage, but only in SetEnabledLogChannels.

    LogLevelQuiet = 0, ///< Disable all output logging.
    LogLevelErrorsOnly = LogChannelError,
    LogLevelErrorWarning = LogChannelError | LogChannelWarning,
    LogLevelErrorWarnInfo = LogChannelError | LogChannelWarning | LogChannelInfo,
    LogLevelErrorWarnInfoDebug = LogChannelError | LogChannelWarning | LogChannelInfo | LogChannelDebug
};

/// Outputs a message to the log to the given channel (if the channel is enabled) to both stdout and ConsoleAPI.
/** On Windows, yellow and red text colors are used for warning and error prints. */
void TUNDRACORE_API PrintLogMessage(u32 logChannel, const String &str);

/// Returns true if the given log channel is enabled.
bool TUNDRACORE_API IsLogChannelEnabled(u32 logChannel);

/// Outputs a string to the stdout.
void TUNDRACORE_API PrintRaw(const String &str);

/// Outputs an error message to the log (if the channel is enabled), both stdout and ConsoleAPI, with "Error: " prefix and with newline appended.
/** On Windows, red text color is used for the stdout print. */
static inline void LogError(const String &msg)     { if (IsLogChannelEnabled(LogChannelError)) PrintLogMessage(LogChannelError, "Error: " + msg + "\n");}

/// Outputs a warning message to the log (if the channel is enabled), both stdout and ConsoleAPI, with "Warning: " prefix and with newline appended.
/** On Windows, yellow text color is used for the stdout print. */
static inline void LogWarning(const String &msg)   { if (IsLogChannelEnabled(LogChannelWarning)) PrintLogMessage(LogChannelWarning, "Warning: " + msg + "\n");}

/// Outputs an information message to the log (if the channel is enabled), both stdout and ConsoleAPI, with newline appended.
static inline void LogInfo(const String &msg)      { if (IsLogChannelEnabled(LogChannelInfo)) PrintLogMessage(LogChannelInfo, msg + "\n");}

/// Outputs a debug message to the log (if the channel is enabled), both stdout and ConsoleAPI, with "Debug: " prefix and with newline appended.
static inline void LogDebug(const String &msg)     { if (IsLogChannelEnabled(LogChannelDebug)) PrintLogMessage(LogChannelDebug, "Debug: " + msg + "\n");}

static inline void LogError(const std::string &msg) /**< @overload */   { if (IsLogChannelEnabled(LogChannelError)) PrintLogMessage(LogChannelError, "Error: " + String(msg.c_str()) + "\n"); }
static inline void LogWarning(const std::string &msg) /**< @overload */ { if (IsLogChannelEnabled(LogChannelWarning)) PrintLogMessage(LogChannelWarning, "Warning: " + String(msg.c_str()) + "\n"); }
static inline void LogInfo(const std::string &msg) /**< @overload */    { if (IsLogChannelEnabled(LogChannelInfo)) PrintLogMessage(LogChannelInfo, String(msg.c_str()) + "\n"); }
static inline void LogDebug(const std::string &msg) /**< @overload */   { if (IsLogChannelEnabled(LogChannelDebug)) PrintLogMessage(LogChannelDebug, "Debug: "+ String(msg.c_str()) + "\n"); }

static inline void LogError(const char *msg) /**< @overload */  { if (IsLogChannelEnabled(LogChannelError)) PrintLogMessage(LogChannelError, "Error: " + String(msg) + "\n"); }
static inline void LogWarning(const char *msg) /**< @overload */{ if (IsLogChannelEnabled(LogChannelWarning)) PrintLogMessage(LogChannelWarning, "Warning: " + String(msg) + "\n"); }
static inline void LogInfo(const char *msg) /**< @overload */   { if (IsLogChannelEnabled(LogChannelInfo)) PrintLogMessage(LogChannelInfo, String(msg) + "\n"); }
static inline void LogDebug(const char *msg) /**< @overload */  { if (IsLogChannelEnabled(LogChannelDebug)) PrintLogMessage(LogChannelDebug, "Debug: " + String(msg) + "\n"); }

}
