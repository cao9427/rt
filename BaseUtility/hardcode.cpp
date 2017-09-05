#include "hardcode.h"
#include <QtGlobal>
#include <QString>
#include <QSettings>
#include <QtNetwork/QNetworkInterface>
#ifdef Q_OS_WIN32
#include <windows.h>
#include <iptypes.h>
#include <Iphlpapi.h>
#include <IPHlpApi.h>
#elif defined  Q_OS_LINUX | defined Q_OS_MACOS
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#define FALSE 0
#define TRUE 1
typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef unsigned char BOOL;
#endif

#if defined Q_OS_LINUX
#include <linux/hdreg.h>
#endif


#include <iostream>
#include <string>
#include <algorithm>
#include <list>


using namespace std;

char HardDriveSerialNumber [1024];


#ifdef Q_OS_WIN32
const int  MAX_IDE_DRIVES = 16;
#define   DFP_GET_VERSION   0x00074080
#define   DFP_SEND_DRIVE_COMMAND   0x0007c084
#define   DFP_RECEIVE_DRIVE_DATA   0x0007c088

#define  SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE
#define  IDENTIFY_BUFFER_SIZE  512
#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ((FILE_DEVICE_SCSI << 16) + 0x0501)
#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition
#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.
#define  IOCTL_GET_DRIVE_INFO   0x0007c088
#define  IOCTL_GET_VERSION          0x00074080

char HardDriveModelNumber [1024];

typedef   struct   _GETVERSIONOUTPARAMS   {
    BYTE   bVersion;        //Binary   driver   version.
    BYTE   bRevision;       //Binary   driver   revision.
    BYTE   bReserved;       //Not   used.
    BYTE   bIDEDeviceMap;   //Bit   map   of   IDE   devices.
    DWORD   fCapabilities; //Bit   mask   of   driver   capabilities.
    DWORD   dwReserved[4]; //For   future   use.
}   GETVERSIONOUTPARAMS,   *PGETVERSIONOUTPARAMS,   *LPGETVERSIONOUTPARAMS;

typedef struct _IDENTIFY_DATA {
    USHORT GeneralConfiguration;            // 00 00
    USHORT NumberOfCylinders;               // 02  1
    USHORT Reserved1;                       // 04  2
    USHORT NumberOfHeads;                   // 06  3
    USHORT UnformattedBytesPerTrack;        // 08  4
    USHORT UnformattedBytesPerSector;       // 0A  5
    USHORT SectorsPerTrack;                 // 0C  6
    USHORT VendorUnique1[3];                // 0E  7-9
    USHORT SerialNumber[10];                // 14  10-19
    USHORT BufferType;                      // 28  20
    USHORT BufferSectorSize;                // 2A  21
    USHORT NumberOfEccBytes;                // 2C  22
    USHORT FirmwareRevision[4];             // 2E  23-26
    USHORT ModelNumber[20];                 // 36  27-46
    UCHAR  MaximumBlockTransfer;            // 5E  47
    UCHAR  VendorUnique2;                   // 5F
    USHORT DoubleWordIo;                    // 60  48
    USHORT Capabilities;                    // 62  49
    USHORT Reserved2;                       // 64  50
    UCHAR  VendorUnique3;                   // 66  51
    UCHAR  PioCycleTimingMode;              // 67
    UCHAR  VendorUnique4;                   // 68  52
    UCHAR  DmaCycleTimingMode;              // 69
    USHORT TranslationFieldsValid:1;        // 6A  53
    USHORT Reserved3:15;
    USHORT NumberOfCurrentCylinders;        // 6C  54
    USHORT NumberOfCurrentHeads;            // 6E  55
    USHORT CurrentSectorsPerTrack;          // 70  56
    ULONG  CurrentSectorCapacity;           // 72  57-58
    USHORT CurrentMultiSectorSetting;       //     59
    ULONG  UserAddressableSectors;          //     60-61
    USHORT SingleWordDMASupport : 8;        //     62
    USHORT SingleWordDMAActive : 8;
    USHORT MultiWordDMASupport : 8;         //     63
    USHORT MultiWordDMAActive : 8;
    USHORT AdvancedPIOModes : 8;            //     64
    USHORT Reserved4 : 8;
    USHORT MinimumMWXferCycleTime;          //     65
    USHORT RecommendedMWXferCycleTime;      //     66
    USHORT MinimumPIOCycleTime;             //     67
    USHORT MinimumPIOCycleTimeIORDY;        //     68
    USHORT Reserved5[2];                    //     69-70
    USHORT ReleaseTimeOverlapped;           //     71
    USHORT ReleaseTimeServiceCommand;       //     72
    USHORT MajorRevision;                   //     73
    USHORT MinorRevision;                   //     74
    USHORT Reserved6[50];                   //     75-126
    USHORT SpecialFunctionsEnabled;         //     127
    USHORT Reserved7[128];                  //     128-255
} IDENTIFY_DATA, *PIDENTIFY_DATA;

