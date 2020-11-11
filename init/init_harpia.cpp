/*
   Copyright (c) 2016, The CyanogenMod Project. All rights reserved.
   Copyright (c) 2016, The LineageOS Project. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <android-base/properties.h>
#include "vendor_init.h"
#include <sys/sysinfo.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

using android::base::GetProperty;

void property_override(char const prop[], char const value[])
{
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void property_override_dual(char const system_prop[], char const vendor_prop[], char const value[])
{
    property_override(system_prop, value);
    property_override(vendor_prop, value);
}

bool is2GB()
{
    struct sysinfo sys;
    sysinfo(&sys);
    return sys.totalram > 1024ull * 1024 * 1024;
}

void vendor_load_properties()
{
    const char *customerid = NULL;
    std::string platform;
    std::string dualsim;
    std::string radio;
    std::string bootdevice;
    std::string sku;
    bool msim = false;
    int rc;

    dualsim = GetProperty("ro.boot.dualsim","");
    if (dualsim == "true") {
        property_override("persist.radio.force_get_pref", "1");
        property_override("persist.radio.multisim.config", "dsds");
        property_override("ro.hw.dualsim", "true");
        msim = true;
    }

    bootdevice = GetProperty("ro.boot.device","");
    property_override("ro.hw.device", bootdevice.c_str());

    radio = GetProperty("ro.boot.radio","");
    property_override("ro.hw.radio", radio.c_str());

    if (is2GB()) {
        property_override("dalvik.vm.heapstartsize", "8m");
        property_override("dalvik.vm.heapgrowthlimit", "192m");
        property_override("dalvik.vm.heapsize", "512m");
        property_override("dalvik.vm.heaptargetutilization", "0.75");
        property_override("dalvik.vm.heapminfree", "512k");
        property_override("dalvik.vm.heapmaxfree", "8m");
    } else {
        property_override("dalvik.vm.heapstartsize", "8m");
        property_override("dalvik.vm.heapgrowthlimit", "96m");
        property_override("dalvik.vm.heapsize", "256m");
        property_override("dalvik.vm.heaptargetutilization", "0.75");
        property_override("dalvik.vm.heapminfree", "2m");
        property_override("dalvik.vm.heapmaxfree", "8m");
    }

    property_override("ro.telephony.default_network", "10");

    sku = GetProperty("ro.boot.hardware.sku","");
    if (sku == "XT1600") {
        /* XT1600 */
        customerid = "retail";
    } else if (sku == "XT1601") {
        /* XT1601 */
        customerid = "retail";
        property_override("persist.radio.process_sups_ind", "1");
        if (msim) {
            property_override("persist.radio.pb.max.match", "8");
            property_override("persist.radio.pb.min.match", "8");
        }
    } else if (sku == "XT1602") {
        /* XT1602 */
    } else if (sku == "XT1603") {
        /* XT1603 */
        customerid = "retail";
        property_override("persist.radio.pb.max.match", "10");
        property_override("persist.radio.pb.min.match", "7");
    } else if (sku == "XT1604") {
        /* XT1604 - HAS NFC! */
    } else if (sku == "XT1607") {
        /* XT1607 */
    } else if (sku == "XT1609") {
        /* XT1609 */
    }

    if (customerid) {
        property_override("ro.mot.build.customerid", customerid);
    }
}
