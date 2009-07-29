/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/route.h>

#include <android/log.h>
#include <cutils/properties.h>

static inline struct in_addr *in_addr(struct sockaddr *sa)
{
    return &((struct sockaddr_in *)sa)->sin_addr;
}

int main(int argc, char **argv)
{
    struct rtentry route = {
        .rt_dst     = {.sa_family = AF_INET},
        .rt_genmask = {.sa_family = AF_INET},
        .rt_gateway = {.sa_family = AF_INET},
        .rt_flags   = RTF_UP | RTF_GATEWAY,
    };
    FILE *f;
    int s;

    errno = EINVAL;
    if (argc > 5 && inet_aton(argv[5], in_addr(&route.rt_gateway)) &&
        (f = fopen("/proc/net/route", "r")) != NULL &&
        (s = socket(AF_INET, SOCK_DGRAM, 0)) != -1) {
        uint32_t *address = &in_addr(&route.rt_dst)->s_addr;
        uint32_t *netmask = &in_addr(&route.rt_genmask)->s_addr;
        char device[64];

        fscanf(f, "%*[^\n]\n");
        while (fscanf(f, "%63s%X%*X%*X%*d%*u%*d%X%*d%*u%*u\n",
                      device, address, netmask) == 3) {
            if (strcmp(argv[1], device)) {
                uint32_t bit = ntohl(*netmask);
                bit = htonl(bit ^ (1 << 31 | bit >> 1));
                if (bit) {
                    *netmask |= bit;
                    if (ioctl(s, SIOCADDRT, &route) == -1 && errno != EEXIST) {
                        break;
                    }
                    *address ^= bit;
                    if (ioctl(s, SIOCADDRT, &route) == -1 && errno != EEXIST) {
                        break;
                    }
                    errno = 0;
                }
            }
        }
    }

    if (!errno) {
        char *dns = getenv("DNS1");
        property_set("vpn.dns1", dns ? dns : "");
        dns = getenv("DNS2");
        property_set("vpn.dns2", dns ? dns : "");
        property_set("vpn.status", "ok");
        __android_log_print(ANDROID_LOG_INFO, "ip-up-vpn",
                            "All traffic is now redirected to %s", argv[5]);
    } else {
        property_set("vpn.status", "error");
        __android_log_write(ANDROID_LOG_ERROR, "ip-up-vpn", strerror(errno));
    }
    return errno;
}
