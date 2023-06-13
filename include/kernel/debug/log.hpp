#include <stddef.h>
#include <stdint.h>

class Hex;
class Hex8;
class Hex16;
class Hex32;
class Hex64;

namespace debug {
struct Color {
    const char* color;

    Color(const char* color) : color(color) {}
};

namespace colors {
static Color Black = "\033[0;30m";
static Color Red = "\033[0;31m";
static Color Green = "\033[0;32m";
static Color Yellow = "\033[0;33m";
static Color Blue = "\033[0;34m";
static Color Magenta = "\033[0;35m";
static Color Cyan = "\033[0;36m";
static Color White = "\033[0;37m";
static Color Bright_red = "\033[1;31m";
static Color Bright_green = "\033[1;32m";
static Color Bright_yellow = "\033[1;33m";
static Color Bright_blue = "\033[1;34m";
static Color Bright_magenta = "\033[1;35m";
static Color Bright_cyan = "\033[1;36m";
static Color Bright_white = "\033[1;37m";
}  // namespace colors

class Logger {
   public:
    static void init() {}

    Logger(Color color = colors::White) : m_color(color) {}

    Logger& operator<<(const char* str);
    Logger& operator<<(char str);
    Logger& operator<<(int value);
    Logger& operator<<(unsigned value);
    Logger& operator<<(uint64_t value);
    Logger& operator<<(Hex value);
    Logger& operator<<(Hex8 value);
    Logger& operator<<(Hex16 value);
    Logger& operator<<(Hex32 value);
    Logger& operator<<(Hex64 value);
    Logger& operator<<(void* ptr);

   private:
    void print_hex(uint32_t value, uint8_t size = 0);
    void print_unsigned(unsigned value);
    void print_unsigned(unsigned long value);
    void print_signed(int value);

   protected:
    Color m_color;
};

class debug : public Logger {
   public:
    debug() : Logger(colors::Bright_blue) {}
};

class info : public Logger {
   public:
    info() : Logger(colors::Green) {}
};

class warn : public Logger {
   public:
    warn() : Logger(colors::Yellow) {}
};

class err : public Logger {
   public:
    err() : Logger(colors::Red) {}
};
}  // namespace debug

class Hex {
   private:
    uint32_t m_data;
    int m_size;

   public:
    Hex(uint8_t value) : m_data(value), m_size(1) {}

    Hex(uint16_t value) : m_data(value), m_size(2) {}

    Hex(uint32_t value) : m_data(value), m_size(4) {}
    friend class debug::Logger;
};

class Hex8 {
   private:
    int m_data;

   public:
    Hex8(uint8_t value) : m_data(value) {}
    friend class debug::Logger;
};

class Hex16 {
   private:
    int m_data;

   public:
    Hex16(uint16_t value) : m_data(value) {}
    friend class debug::Logger;
};

class Hex32 {
   private:
    int m_data;

   public:
    Hex32(uint32_t value) : m_data(value) {}
    friend class debug::Logger;
};

class Hex64 {
   private:
    long int m_data;

   public:
    Hex64(uint64_t value) : m_data(value) {}
    friend class debug::Logger;
};

namespace log {
extern debug::info info;
extern debug::err error;
extern debug::warn warn;
extern debug::debug debug;
}  // namespace log