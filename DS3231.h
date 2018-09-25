#ifndef ACE_TIME_HW_DS3231_H
#define ACE_TIME_HW_DS3231_H

#include <stdint.h>

namespace ace_time {
namespace hw {

class HardwareDateTime;
class HardwareTemperature;

/**
 * A class that reads and writes HardwareDateTime and HardwareTemperature from a
 * DS3231 RTC chip. This class is designed to access just enough features of the
 * DS3231 chip to implement the ace_time::DS3231TimeKeeper class. It is not
 * meant to provide access to all the features of the DS3231 chip. There are
 * other libraries which are far better for that purpose.
 */
class DS3231 {
  public:
    static const uint8_t kI2cAddress = 0x68;

    /** Constructor. */
    explicit DS3231(uint8_t address = kI2cAddress):
        mAddress(address) {}

    /** Read the time into the HardwareDateTime object. */
    void readDateTime(HardwareDateTime* dateTime) const;

    /** Set the DS3231 with the HardwareDateTime values. */
    void setDateTime(const HardwareDateTime& dateTime) const;

    /** Read the temperature into the HardwareTemperature object. */
    void readTemperature(HardwareTemperature* temperature) const;

  private:
    const uint8_t mAddress;
};

}
}

#endif
