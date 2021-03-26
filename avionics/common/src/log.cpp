#include <log.hpp>

namespace rktlog {

// --- FUNCTIONS ---

/**
 * @brief Converts an int32 into a C-style string. No dynamic memory
 * allocations are used.
 * @param buf Pointer to character buffer of at least 12 bytes, where the
 * output is written. At most 12 bytes are used. 12 is just sufficient to
 * contain the maximum int32 value, including sign character and terminating
 * zero.
 * @return Pointer to start of used buffer - this is not equal to buf; see
 * implementation notes
 *
 * Getting characters out of an integer (the easy way) involves a loop of
 * modulus & integer division, but this ends up getting the values from
 * right to left. So, for convenience we'll just write the output from right
 * to left, starting at the last position. This necessitates reporting what
 * final position we ended at, hence the return value does that.
 */
char *itostr(char *buf, std::int32_t val) {
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    // fill in null termination, and start writing backwards
    buf[11] = '\0';
    buf += 10;

    // keep in mind that if val = -2147283648, can't write val = -val (overflows
    // integer) - hence need manual initial iteration to prevent this
    bool negative = false;
    if (val < 0) {
        *buf = '0' - val % 10;
        val = -(val / 10);
        negative = true;
    } else {
        *buf = '0' + val % 10;
        val /= 10;
    }

    --buf;

    while (val != 0) {
        *buf = '0' + val % 10;
        --buf;
        val /= 10;
    }
    // buf is always one position behind the last written one

    if (negative) {
        *buf = '-';
        return buf;
    }

    return buf + 1;
}

// --- RawOutput ---

RawOutput &RawOutput::put(unsigned char c) {
    putImpl(c);
    return *this;
}

RawOutput &RawOutput::put(unsigned const char *c, int len) {
    putImpl(c, len);
    return *this;
}

void RawOutput::putImpl(unsigned const char *c, std::size_t len) {
    for (std::size_t i = 0; i < len; ++i) {
        putImpl(*c++);
    }
}

// --- FormatOutput ---

FormatOutput::FormatOutput(RawOutput &out) : out_(&out) {}

void FormatOutput::setRawOutput(RawOutput &out) { out_ = &out; }

FormatOutput &FormatOutput::operator<<(const char *str) {
    while (*str != '\0') {
        out_->put(static_cast<unsigned char>(*str));
        ++str;
    }

    return *this;
}

FormatOutput &FormatOutput::operator<<(char c) {
    out_->put(c);
    return *this;
}

FormatOutput &FormatOutput::operator<<(std::int32_t val) {
    char buf[kInt32Strlen];

    *this << itostr(buf, val);
    return *this;
}

FormatOutput &FormatOutput::operator<<(Endl &&) {
    out_->put(static_cast<unsigned char>('\n'));
    out_->flush();
    return *this;
}

// --- Logger ---

DevNullOutput Logger::default_raw_output_;

FormatOutput Logger::output_(Logger::default_raw_output_);

const char *Logger::level_names_[4] = {"DEBG", "INFO", "WARN", "ERRO"};

template <Level level>
FormatOutput &Logger::logAt(const char *file, std::int32_t line) {
    const char *lvlName = level_names_[static_cast<int>(level)];
    output_ << lvlName << "  " << file << '[' << line << "]: ";
    return output_;
}

// explicitly instantiate log levels
template FormatOutput &Logger::logAt<Level::kDebug>(const char *, std::int32_t);
template FormatOutput &Logger::logAt<Level::kInfo>(const char *, std::int32_t);
template FormatOutput &Logger::logAt<Level::kWarn>(const char *, std::int32_t);
template FormatOutput &Logger::logAt<Level::kErr>(const char *, std::int32_t);

void Logger::setOutput(RawOutput &out) { output_.setRawOutput(out); }

} // namespace rktlog