typedef struct _SRB_IO_CONTROL{
    ULONG HeaderLength;
    UCHAR Signature[8];
    ULONG Timeout;
    ULONG ControlCode;
    ULONG ReturnCode;
    ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;

typedef struct _IDSECTOR{
    USHORT  wGenConfig;
    USHORT  wNumCyls;
    USHORT  wReserved;
    USHORT  wNumHeads;
    USHORT  wBytesPerTrack;
    USHORT  wBytesPerSector;
    USHORT  wSectorsPerTrack;
    USHORT  wVendorUnique[3];
    CHAR    sSerialNumber[20];
    USHORT  wBufferType;
    USHORT  wBufferSize;
    USHORT  wECCSize;
    CHAR    sFirmwareRev[8];
    CHAR    sModelNumber[40];
    USHORT  wMoreVendorUnique;
    USHORT  wDoubleWordIO;
    USHORT  wCapabilities;
    USHORT  wReserved1;
    USHORT  wPIOTiming;
    USHORT  wDMATiming;
    USHORT  wBS;
    USHORT  wNumCurrentCyls;
    USHORT  wNumCurrentHeads;
    USHORT  wNumCurrentSectorsPerTrack;
    ULONG   ulCurrentSectorCapacity;
    USHORT  wMultSectorStuff;
    ULONG   ulTotalAddressableSectors;
    USHORT  wSingleWordDMA;
    USHORT  wMultiWordDMA;
    BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;
#endif

char *ConvertToString (DWORD diskdata [256],int firstIndex,int lastIndex,char* buf)
{
    int index = 0;
    int position = 0;

    //  each integer has two characters stored in it backwards
    for (index = firstIndex; index <= lastIndex; index++){
        //  get high byte for 1st character
        buf [position++] = (char) (diskdata [index] / 256);

        //  get low byte for 2nd character
        buf [position++] = (char) (diskdata [index] % 256);
    }

    //  end the string
    buf[position] = '\0';

    //  cut off the trailing blanks
    for (index = position - 1; index > 0 && isspace(buf [index]); index--)
        buf [index] = '\0';

    return buf;
}

std::string& trim(std::string &s)
{
    if (s.empty()){
        return s;
    }

    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

int GetMacAddress(QList<QString> &maclist)
{
    int nCount = 0;

    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();

    foreach(QNetworkInterface interfaceItem, interfaceList){
        if(interfaceItem.flags().testFlag(QNetworkInterface::IsUp)
                &&interfaceItem.flags().testFlag(QNetworkInterface::IsRunning)
                &&interfaceItem.flags().testFlag(QNetworkInterface::CanBroadcast)
                &&interfaceItem.flags().testFlag(QNetworkInterface::CanMulticast)
                &&!interfaceItem.flags().testFlag(QNetworkInterface::IsLoopBack)
                &&interfaceItem.hardwareAddress().toUpper() !="00:50:56:C0:00:01"
                &&interfaceItem.hardwareAddress().toUpper() !="00:50:56:C0:00:08"){
            QList<QNetworkAddressEntry> addressEntryList=interfaceItem.addressEntries();
            foreach(QNetworkAddressEntry addressEntryItem, addressEntryList){
                if(addressEntryItem.ip().protocol()==QAbstractSocket::IPv4Protocol){
                    maclist.push_back(interfaceItem.hardwareAddress().toUpper().replace(':','-'));
                    nCount++;
                    std::cout<<nCount<<std::endl;
                }
            }
        }
    }
    return nCount;
}

#ifdef Q_OS_WIN32
BOOL ReadPhysicalDriveInNTUsingSmart (std::list<std::string> &listRes)
{
    BOOL done = FALSE;
    int drive = 0;
    char resbuf[100]={0};
    for (drive = 0; drive < MAX_IDE_DRIVES; drive++){
        HANDLE hPhysicalDriveIOCTL = 0;

        //  Try to get a handle to PhysicalDrive IOCTL, report failure
        //  and exit if can't.
        char driveName [256];

        sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

        //  Windows NT, Windows 2000, Windows Server 2003, Vista
        hPhysicalDriveIOCTL = CreateFileA (driveName,
                                           GENERIC_READ | GENERIC_WRITE,
                                           FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                                           NULL, OPEN_EXISTING, 0, NULL);
        // if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
        //    printf ("Unable to open physical drive %d, error code: 0x%lX\n",
        //            drive, GetLastError ());

        if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE){
#ifdef PRINTING_TO_CONSOLE_ALLOWED
            if (PRINT_DEBUG)
                printf ("\n%d ReadPhysicalDriveInNTUsingSmart ERROR"
                        "\nCreateFile(%s) returned INVALID_HANDLE_VALUE\n"
                        "Error Code %d\n",
                        __LINE__, driveName, GetLastError ());
#endif
        }
        else{
            GETVERSIONINPARAMS GetVersionParams;
            DWORD cbBytesReturned = 0;

            // Get the version, etc of PhysicalDrive IOCTL
            memset ((void*) & GetVersionParams, 0, sizeof(GetVersionParams));

            if ( ! DeviceIoControl (hPhysicalDriveIOCTL, SMART_GET_VERSION,
                                    NULL,
                                    0,
                                    &GetVersionParams, sizeof (GETVERSIONINPARAMS),
                                    &cbBytesReturned, NULL) ){
#ifdef PRINTING_TO_CONSOLE_ALLOWED
                if (PRINT_DEBUG){
                    DWORD err = GetLastError ();
                    printf ("\n%d ReadPhysicalDriveInNTUsingSmart ERROR"
                            "\nDeviceIoControl(%d, SMART_GET_VERSION) returned 0, error is %d\n",
                            __LINE__, (int) hPhysicalDriveIOCTL, (int) err);
                }
#endif
            }
            else{
                // Print the SMART version
                // PrintVersion (& GetVersionParams);
                // Allocate the command buffer
                ULONG CommandSize = sizeof(SENDCMDINPARAMS) + IDENTIFY_BUFFER_SIZE;
                PSENDCMDINPARAMS Command = (PSENDCMDINPARAMS) malloc (CommandSize);
                // Retrieve the IDENTIFY data
                // Prepare the command
#define ID_CMD          0xEC            // Returns ID sector for ATA
                Command -> irDriveRegs.bCommandReg = ID_CMD;
                DWORD BytesReturned = 0;
                if ( ! DeviceIoControl (hPhysicalDriveIOCTL,
                                        SMART_RCV_DRIVE_DATA, Command, sizeof(SENDCMDINPARAMS),
                                        Command, CommandSize,
                                        &BytesReturned, NULL) ){
                    // Print the error
                    //PrintError ("SMART_RCV_DRIVE_DATA IOCTL", GetLastError());
                }
                else{
                    // Print the IDENTIFY data
                    DWORD diskdata [256];
                    USHORT *pIdSector = (USHORT *)
                            (PIDENTIFY_DATA) ((PSENDCMDOUTPARAMS) Command) -> bBuffer;

                    for (int ijk = 0; ijk < 256; ijk++)
                        diskdata [ijk] = pIdSector [ijk];

                    //PrintIdeInfo (drive, diskdata);

                    //drive model number +Drive serial number
                    char serialNumber [1024]={0};
                    char modelNumber [1024]={0};
                    char revisionNumber [1024]={0};

                    ConvertToString (diskdata, 10, 19, serialNumber);
                    ConvertToString (diskdata, 27, 46, modelNumber);
                    ConvertToString (diskdata, 23, 26, revisionNumber);

                    std::string tmp(modelNumber);
                    strcat(resbuf,trim(tmp).c_str());
                    strcat(resbuf,"-");
                    tmp=serialNumber;
                    strcat(resbuf,trim(tmp).c_str());
                    done = TRUE;

                    listRes.push_back(resbuf);
                    ZeroMemory(resbuf,100);
                    //AfxMessageBox(_T("yes xp"));
                    //USES_CONVERSION;
                    //AfxMessageBox(A2T(resbuf));

                }
                // Done
                CloseHandle (hPhysicalDriveIOCTL);
                free (Command);
            }
        }
    }

    return done;
}
BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];

BOOL DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
                 PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
                 PDWORD lpcbBytesReturned)
{
    // Set up data structures for IDENTIFY command.
    pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
    pSCIP -> irDriveRegs.bFeaturesReg = 0;
    pSCIP -> irDriveRegs.bSectorCountReg = 1;
    //pSCIP -> irDriveRegs.bSectorNumberReg = 1;
    pSCIP -> irDriveRegs.bCylLowReg = 0;
    pSCIP -> irDriveRegs.bCylHighReg = 0;

    // Compute the drive number.
    pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

    // The command can either be IDE identify or ATAPI identify.
    pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
    pSCIP -> bDriveNumber = bDriveNum;
    pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;

    return ( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
                              (LPVOID) pSCIP,
                              sizeof(SENDCMDINPARAMS) - 1,
                              (LPVOID) pSCOP,
                              sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
                              lpcbBytesReturned, NULL) );
}
#endif

