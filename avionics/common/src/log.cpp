#include <log.hpp>

namespace rktlog {

// --- FUNCTIONS ---

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
    std::sprintf(buf, "%i", val);
    return *this << buf;
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
