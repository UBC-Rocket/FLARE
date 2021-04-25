#ifndef LOG_HPP_E90DE04B2AC74C05BE2B65A022ED84AB
#define LOG_HPP_E90DE04B2AC74C05BE2B65A022ED84AB

#include <cstdint>
#include <cstdio>
#include <ostream>

// TODO: provide more granularity (i.e. specify log level to keep) in
// log_control.hpp

// Define the macro LOG_CONTROL_DISABLE_LOGGING in log_control.hpp to disable
// logging. Seperate versions of log_control.hpp are used between the board and
// x86 configurations.
#include "log_control.hpp"

/*
Defined logging levels. Usage:
LOG_WARN("Warning message; warning code: " << 123 << " addnl string")
*/
#ifndef LOG_CONTROL_DISABLE_LOGGING

#define LOG_DEBUG(expr) LOG_AT_SPECIFIED_LEVEL(rktlog::Level::kDebug, expr)
#define LOG_INFO(expr) LOG_AT_SPECIFIED_LEVEL(rktlog::Level::kInfo, expr)
#define LOG_WARN(expr) LOG_AT_SPECIFIED_LEVEL(rktlog::Level::kWarn, expr)
#define LOG_ERROR(expr) LOG_AT_SPECIFIED_LEVEL(rktlog::Level::kErr, expr)

#else // LOG_CONTROL_DISABLE_LOGGING

#define LOG_DEBUG(expr)
#define LOG_INFO(expr)
#define LOG_WARN(expr)
#define LOG_ERROR(expr)

#endif // LOG_CONTROL_DISABLE_LOGGING

// Log at a specified level. Strongly recommended to instead use the defined log
// level macros instead of this.
#define LOG_AT_SPECIFIED_LEVEL(level, expr)                                    \
    rktlog::Logger::logAt<level>(__FILE__, __LINE__) << expr << rktlog::Endl()

namespace rktlog {

// Type used to take advantage of function overloading to flush the stream. Used
// internally by logger; do not use externally.
class Endl {};

/**
 * @brief Serves as interface to an output stream. Subclass this class to
 * specify the output of the logger (see below), or use one of the existing
 * subclasses.
 */
class RawOutput {
  public:
    /**
     * @brief Raw output function. Puts a single byte into the output. Calls
     * putImpl but returns *this to allow method chaining.
     * @param c Byte to output.
     */
    RawOutput &put(unsigned char c);

    /**
     * @brief Raw output function. Calls putImpl but returns *this to allow
     * method chaining.
     * @param c Pointer to bytes.
     * @param len Length of data to output.
     */
    RawOutput &put(unsigned const char *c, int len);

    /**
     * @brief Flushes the output.
     */
    virtual void flush() = 0;

  protected:
    /**
     * @brief Implementation of put(); to be overriden.
     * @param c Byte to output.
     */
    virtual void putImpl(unsigned char c) = 0;

    /**
     * @brief Implementation of put(); to be overriden. Convenient for cases
     * where it may be more efficient to send multiple bytes at a time.
     * @param c Pointer to bytes to output.
     * @param len Number of bytes to output.
     */
    virtual void putImpl(unsigned const char *c, std::size_t len);
};

/**
 * @brief Implements RawOutput but discards the output.
 */
class DevNullOutput : public RawOutput {
    void putImpl(unsigned char) override{};
    void flush() override {}
};

/**
 * @brief Implements RawOutput for any std::ostream; e.g. for logging to
 * std::cerr or log files.
 */
class OstreamOutput : public RawOutput {
  private:
    std::ostream &stream_;

    void putImpl(unsigned char c) override { stream_ << c; }

    void putImpl(unsigned const char *c, std::size_t len) override {
        stream_.write(reinterpret_cast<const char *>(c), len);
    }

    void flush() override { stream_.flush(); }

  public:
    OstreamOutput(std::ostream &stream) : stream_(stream) {}
};

/**
 * @brief List of the available logging levels
 */
enum class Level { kDebug, kInfo, kWarn, kErr };

/**
 * @brief Provides formatting on top of a RawOutput class. Do not use outside of
 * the log implementation; instead use the LOG_[LEVEL] macros.
 */
class FormatOutput {
  private:
    static constexpr std::size_t kInt32Strlen = 12;

  public:
    /**
     * @brief Initializes the internal output stream pointer.
     * @param out Reference to RawOutput
     */
    FormatOutput(RawOutput &out);

    /**
     * @brief Specify the output stream for the formatter
     */
    void setRawOutput(RawOutput &out);

    /**
     * @brief Various stream-out overloads to print formatted results.
     */
    FormatOutput &operator<<(const char *str);

    FormatOutput &operator<<(char c);

    // TODO: Create more operator<< for different sized integers and floating
    // point numbers
    FormatOutput &operator<<(std::int32_t val);

    FormatOutput &operator<<(Endl &&);

  private:
    // Pointer is needed to make it possible to change the output stream
    // dynamically.
    RawOutput *out_;
};

/**
 * @brief Implementation class for logging. Not all public methods here should
 * be called; instead use LOG_[LEVEL] macros. Public methods that are
 * appropriate to be called are indicated in documentation with [CALL ME].
 */
class Logger {
  private:
    static DevNullOutput default_raw_output_;
    static FormatOutput output_;

    // look up table for level names
    static const char *level_names_[4];

  public:
    /**
     * @brief Prefix to all output log messages. Called by the LOG_* macros; do
     * not call directly.
     * @param file Name of the file of the log statement
     * @param line Line of the file of the log statement
     */
    template <Level level>
    static FormatOutput &logAt(const char *file, std::int32_t line);

    /**
     * @brief [CALL ME] Set where the output of the logger goes to. Call to
     * specify output for the logger; otherwise the default is to output to a
     * DevNull output (i.e. all logs are discarded)
     * @param out RawOutput object, where output goes to.
     */
    static void setOutput(RawOutput &out);
};

} // namespace rktlog

#endif