//#ifdef Q_OS_LINUX

////1.grub 所有硬盘分区都是 hd 的，而不是 sd，在 linux 下才有 hda 与 sda 的区分
////2.sda是scsi,SATA硬盘设备，hda是IDE硬盘设备
////3.root (hd0,1)这里的hd0是GRUB里硬盘命名的规则
////sda1,sdb2之类的是Linux下的命名规则，不是一个体系的
////4.U盘也是属于scsi，所以U盘也是sda
////5.HDA是IDE硬盘的名称.SDA是SATA的名称
////sda是scsi设备，如果是在vmware里面的话，Vmware 会把虚拟分区虚拟做成scsi设备


//int ReadPhysicalDriveInLinux (std::list<std::string> &listRes)
//{
//    int done = -1;
//    //获取硬盘列表
//    //ls /sys/class/block/ | grep "sd|hd"

//            //查询第一个
//            static struct hd_driveid hd;
//    char resbuf[100]={0};
//    int fd;

//    if (geteuid() >  0)
//    {
//        printf("ERROR: Must be root to use\n");
//    }
//    else
//    {
//        if ((fd = open(hdName, O_RDONLY|O_NONBLOCK)) < 0)
//        {
//            printf("ERROR: Cannot open device %s\n", argv[1]);
//        }
//        else
//        {
//            if (!ioctl(fd, HDIO_GET_IDENTITY, &hd))
//            {
//                strcat(resbuf,hd.model);
//                strcat(resbuf,"-");
//                strcat(resbuf,hd.serial_no);

