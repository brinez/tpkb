#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstdlib>

#include "hidapi.h"

int main(int argc, char **argv)
{
    const uint16_t vendor_id = 0x17EF;
    const uint16_t product_id = 0x6048;
    unsigned char sens = 0;

    if (hid_init()) {
        return -1;
    }

    hid_device *dev = hid_open(vendor_id, product_id, NULL);

    if (dev == 0) {
        printf("TP Keyboard not found. Possible solutions:\n"
               " * Have you run tpkb with 'sudo'?\n"
               " * Make sure you don't have keyboard customizers running (for example Karabiner)\n");

        //Debug support
        printf("Detected devices:\n");
        hid_device_info *devs = hid_enumerate(0,0);
        for (;devs; devs = devs->next) {
            printf("VID: 0x%04X\tPID: 0x%04X\tManufacturer: %ls\n",
                devs->vendor_id,
                devs->product_id,
                devs->manufacturer_string);
        }
        hid_free_enumeration(devs);
        devs = NULL;

        return 0;
    }

    // Known HID sequences:
    // 18 01 03 - Make F7/F9/F11 return custom single events, as opposed to strings of keys
    // 18 02 xx - Sensitivity, 01..09
    // 18 05 00 - Disable fn-lock
    // 18 05 01 - Enable fn-lock
    // 19 09 00 - Standard mode for middle button
    // 18 09 01 - Special mode for middle button

    if (argc == 2) {
        sens = (unsigned char)atoi(argv[1]);
    }

    if (sens == 0 || sens > 9) {
        sens = 5;
        printf("Invalid mouse sensitivity argument, defaulting to 5.\n");
    } else {
        printf("Setting mouse sensitivity to %u.\n", sens);
    }

    unsigned char buf[4];

    buf[0] = 0x18;
    buf[1] = 0x02;
    buf[2] = sens;
    hid_write(dev, buf, 3);

    hid_close(dev);
    return 0;
}
