#include <stddef.h>
#include <stdint.h>

namespace system::port {
extern "C" {
/**
 * @brief Receives a 8-bit value from an I/O location
 * 
 * @param port I/O location
 * @return uint8_t
 */
uint8_t in8(uint16_t port);

/**
 * @brief Receives a 16-bit value from an I/O location
 * 
 * @param port I/O location
 * @return uint16_t
 */
uint16_t in16(uint16_t port);

/**
 * @brief Receives a 32-bit value from an I/O location
 * 
 * @param port I/O location
 * @return uint32_t
 */
uint32_t in32(uint16_t port);

/**
 * @brief Sends a 8-bit value on an I/O location
 * 
 * @param port I/O location
 * @param val 
 */
void out8(uint16_t port, uint8_t val);

/**
 * @brief Sends a 16-bit value on an I/O location
 * 
 * @param port I/O location
 * @param val 
 */
void out16(uint16_t port, uint16_t val);

/**
 * @brief Sends a 32-bit value on an I/O location
 * 
 * @param port I/O location
 * @param val 
 */
void out32(uint16_t port, uint32_t val);

/**
 * @brief Wait a very small amount of time (1-4 μs, generally).
 * 
 */
void io_pause();
}
}  // namespace system::port