//                listRes.push_back(resbuf);
//                ZeroMemory(resbuf,100);
//                done = 0;

//            }
//            else if (errno == -ENOMSG)
//            {
//                printf("No hard disk identification information available\n");
//            }
//            else
//            {
//                perror("ERROR: HDIO_GET_IDENTITY");

//            }
//        }

//    }
//    return done;

//}
//#endif

#ifdef Q_OS_WIN32
BOOL ReadPhysicalDriveInNTWithAdminRights (std::list<std::string> &listRes)
{
    BOOL done = FALSE;
    int drive = 0;
    char resbuf[100]={0};
    for (drive = 0; drive < MAX_IDE_DRIVES; drive++){
        HANDLE hPhysicalDriveIOCTL = 0;

        //  Try to get a handle to PhysicalDrive IOCTL, report failure
        //  and exit if can't.
        char driveName [256];
        sprintf(driveName,"\\\\.\\PhysicalDrive%d",drive);

        //  Windows NT, Windows 2000, must have admin rights
        hPhysicalDriveIOCTL = CreateFileA (driveName,
                                           GENERIC_READ | GENERIC_WRITE,
                                           FILE_SHARE_READ | FILE_SHARE_WRITE , NULL,
                                           OPEN_EXISTING, 0, NULL);
        // if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
        //    printf ("Unable to open physical drive %d, error code: 0x%lX\n",
        //            drive, GetLastError ());

        if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE){

        }
        else{
            GETVERSIONOUTPARAMS VersionParams;
            DWORD               cbBytesReturned = 0;

            // Get the version, etc of PhysicalDrive IOCTL
            memset ((void*) &VersionParams, 0, sizeof(VersionParams));

            if ( ! DeviceIoControl (hPhysicalDriveIOCTL, DFP_GET_VERSION,
                                    NULL,
                                    0,
                                    &VersionParams,
                                    sizeof(VersionParams),
                                    &cbBytesReturned, NULL) ){

            }

            // If there is a IDE device at number "i" issue commands
            // to the device
            if (VersionParams.bIDEDeviceMap <= 0){

            }
            else{
                BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
                SENDCMDINPARAMS  scip;
                //SENDCMDOUTPARAMS OutCmd;

                // Now, get the ID sector for all IDE devices in the system.
                // If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
                // otherwise use the IDE_ATA_IDENTIFY command
                bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? \
                            IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

                memset (&scip, 0, sizeof(scip));
                memset (IdOutCmd, 0, sizeof(IdOutCmd));

                if ( DoIDENTIFY (hPhysicalDriveIOCTL,
                                 (PSENDCMDINPARAMS)(&scip),
                                 (PSENDCMDOUTPARAMS)&IdOutCmd,
                                 (BYTE) bIDCmd,
                                 (BYTE) drive,
                                 &cbBytesReturned)){
                    DWORD diskdata [256];
                    int ijk = 0;
                    USHORT *pIdSector = (USHORT *)
                            ((PSENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;

                    for (ijk = 0; ijk < 256; ijk++)
                        diskdata [ijk] = pIdSector [ijk];

                    //PrintIdeInfo (drive, diskdata);
                    //drive model number +Drive serial number
                    char serialNumber [1024]={0};
                    char modelNumber [1024]={0};

                    //  copy the hard drive serial number to the buffer
                    ConvertToString (diskdata, 10, 19, serialNumber);
                    ConvertToString (diskdata, 27, 46, modelNumber);
                    //ConvertToString (diskdata, 23, 26, revisionNumber);

                    std::string tmp(modelNumber);
                    strcat(resbuf,trim(tmp).c_str());
                    strcat(resbuf,"-");
                    tmp=serialNumber;
                    strcat(resbuf,trim(tmp).c_str());

                    listRes.push_back(resbuf);
                    ZeroMemory(resbuf,100);
                    done = TRUE;

                }
            }

            CloseHandle (hPhysicalDriveIOCTL);
        }
    }

    return done;
}


char * flipAndCodeBytes (const char * str,int pos,int flip,char * buf)
{
    int i;
    int j = 0;
    int k = 0;

    buf [0] = '\0';
    if (pos <= 0)
        return buf;

    if ( ! j){
        char p = 0;

        // First try to gather all characters representing hex digits only.
        j = 1;
        k = 0;
        buf[k] = 0;
        for (i = pos; j && str[i] != '\0'; ++i){
            char c = tolower(str[i]);

            if (isspace(c))
                c = '0';

            ++p;
            buf[k] <<= 4;

            if (c >= '0' && c <= '9')
                buf[k] |= (unsigned char) (c - '0');
            else if (c >= 'a' && c <= 'f')
                buf[k] |= (unsigned char) (c - 'a' + 10);
            else{
                j = 0;
                break;
            }

            if (p == 2){
                if (buf[k] != '\0' && ! isprint((unsigned char) buf[k])){
                    j = 0;
                    break;
                }
                ++k;
                p = 0;
                buf[k] = 0;
            }

        }
    }

    if ( ! j)
    {
        // There are non-digit characters, gather them as is.
        j = 1;
        k = 0;
        for (i = pos; j && str[i] != '\0'; ++i){
            unsigned char c = (unsigned char) str[i];

            if ( ! isprint(c)){
                j = 0;
                break;
            }

            buf[k++] = c;
        }
    }

    if ( ! j){
        // The characters are not there or are not printable.
        k = 0;
    }

    buf[k] = '\0';

    if (flip)
        // Flip adjacent characters
        for (j = 0; j < k; j += 2){
            char t = buf[j];
            buf[j] = buf[j + 1];
            buf[j + 1] = t;
        }

    // Trim any beginning and end space
    i = j = -1;
    for (k = 0; buf[k] != '\0'; ++k){
        if (! isspace(buf[k])){
            if (i < 0)
                i = k;
            j = k;
        }
    }

    if ((i >= 0) && (j >= 0)){
        for (k = i; (k <= j) && (buf[k] != '\0'); ++k)
            buf[k - i] = buf[k];
        buf[k - i] = '\0';
    }

    return buf;
}



BOOL ReadPhysicalDriveInNTWithZeroRights (std::list<std::string> &listRes)
{
    char resbuf[100]={0};
    BOOL done = FALSE;
    int drive = 0;

    for (drive = 0; drive < MAX_IDE_DRIVES; drive++){
        HANDLE hPhysicalDriveIOCTL = 0;

        //  Try to get a handle to PhysicalDrive IOCTL, report failure
        //  and exit if can't.
        char driveName [256];

        sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

        //  Windows NT, Windows 2000, Windows XP - admin rights not required
        hPhysicalDriveIOCTL = CreateFileA (driveName, 0,
                                           FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                           OPEN_EXISTING, 0, NULL);
        if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE){

        }
        else{
            STORAGE_PROPERTY_QUERY query;
            DWORD cbBytesReturned = 0;
            char buffer [10000];

            memset ((void *) & query, 0, sizeof (query));
            query.PropertyId = StorageDeviceProperty;
            query.QueryType = PropertyStandardQuery;

            memset (buffer, 0, sizeof (buffer));

            if ( DeviceIoControl (hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,
                                  & query,
                                  sizeof (query),
                                  & buffer,
                                  sizeof (buffer),
                                  & cbBytesReturned, NULL) ){
                STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *) & buffer;
                char serialNumber [1000];
                char modelNumber [1000];
                char vendorId [1000];
                char productRevision [1000];

                flipAndCodeBytes (buffer,
                                  descrip -> VendorIdOffset,
                                  0, vendorId );
                flipAndCodeBytes (buffer,
                                  descrip -> ProductIdOffset,
                                  0, modelNumber );
                flipAndCodeBytes (buffer,
                                  descrip -> ProductRevisionOffset,
                                  0, productRevision );
                flipAndCodeBytes (buffer,
                                  descrip -> SerialNumberOffset,
                                  1, serialNumber );

                if (0 == HardDriveSerialNumber [0] &&
                        //  serial number must be alphanumeric
                        //  (but there can be leading spaces on IBM drives)
                        (isalnum (serialNumber [0]) || isalnum (serialNumber [19]))){
                    strcpy (HardDriveSerialNumber, serialNumber);
                    strcpy (HardDriveModelNumber, modelNumber);
                    done = TRUE;
                    //VENDORID ----product id -------serial number

                    std::string tmp(vendorId);
                    strcat(resbuf,trim(tmp).c_str());
                    tmp=modelNumber;
                    strcat(resbuf,trim(tmp).c_str());
                    strcat(resbuf,"-");
                    tmp=serialNumber;
                    strcat(resbuf,trim(tmp).c_str());
                    listRes.push_back(resbuf);
                    ZeroMemory(resbuf,100);
                    //AfxMessageBox(_T("yes zero right"));
                    //USES_CONVERSION;
                    //AfxMessageBox(A2T(resbuf));
                    //break;
                }
            }
            else{
                //DWORD err = GetLastError ();
            }

            CloseHandle (hPhysicalDriveIOCTL);
        }
    }


    return done;
}


