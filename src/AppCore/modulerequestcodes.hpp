#ifndef MODULE_REQUEST_CODES_HPP
#define MODULE_REQUEST_CODES_HPP

#include <string>

namespace RequestCommandCodes
{
    namespace OS
    {
        const std::string
            OS_REBOOT              = "reboot",
            OS_POWEROFF            = "poweroff"
        ;
    }

    namespace Mining
    {
        const std::string
            MINING_START                = "start",
            MINING_STOP                 = "stop",
            MINING_SET_GPU_SETTINGS     = "setGpusSettings"
        ;
    }

    namespace GPU
    {
        const std::string
            GPU_SETTINGS        = "settings"
        ;
    }
}

namespace RequestDataCodes
{
    namespace Minux
    {
        const std::string
            MINUX_SUPPORTED_ALGORITHMS    = "algorithms",
            MINUX_SYSTEM_INFORMATION      = "system"
        ;
    }

    namespace Mining
    {
        const std::string
            MINING_WORKING_STATISTIC             = "miningStatistic",
            MINING_WORKING_GPU_MINING_SETTINGS   = "miningGpuSettings",
            MINING_GPU_SETTINGS                  = "miningGpuSettings"
        ;
    }


    namespace Common
    {
        const std::string
            ALL                 = "all",
            MANUFACTURER        = "manufacturer",
            SERIAL_NUMBER       = "serialnumber",
            PRODUCT_NAME        = "productname"
        ;
    }


    namespace Motherboard
    {
        const std::string 
            MOTHERBOARD_SATA_COUNT          = "satacount",
            MOTHERBOARD_PCI_COUNT           = "pcicount",
            MOTHERBOARD_RAM_MAX_SPEED       = "rammaxspeed",
            MOTHERBOARD_RAM_MAX_CAPACITY    = "rammaxcapacity"
        ;
    }


    namespace CPU
    {
        const std::string
            CPU_ARCHITECTURE            = "architecture",
            CPU_OPERATION_MODE          = "opmode",
            CPU_CORE_COUNT              = "corecount",
            CPU_THREADS_PER_CORE        = "threadspercore",
            CPU_THREADS_PER_SOCKET      = "threadspersocket",
            CPU_SOCKET_COUNT            = "socketcount",
            CPU_CLOCK_MAX               = "clockmax",
            CPU_CLOCK_MIN               = "clockmin"
        ;
    }


    namespace RAM
    {
        const std::string
            RAM_FORM_FACTOR         = "formfactor",
            RAM_TYPE                = "type",
            RAM_SPEED               = "speed",
            RAM_CONFIGURED_SPEED    = "configuredspeed",
            RAM_CONFIGURED_VOLTAGE  = "configuredvoltage",
            RAM_SIZE                = "size",
            RAM_TOTAL_WIDTH         = "totalwidth",
            RAM_DATA_WIDTH          = "datawidth",
            RAM_TOTAL               = "total",
            RAM_FREE                = "free",
            RAM_USAGE               = "usage"
        ;
    }


    namespace Drive
    {
        const std::string
            DRIVE_SPACE_TOTAL           = "spacetotal",
            DRIVE_SPACE_FREE            = "spacefree",
            DRIVE_SPACE_AVAILABLE       = "spaceavailable",
            DRIVE_SPACE_USED            = "spaceused",
            DRIVE_TEMPERATURE_CURRENT   = "temperaturecurrent"
        ;
    }

    namespace GPU
    {
        const std::string
            GPU_WORKING         = "working",
            GPU_SETTINGS        = "settings",
            GPU_DESCRIBE        = "describeGpu" // TODO: Remove this
        ;
    }
}

#endif // MODULE_REQUEST_CODES_HPP
