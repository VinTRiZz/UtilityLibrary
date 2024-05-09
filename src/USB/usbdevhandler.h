#ifndef USBDEVHANDLER_H
#define USBDEVHANDLER_H

/*
Typical port names:

/dev/ttyACM0 - ACM stands for the ACM modem on the USB bus. Arduino UNOs (and similar) will appear using this name.

/dev/ttyS0 - Standard COM ports will have this name. These are less common these days with newer desktops and laptops not having actual COM ports.

/dev/ttyUSB0 - Most USB-to-serial cables will show up using a file named like this.

*********************************
BEFORE USING SETTINGS
sudo adduser $USER dialout
*/

#define USBDEV_SUCCESS 0
#define USBDEV_ERROR 1
#define USBDEV_OPEN_PORT_ERROR 2

#define DEFAULT_BUFFER_SIZE 64


// Message history handling
#include <queue>
#include <string>

// File control
#include <fcntl.h>

// POSIX terminal control
#include <termios.h>

// write, read and close port
#include <unistd.h>

namespace USB
{
    /** @brief Structure to storage settings in more capable format
     */
    struct tty_config
    {
        bool enable_parity                      = 0;
        bool enable_two_stop_bits               = 0;
        bool enable_hardware_flow_control       = 0;
        bool enable_specific_lines              = 0;
        bool enable_read_data                   = 1;
        bool enable_canonical_mode              = 0;
        bool enable_basic_echo                  = 0;
        bool enable_echo_erasure                = 0;
        bool enable_new_line_echo               = 0;
        bool enable_signal_chars                = 0;
        bool enable_software_flow_control       = 0;
        bool enable_special_byte_handle         = 0;
        bool enable_special_input_interpret     = 0;
        bool enable_conversion_of_new_line      = 0;
        bool enable_conversion_tab_to_space     = 0;
        bool enable_removal_c_d_chars           = 0;
        int  bit_per_byte                       = 8;
        int  set_vmin                           = 10;
        int  set_vtime                          = 1;
        int  set_i_baud_rate                    = 9600;
        int  set_o_baud_rate                    = 9600;
    };

    /** @brief Handles USB or ACM connection
     */
    class UsbDevHandler
    {
        public:
            UsbDevHandler() = default;
            ~UsbDevHandler();

            /// ----------------------------- ///
            ///        ACM FUNCTIONS          ///
            /// ----------------------------- ///

            /// ATTENTION!
            /// NOT TESTED!
            int8_t init_ACM(const int8_t port, tty_config & configs);
            int8_t init_default_ACM(const int8_t port);
            int8_t getMessage_ACM(std::string & msg);
            int8_t sendMessage_ACM(char * msg);

            /// ----------------------------- ///
            ///        USB FUNCTIONS          ///
            /// ----------------------------- ///

            int8_t init_USB(const int8_t port, tty_config & configs);
            int8_t init_default_USB(const int8_t port);
            int8_t getMessage_USB(std::string & msg);
            int8_t sendMessage_USB(char * msg);

        private:
            std::queue<std::string> m_ACM_packetsReceived; ///< Queue to store messages got

            std::queue<std::string> m_USB_packetsReceived; ///< Queue to store messages got

            // Create new termios struct, we call it 'tty' for convention
            // No need for "= {0}" at the end as we'll immediately write the existing
            // config to this struct
            struct termios tty_ACM; // For msg handling with ACM
            struct termios tty_USB;  // For msg handling with USB

            int8_t setup_tty(termios& tty, tty_config & configs, char port, char port_type);
            void clearBuffer(char * buffer);

            /// ----------------------------- ///
            ///          ACM THINGS           ///
            /// ----------------------------- ///
            int m_ACM_serial_port = 0; ///< ACM port file descriptor

            char * m_ACM_readBuf = nullptr; ///< ACM port read buffer
            int8_t usb_ACM_configure_default(char port);
            int8_t usb_ACM_configure(char port);
            int8_t usb_ACM_open_port(char port);
            int8_t usb_ACM_read();
            int8_t usb_ACM_write(char * msg);
            void usb_close_ACM_port();


            /// ----------------------------- ///
            ///          USB THINGS           ///
            /// ----------------------------- ///
            int m_USB_serial_port = 0; ///< USB port file descriptor

            char * m_USB_readBuf = nullptr; ///< USB port reading buffer

            int8_t usb_USB_configure_default(char port);
            int8_t usb_USB_open_port(char port);
            int8_t usb_USB_write(char * msg);
            int8_t usb_USB_read();
            void usb_close_USB_port();
    };
}

#endif // USBDEVHANDLER_H