BOOL ReadIdeDriveAsScsiDriveInNT(std::list<std::string> &listRes)
{
    BOOL done = FALSE;
    int controller = 0;
    char resbuf[100]={0};
    for (controller = 0; controller < 16; controller++){
        HANDLE hScsiDriveIOCTL = 0;
        char   driveName [256];

        //  Try to get a handle to PhysicalDrive IOCTL, report failure
        //  and exit if can't.
        sprintf (driveName, "\\\\.\\Scsi%d:", controller);

        //  Windows NT, Windows 2000, any rights should do
        hScsiDriveIOCTL = CreateFileA (driveName,
                                       GENERIC_READ | GENERIC_WRITE,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                       OPEN_EXISTING, 0, NULL);

        if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE){
            int drive = 0;

            for (drive = 0; drive < 2; drive++){
                char buffer [sizeof (SRB_IO_CONTROL) + SENDIDLENGTH];
                SRB_IO_CONTROL *p = (SRB_IO_CONTROL *) buffer;
                SENDCMDINPARAMS *pin =
                        (SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
                DWORD dummy;

                memset (buffer, 0, sizeof (buffer));
                p -> HeaderLength = sizeof (SRB_IO_CONTROL);
                p -> Timeout = 10000;
                p -> Length = SENDIDLENGTH;
                p -> ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
                strncpy ((char *) p -> Signature, "SCSIDISK", 8);

                pin -> irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
                pin -> bDriveNumber = drive;

                if (DeviceIoControl (hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT,
                                     buffer,
                                     sizeof (SRB_IO_CONTROL) +
                                     sizeof (SENDCMDINPARAMS) - 1,
                                     buffer,
                                     sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,
                                     &dummy, NULL)){
                    SENDCMDOUTPARAMS *pOut =
                            (SENDCMDOUTPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
                    IDSECTOR *pId = (IDSECTOR *) (pOut -> bBuffer);
                    if (pId -> sModelNumber [0])
                    {
                        DWORD diskdata [256];
                        int ijk = 0;
                        USHORT *pIdSector = (USHORT *) pId;

                        for (ijk = 0; ijk < 256; ijk++)
                            diskdata [ijk] = pIdSector [ijk];

                        //PrintIdeInfo (controller * 2 + drive, diskdata);
                        char serialNumber [1024]={0};
                        char modelNumber [1024]={0};
                        char revisionNumber [1024]={0};
                        //  copy the hard drive serial number to the buffer
                        ConvertToString (diskdata, 10, 19, serialNumber);
                        ConvertToString (diskdata, 27, 46, modelNumber);
                        ConvertToString (diskdata, 23, 26, revisionNumber);

                        std::string tmp(modelNumber);
                        strcat(resbuf,trim(tmp).c_str());
                        strcat(resbuf,"-");
                        tmp=serialNumber;
                        strcat(resbuf,trim(tmp).c_str());

                        listRes.push_back(resbuf);
                        ZeroMemory(resbuf,100);
                        //AfxMessageBox(_T("yes Scsi"));
                        //USES_CONVERSION;
                        //AfxMessageBox(A2T(resbuf));
                        done = TRUE;

                    }
                }
            }
            CloseHandle (hScsiDriveIOCTL);
        }
    }

    return done;
}
#endif

BOOL getHardDriveComputerID(char *buffer)
{
    BOOL done = FALSE;

    strcpy (HardDriveSerialNumber, "");
#ifdef Q_OS_WIN32
    OSVERSIONINFO version;
    memset (&version, 0, sizeof (version));
    version.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    GetVersionEx (&version);
#endif

    //从配置文件中读取DID
#ifdef Q_OS_WIN32
    std::string szMacAddressKeepFilePath ="C:\\Program Files (x86)\\SRC\\Product\\Cfg\\UserCfg\\";
#endif

#ifdef Q_OS_LINUX
    std::string szMacAddressKeepFilePath ="./";
#endif

#ifdef Q_OS_MACOS
    std::string szMacAddressKeepFilePath ="./";
#endif
    szMacAddressKeepFilePath += "Qmxing40.Ini";

    QString  szUsedID;
    //GetPrivateProfileStringA("IDENTIFY", "DID", "", szUsedID, 200, szMacAddressKeepFilePath.c_str());
    QString sIniFilePath   =QString::fromStdString(szMacAddressKeepFilePath);
    QSettings *sIniSetting = new QSettings(sIniFilePath, QSettings::IniFormat);
    sIniSetting->setIniCodec("UTF-8");
    szUsedID = sIniSetting->value("IDENTIFY/DID",QString("null")).toString();

    std::string  strIDFromFile= szUsedID.toStdString();
    strIDFromFile = trim(strIDFromFile);
    std::string  strWillUse;

    std::list<std::string> listRes;
#ifdef Q_OS_WIN32
    if (version.dwPlatformId == VER_PLATFORM_WIN32_NT){ //windows NT 及以上版本
        //this works under WinNT4 or Win2K if you have admin rights
        done = ReadPhysicalDriveInNTWithAdminRights(listRes);
        // this works under WinNT4 or Win2K or WinXP if you have any rights
        if (!done)
            done = ReadPhysicalDriveInNTWithZeroRights(listRes);
        //  this should work in WinNT or Win2K if previous did not work
        //  this is kind of a backdoor via the SCSI mini port driver into
        //  the IDE drives
        if ( ! done)
            done = ReadIdeDriveAsScsiDriveInNT (listRes);

        //this works under WinNT4 or Win2K or WinXP or Windows Server 2003 or Vista if you have any rights
        if ( ! done)
            done = ReadPhysicalDriveInNTUsingSmart(listRes);
    }
#endif
    /*
    //#ifdef Q_OS_LINUX
    //1.grub 所有硬盘分区都是 hd 的，而不是 sd，在 linux 下才有 hda 与 sda 的区分
    //2.sda是scsi,SATA硬盘设备，hda是IDE硬盘设备
    //3.root (hd0,1)这里的hd0是GRUB里硬盘命名的规则
    //sda1,sdb2之类的是Linux下的命名规则，不是一个体系的
    //4.U盘也是属于scsi，所以U盘也是sda
    //5.HDA是IDE硬盘的名称.SDA是SATA的名称
    //sda是scsi设备，如果是在vmware里面的话，Vmware 会把虚拟分区虚拟做成scsi设备
    //ReadPhysicalDriveInLinux(listRes);

    因linux 权限及处理设备不同接口较复杂，本函数getHardDriveComputerID 直接返回 done = FALSE，无需处理，使用mac地址
#endif
    */
    if(listRes.size()>0){//主要针对多硬盘

        if(strIDFromFile.length()>0){ //如果有保存，则把相同的那个ID返回。如果没有相同的，则返回第一个得了

            bool bHasSame=false;
            list <std::string>::iterator citer;
            for(citer = listRes.begin(); citer != listRes.end( ); citer++ ){
                if(strIDFromFile == *citer){
                    bHasSame=true;
                    strWillUse=strIDFromFile;
                    break;
                }
            }
            if (!bHasSame){//如果没有相同的，那就使用第一个得了.注意:如果第一个是固态硬盘,或许取不到,一般长度小于7.或者存在?,跳过它

                for(citer = listRes.begin(); citer != listRes.end( ); citer++ ){
                    if((*citer).length()>6 && ((*citer).find('?') == std::string::npos)){
                        strWillUse=*citer;
                        break;
                    }
                }
            }
        }
        else { //如果还没有保存，那就使用第一个得了.注意:如果第一个是固态硬盘,或许取不到,一般长度小于7.跳过它

            list <std::string>::iterator citer;
            for(citer = listRes.begin(); citer != listRes.end( ); citer++ ){
                if(((*citer).length()>6) && ((*citer).find('?') == std::string::npos)){
                    strWillUse=*citer;
                    break;
                }
            }
        }
    }
    done=FALSE;
    if(strWillUse.length()>0){
        strcpy(buffer,strWillUse.c_str());
        done=TRUE;
    }
    if(strIDFromFile!=strWillUse){
        //wchar_t *tmp=CBaseUtility::GB2312ToUnicode(strWillUse);
        //WritePrivateProfileStringA("IDENTIFY", "DID",  tmp, szMacAddressKeepFilePath.c_str());
        QSettings *configIniWrite = new QSettings(szMacAddressKeepFilePath.c_str(), QSettings::IniFormat);
        configIniWrite->setValue("/IDENTIFY/DID", strWillUse.c_str());
        delete configIniWrite;

    }
    return done;
}




E_HardwareType	GetDiskId(char*	szBuffer)
{
    std::string szMacAddressKeepFilePath;
    char tmpBuffer[300]={0};
    E_HardwareType	nResult =E_HardwareType::HT_UNKNOWN;
#ifdef Q_OS_WIN32
    //获取系统版本
    OSVERSIONINFO VersionInfo;
    ZeroMemory(&VersionInfo,sizeof(VersionInfo));
    VersionInfo.dwOSVersionInfoSize=sizeof(VersionInfo);
    GetVersionEx(&VersionInfo);
#endif

    //读取配置文件 OM
#ifdef Q_OS_WIN32 //tmp test
    szMacAddressKeepFilePath ="C:\\Program Files (x86)\\SRC\\Product\\Cfg\\UserCfg\\";
#endif
#ifdef LINUX
    szMacAddressKeepFilePath ="./"; //tmp test
#endif
    szMacAddressKeepFilePath += "Qmxing40.Ini";
    int om=0;
    //om=GetPrivateProfileIntA("IDENTIFY", "OM", 0, szMacAddressKeepFilePath.c_str());
    QString sIniFilePath   =QString::fromStdString(szMacAddressKeepFilePath);
    QSettings *sIniSetting = new QSettings(sIniFilePath, QSettings::IniFormat);
    sIniSetting->setIniCodec("UTF-8");
    om = sIniSetting->value("IDENTIFY/OM",QString("0")).toInt();


    BOOL bResult = FALSE;
    if(0==om){//om为1就不读硬盘号了。如果指定不读硬盘，就不读了。主要针对少数读不出硬盘ID的盘

        memset(tmpBuffer,0,300);

        if (getHardDriveComputerID(tmpBuffer)){

            bResult=TRUE;
            nResult = E_HardwareType::HT_HARD_DISK_NEW;
            strcpy(szBuffer,tmpBuffer);
        }
    }
    if( ! bResult ){//读取硬盘失败，改读网卡

        char szUsedMac[200]={0};
        nResult = E_HardwareType::HT_UNKNOWN;
        QList<QString> macAddrList;
        if( GetMacAddress(macAddrList)>0 ){
            nResult = E_HardwareType::HT_NET_CARD; //成功读取

            QString sIniFilePath   =QString::fromStdString(szMacAddressKeepFilePath);
            QSettings *sIniSetting = new QSettings(sIniFilePath, QSettings::IniFormat);
            sIniSetting->setIniCodec("UTF-8");
            QString qszUsedMac = sIniSetting->value("IDENTIFY/MACID",QString("null")).toString().toUpper();
            int index = 0;
            for ( index = 0; index !=macAddrList.size(); ++index ){
                if ( macAddrList.at(index) == qszUsedMac ){
                    strcpy(tmpBuffer, macAddrList[index].toStdString().c_str());
                    break;
                }
            }
            if (index >= macAddrList.size() ){//和原来保存的不一致 ,保存第一个

                strcpy(tmpBuffer, macAddrList[0].toStdString().c_str());
            }
        }
        else{
            memset(tmpBuffer,0,300);//失败读取
            nResult = E_HardwareType::HT_UNKNOWN;
        }
        strcpy(szUsedMac, tmpBuffer);
        strcpy(szBuffer,tmpBuffer);
        QSettings *configIniWrite = new QSettings(szMacAddressKeepFilePath.c_str(), QSettings::IniFormat);
        configIniWrite->setValue("/IDENTIFY/MACID", szUsedMac);
        delete configIniWrite;

    }
    return	nResult;
}
