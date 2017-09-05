#ifndef HARDCODE_H
#define HARDCODE_H

enum E_HardwareType
{
    HT_UNKNOWN=-1,
    HT_HARD_DISK=1,
    HT_NET_CARD=2,
    HT_HARD_DISK_NEW=11
};


E_HardwareType	GetDiskId(char*	szBuffer);

#endif // HARDCODE_H
