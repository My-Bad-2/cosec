#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <arch.hpp>

#include <debug/log.hpp>

namespace debug {
Logger& Logger::operator<<(const char* str) {
    println(m_color.color);
    println(str);
    println("\033[0m");

    return *this;
}

Logger& Logger::operator<<(char str) {
    println(m_color.color);
    printc(str);
    println("\033[0m");

    return *this;
}

Logger& Logger::operator<<(int value) {
    println(m_color.color);
    print_signed(value);
    println("\033[0m");

    return *this;
}

Logger& Logger::operator<<(unsigned value) {
    println(m_color.color);
    print_unsigned(value);
    println("\033[0m");

    return *this;
}

Logger& Logger::operator<<(unsigned long value) {
    println(m_color.color);
    print_unsigned(value);
    println("\033[0m");

    return *this;
}

Logger& Logger::operator<<(Hex value) {
    println(m_color.color);
    println("0x");
    print_hex(value.m_data, value.m_size);
    println("\033[0m");

    return *this;
}

Logger& Logger::operator<<(Hex8 value) {
    println(m_color.color);
    println("0x");
    print_hex(value.m_data, 1);
    println("\033[0m");

    return *this;
}

Logger& Logger::operator<<(Hex16 value) {
    println(m_color.color);
    println("0x");
    print_hex(value.m_data, 2);
    println("\033[0m");

    return *this;
}

Logger& Logger::operator<<(Hex32 value) {
    println(m_color.color);
    println("0x");
    print_hex(value.m_data, 2);
    println("\033[0m");

    return *this;
}

Logger& Logger::operator<<(Hex64 value) {
    println(m_color.color);
    println("0x");
    print_hex((value.m_data & 0xFFFFFFFF00000000) >> 32, 4);
    print_hex(value.m_data & 0xFFFFFFFF, 4);
    println("\033[0m");

    return *this;
}

Logger& Logger::operator<<(void* ptr) {
    println(m_color.color);
    println("0x");
    print_hex(reinterpret_cast<uintptr_t>(ptr), sizeof(uintptr_t));
    println("\033[0m");

    return *this;
}

void Logger::print_hex(uint32_t value, uint8_t size) {
    char buf[9];
    itoa(buf, value, 16);

    for (size_t i = 0; i < strlen(buf); i++) {
        buf[i] = toupper(buf[i]);
    }

    size_t leading_zeros = size * 2 - strlen(buf);

    for (size_t i = 0; i < leading_zeros; i++) {
        printc('0');
    }

    println(buf);
}

void Logger::print_unsigned(unsigned value) {
    char buf[11];
    itoa(buf, value, 10);
    println(buf);
}

void Logger::print_unsigned(unsigned long value) {
    char buf[21];
    itoa(buf, value, 20);
    println(buf);
}

void Logger::print_signed(int value) {
    char buf[11];

    if (value < 0) {
        value = 0 - value;
        printc('-');
    }

    itoa(buf, value, 10);
    println(buf);
}
}  // namespace debug

namespace log {
debug::info info;
debug::err error;
debug::warn warn;
debug::debug debug;
}  // namespace log