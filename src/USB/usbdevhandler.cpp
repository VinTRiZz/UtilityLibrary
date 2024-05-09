#include "./usbdevhandler.h"

/******************************************************/
/******************************************************/
/**************** User interface **********************/

USB::UsbDevHandler::~UsbDevHandler()
{
    if (m_ACM_readBuf)
        delete[] m_ACM_readBuf;

    if (m_USB_readBuf)
        delete[] m_USB_readBuf;

    usb_close_ACM_port();
    usb_close_USB_port();
}


int8_t USB::UsbDevHandler::init_ACM(const int8_t port, tty_config & configs)
{
    usb_close_ACM_port();

    int8_t result = USBDEV_SUCCESS;

    std::string port_str = std::to_string(port);

    result = setup_tty(tty_ACM, configs, port_str[0], 'A');

    if (result != USBDEV_SUCCESS)
    {
        return result;
    }
    else
    {
        if (m_ACM_readBuf)
            delete[] m_ACM_readBuf;

        m_ACM_readBuf = new char(DEFAULT_BUFFER_SIZE);
    }
    return USBDEV_SUCCESS;
}


int8_t USB::UsbDevHandler::init_default_ACM(const int8_t port)
{
    usb_close_ACM_port();

    int8_t result = USBDEV_SUCCESS;

    std::string port_str = std::to_string(port);

    result = usb_ACM_configure_default(port_str[0]);

    if (result != USBDEV_SUCCESS)
    {
        return result;
    }
    else
    {
        if (m_ACM_readBuf)
            delete[] m_ACM_readBuf;

        m_ACM_readBuf = new char(DEFAULT_BUFFER_SIZE);
    }
    return USBDEV_SUCCESS;
}


int8_t USB::UsbDevHandler::getMessage_ACM(std::string & msg)
{
    clearBuffer(m_ACM_readBuf);

    if (usb_ACM_read() > 0)
        msg = m_ACM_packetsReceived.front();
    else
        return USBDEV_ERROR;

    return USBDEV_SUCCESS;
}


int8_t USB::UsbDevHandler::sendMessage_ACM(char * msg)
{
    return usb_ACM_write(msg);
}


int8_t USB::UsbDevHandler::init_USB(const int8_t port, tty_config & configs)
{
    usb_close_USB_port();

    int8_t result = USBDEV_SUCCESS;

    std::string port_str = std::to_string(port);

    result = setup_tty(tty_USB, configs, port_str[0], 'U');

    if (result != USBDEV_SUCCESS)
    {
        return result;
    }
    else
    {
        if (m_USB_readBuf)
            delete[] m_USB_readBuf;

        m_USB_readBuf = new char(DEFAULT_BUFFER_SIZE);
    }
    return USBDEV_SUCCESS;
}


int8_t USB::UsbDevHandler::init_default_USB(const int8_t port)
{
    usb_close_USB_port();

    int8_t result = USBDEV_SUCCESS;

    std::string port_str = std::to_string(port);

    result = usb_USB_configure_default(port_str[0]);

    if (result != USBDEV_SUCCESS)
    {
        return result;
    }
    else
    {
        if (m_USB_readBuf)
            delete[] m_USB_readBuf;

        m_USB_readBuf = new char(DEFAULT_BUFFER_SIZE);
    }
    return USBDEV_SUCCESS;
}


int8_t USB::UsbDevHandler::getMessage_USB(std::string & msg)
{
    clearBuffer(m_USB_readBuf);

    if (usb_USB_read() > 0)
        msg = m_USB_packetsReceived.front();
    else
        return USBDEV_ERROR;

    return USBDEV_SUCCESS;
}


int8_t USB::UsbDevHandler::sendMessage_USB(char * msg)
{
    return usb_USB_write(msg);
}










/******************************************************/
/******************************************************/
/************************ System **********************/

// Configuring tty_ACM
int8_t USB::UsbDevHandler::usb_ACM_configure_default(char port)
{
    // If port is already init, return
    if (usb_ACM_open_port(port) >= 0)
    {
        if(tcgetattr(m_ACM_serial_port, &tty_ACM) != 0)
        {
            return USBDEV_ERROR;
        }
    }
    else
    {
        return USBDEV_OPEN_PORT_ERROR;
    }

    // Set I-O baud rate to 9600
    cfsetispeed(&tty_ACM, B9600);
    cfsetospeed(&tty_ACM, B9600);

    // Disable parity
    tty_ACM.c_cflag &= ~PARENB;

    // One stop bit
    tty_ACM.c_cflag &= ~CSTOPB;

    // Set data size
    tty_ACM.c_cflag &= ~CSIZE; // Clear size bit
    tty_ACM.c_cflag |= CS8;    // Set data size to 8

    // Disable hardware flow control
    tty_ACM.c_cflag &= ~CRTSCTS;

    // Enable receiver, ignore modem control lines
    tty_ACM.c_cflag |= CREAD | CLOCAL;

    // Disable XON/XOFF flow control both i/p and o/p
    tty_ACM.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Timeouts
    tty_ACM.c_cc[VMIN] = 10;  // Read at least 1 char
    tty_ACM.c_cc[VTIME] = 10; // Wait until get anything

    // Setup configuration
    if ((tcsetattr(m_ACM_serial_port,TCSAFLUSH, &tty_ACM)) != 0)
    {
        return USBDEV_ERROR;
    }

    return USBDEV_SUCCESS;
}

// Open port with type and number
int8_t USB::UsbDevHandler::usb_ACM_open_port(char port)
{
    char buf[] = {
        '/', 'd', 'e', 'v',
        '/', 't', 't', 'y',
        'A', 'C', 'M', port, '\0'
    };

    m_ACM_serial_port = open(buf, O_RDWR | O_NOCTTY | O_SYNC);

    // Check for errors
    if (m_ACM_serial_port < 0)
    {
        return USBDEV_ERROR;
    }

    // Read in existing settings, and handle any error
    // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
    // must have been initialized with a call to tcgetattr() overwise behaviour
    // is undefined
    if(tcsetattr(m_ACM_serial_port, TCSANOW, &tty_ACM) != 0)
        return USBDEV_ERROR;

    // Flush Port, then applies attributes
    tcflush(m_ACM_serial_port, TCIFLUSH);

    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(m_ACM_serial_port, &readfds);

    int flags = fcntl(m_ACM_serial_port, F_GETFL, 0);
    fcntl(m_ACM_serial_port, F_SETFL, flags | O_NONBLOCK);

    return USBDEV_SUCCESS;
}

/*
// R/W to ACM
int8_t USB::UsbDevHandler::usb_ACM_read()
{
    int sum = 0;
    int8_t n = read(m_ACM_serial_port, m_ACM_readBuf, sizeof(m_ACM_readBuf));

    while ((n > 0) && (sum < DEFAULT_BUFFER_SIZE))
    {
        std::cout << m_ACM_readBuf << " || " << (int)n << "\n\n";
        n = read(m_ACM_serial_port, m_ACM_readBuf, sizeof(m_ACM_readBuf));
        sum += n;
    }

    m_ACM_packetsReceived.push(m_USB_readBuf);

    return sum;
}
*/

int8_t USB::UsbDevHandler::usb_ACM_read()
{
    int sum = 0;
    int8_t n;

    n = read(m_ACM_serial_port, m_ACM_readBuf, sizeof(m_ACM_readBuf));
    while ((n > 0) && (sum < DEFAULT_BUFFER_SIZE))
    {
        n = read(m_ACM_serial_port, m_ACM_readBuf, sizeof(m_ACM_readBuf));
        sum += n;
    }

    return sum;
}

int8_t USB::UsbDevHandler::usb_ACM_write(char * msg)
{
    if (write(m_ACM_serial_port, msg, sizeof(msg)) < 0)
        return USBDEV_ERROR;
    else
        return USBDEV_SUCCESS;
}

void USB::UsbDevHandler::usb_close_ACM_port()
{
    close(m_ACM_serial_port);
}







// Configuring tty_USB
int8_t USB::UsbDevHandler::usb_USB_configure_default(char port)
{
    // If port is already init, return
    if (usb_USB_open_port(port) >= 0)
    {
        if(tcgetattr(m_USB_serial_port, &tty_USB) != 0)
        {
            return USBDEV_ERROR;
        }
    }
    else
    {
        return USBDEV_OPEN_PORT_ERROR;
    }

    // Set I-O baud rate to 230400 for really fast communication
    cfsetispeed(&tty_USB, B230400);
    cfsetospeed(&tty_USB, B230400);

    // Disable parity
    tty_USB.c_cflag &= ~PARENB;

    // One stop bit
    tty_USB.c_cflag &= ~CSTOPB;

    // Set data size
    tty_USB.c_cflag &= ~CSIZE; // Clear size bit
    tty_USB.c_cflag |= CS8;    // Set data size to 8

    // Disable hardware flow control
    tty_USB.c_cflag &= ~CRTSCTS;

    // Enable receiver, ignore modem control lines
    tty_USB.c_cflag |= CREAD | CLOCAL;

    // Disable XON/XOFF flow control both i/p and o/p
    tty_USB.c_iflag &= ~(IXON | IXOFF | IXANY);

    // No sense because disabled canonical mode
    // tty_USB.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // No output processing
    // tty_USB.c_oflag &= ~OPOST;

    // Timeouts
    tty_USB.c_cc[VMIN] = 10;  // Read at least 1 char
    tty_USB.c_cc[VTIME] = 1; // Wait until get anything

    // Setup configuration
    if ((tcsetattr(m_USB_serial_port,TCSAFLUSH, &tty_USB)) != 0)
    {
        return USBDEV_ERROR;
    }

    return USBDEV_SUCCESS;
}

// Open port with type and number

int8_t USB::UsbDevHandler::usb_USB_open_port(char port)
{
    char buf[] = {
        '/', 'd', 'e', 'v',
        '/', 't', 't', 'y',
        'U', 'S', 'B', port, '\0'
    };

    m_USB_serial_port = open(buf, O_RDWR | O_NOCTTY | O_SYNC);

    // Check for errors
    if (m_USB_serial_port < 0)
        return USBDEV_ERROR;

    // Read in existing settings, and handle any error
    // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
    // must have been initialized with a call to tcgetattr() overwise behaviour
    // is undefined
    if(tcsetattr(m_USB_serial_port, TCSANOW, &tty_USB) != 0)
        return USBDEV_ERROR;

    // Flush Port, then applies attributes
    tcflush(m_USB_serial_port, TCIFLUSH);

    if ((tcsetattr(m_USB_serial_port, TCSANOW, &tty_USB)) != 0)
        return USBDEV_ERROR;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_USB_serial_port, &readfds);

    int flags = fcntl(m_USB_serial_port, F_GETFL, 0);
    fcntl(m_USB_serial_port, F_SETFL, flags | O_NONBLOCK);

    return USBDEV_SUCCESS;
}

// R/W to USB
int8_t USB::UsbDevHandler::usb_USB_write(char * msg)
{
    if (write(m_USB_serial_port, msg, sizeof(msg)) < 0)
        return USBDEV_ERROR;
    else
        return USBDEV_SUCCESS;
}
int8_t USB::UsbDevHandler::usb_USB_read()
{
    int sum = 0;
    int8_t n = read(m_USB_serial_port, m_USB_readBuf, sizeof(m_USB_readBuf));

    while ((n > 0) && (sum < DEFAULT_BUFFER_SIZE))
    {
        n = read(m_USB_serial_port, m_USB_readBuf, sizeof(m_USB_readBuf));
        sum += n;
    }

    m_USB_packetsReceived.push(m_USB_readBuf);

    return sum;
}

// Close port
void USB::UsbDevHandler::usb_close_USB_port()
{
    close(m_USB_serial_port);
}

// Clear any buffer
void USB::UsbDevHandler::clearBuffer(char * buffer)
{
    for (int i = 0; buffer[i]; i++)
    {
        buffer[i] = '\0';
    }
}




// Setups tty using structure
int8_t USB::UsbDevHandler::setup_tty(termios& tty, tty_config & configs, char port, char port_type)
{
    if (port_type == 'A')
    {
        // If port is already init, return
        if (usb_ACM_open_port(port) >= 0)
        {
            if(tcgetattr(m_ACM_serial_port, &tty_ACM) != 0)
            {
                return USBDEV_ERROR;
            }
        }
        else
        {
            return USBDEV_OPEN_PORT_ERROR;
        }
    }
    else if (port_type == 'U')
    {
        // If port is already init, return
        if (usb_USB_open_port(port) >= 0)
        {
            if(tcgetattr(m_ACM_serial_port, &tty_ACM) != 0)
            {
                return USBDEV_ERROR;
            }
        }
        else
        {
            return USBDEV_OPEN_PORT_ERROR;
        }
    }
    else
    {
        return USBDEV_ERROR;
    }

    // Parity bit (PARENB)
    // If this bit is set, generation and detection of the parity bit is enabled. Most serial communications do not use a parity bit, so if you are unsure, clear this bit.
    if (configs.enable_parity)
    {
        tty.c_cflag |= PARENB;  // Set parity bit, enabling parity
    }
    else
    {
        tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    }


    // Num. stop bits (CSTOPB)
    // If this bit is set, two stop bits are used. If this is cleared, only one stop bit is used. Most serial communications only use one stop bit.
    if (configs.enable_two_stop_bits)
    {
        tty.c_cflag |= CSTOPB;  // Set stop field, two stop bits used in communication
    }
    else
    {
        tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    }

    // Bits per byte
    // The CS<number> fields set how many data bits are transmitted per byte across the serial port. The most common setting here is 8 (CS8).
    // Definitely use this if you are unsure, I have never used a serial port before which didn’t use 8 (but they do exist).
    // You must clear all of the size bits before setting any of them with &= ~CSIZE.
    tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
    switch (configs.bit_per_byte)
    {
        case 5:
        tty.c_cflag |= CS5; // 5 bits per byte
            break;

        case 6:
        tty.c_cflag |= CS6; // 6 bits per byte
            break;

        case 7:
        tty.c_cflag |= CS7; // 7 bits per byte
            break;

        default:
            tty.c_cflag |= CS8; // 8 bits per byte (most common)
            break;
    }


    // Hardware Flow Control (CRTSCTS)
    // If the CRTSCTS field is set, hardware RTS/CTS flow control is enabled. This is when there are two extra wires between the end points,
    // used to signal when data is ready to be sent/received. The most common setting here is to disable it. Enabling this when it should be
    // disabled can result in your serial port receiving no data, as the sender will buffer it indefinitely, waiting for you to be “ready”.
    if (configs.enable_hardware_flow_control)
    {
        tty.c_cflag |= CRTSCTS;  // Enable RTS/CTS hardware flow control
    }
    else
    {
        tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    }


    // CREAD and CLOCAL
    // Setting CLOCAL disables modem-specific signal lines such as carrier detect. It also prevents the controlling process from getting sent
    // a SIGHUP signal when a modem disconnect is detected, which is usually a good thing here. Setting CREAD allows us to read data (we definitely want that!).
    if (configs.enable_specific_lines)
    {
        tty.c_cflag |= CLOCAL; // ignore ctrl lines
    }
    else
    {
        tty.c_cflag &= ~CLOCAL;
    }

    if (configs.enable_read_data)
    {
        tty.c_cflag |= CREAD; // Turn on READ
    }
    else
    {
        tty.c_cflag &= ~CREAD;
    }


    // Canonical mode
    // UNIX systems provide two basic modes of input, canonical and non-canonical mode. In canonical mode, input is processed when a new line character is
    // received. The receiving application receives that data line-by-line. This is usually undesirable when dealing with a serial port, and so we normally
    // want to disable canonical mode.
    if (configs.enable_canonical_mode)
    {
        tty.c_lflag |= ICANON;
    }
    else
    {
        tty.c_lflag &= ~ICANON;
    }

    // Echo
    // If this bit is set, sent characters will be echoed back. Because we disabled canonical mode, I don’t think these bits actually do anything,
    // but it doesn’t harm to disable them just in case!
    if (configs.enable_basic_echo)
    {
        tty.c_lflag |= ECHO;
    }
    else
    {
        tty.c_lflag &= ~ECHO; // Disable echo
    }

    if (configs.enable_echo_erasure)
    {
        tty.c_lflag |= ECHOE;
    }
    else
    {
        tty.c_lflag &= ~ECHOE; // Disable erasure
    }

    if (configs.enable_new_line_echo)
    {
        tty.c_lflag |= ECHONL; // Disable new-line echo
    }
    else
    {
        tty.c_lflag &= ~ECHONL; // Disable new-line echo
    }


    // Signal chars
    // When the ISIG bit is set, INTR, QUIT and SUSP characters are interpreted. We don’t want this with a serial port, so clear this bit.
    if (configs.enable_signal_chars)
    {
        tty.c_lflag |= ISIG;
    }
    else
    {
        tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    }

    // Software Flow Control (IXOFF, IXON, IXANY)
    // Clearing IXOFF, IXON and IXANY disables software flow control, which we don’t want.
    if (configs.enable_software_flow_control)
    {
        tty.c_iflag |= (IXON | IXOFF | IXANY); // Turn on s/w flow ctrl
    }
    else
    {
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    }


    // Disabling Special Handling Of Bytes On Receive
    // Clearing all of the following bits disables any special handling of the bytes as they are received by the serial port, before they are
    // passed to the application. We just want the raw data thanks!
    if (configs.enable_special_byte_handle)
    {
        tty.c_iflag |= (IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    }
    else
    {
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    }


    // The c_oflag member of the termios struct contains low-level settings for output processing. When configuring a serial port, we want to
    // disable any special handling of output chars/bytes, so do the following:
    if (configs.enable_special_input_interpret)
    {
        tty.c_oflag |= OPOST;
    }
    else
    {
        tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    }

    if (configs.enable_conversion_of_new_line)
    {
        tty.c_oflag |= ONLCR;
    }
    else
    {
        tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    }

    #if not (defined(__linux__) or defined (__linux) or defined (linux) or defined (unix) or defined (__unix) or defined (__unix__) )
    if (configs.enable_conversion_tab_to_space)
    {
        tty.c_oflag |= OXTABS;
    }
    else
    {
       tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
    }

    if (configs.enable_removal_c_d_chars)
    {
        tty.c_oflag |= ONOEOT;
    }
    else
    {
        tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)
    }
    #endif // __linux__ __linux linux unix __unix __unix__

    /*
    VMIN and VTIME are a source of confusion for many programmers when trying to configure a serial port in Linux.

    An important point to note is that VTIME means slightly different things depending on what VMIN is. When VMIN is 0,
    VTIME specifies a time-out from the start of the read() call. But when VMIN is > 0, VTIME specifies the time-out
    from the start of the first received character.

    Different combinations:
    VMIN = 0, VTIME = 0: No blocking, return immediately with what is available
    VMIN > 0, VTIME = 0: This will make read() always wait for bytes (exactly how many is determined by VMIN), so read() could block indefinitely.
    VMIN = 0, VTIME > 0: This is a blocking read of any number of chars with a maximum timeout (given by VTIME). read() will block until either
    any amount of data is available, or the timeout occurs. This happens to be my favourite mode (and the one I use the most).
    VMIN > 0, VTIME > 0: Block until either VMIN characters have been received, or VTIME after first character has elapsed. Note that the timeout
    for VTIME does not begin until the first character is received.

    VMIN and VTIME are both defined as the type cc_t, which I have always seen be an alias for unsigned char (1 byte). This puts an upper limit on
    the number of VMIN characters to be 255 and the maximum timeout of 25.5 seconds (255 deciseconds).

    “Returning as soon as any data is received” does not mean you will only get 1 byte at a time. Depending on the OS latency, serial port speed,
    hardware buffers and many other things you have no direct control over, you may receive any number of bytes.
    */
    if (configs.set_vtime >= 0 && configs.set_vtime < 1000)
    {
        tty.c_cc[VTIME] = configs.set_vtime;
    }
    else
    {
        tty.c_cc[VTIME] = 10;
    }

    if (configs.set_vmin >= 0 && configs.set_vmin < 1000)
    {
        tty.c_cc[VMIN] = configs.set_vmin;
    }
    else
    {
        tty.c_cc[VMIN] = 0;
    }



    // Baud rate
    // Rather than use bit fields as with all the other settings, the serial port baud rate is set by calling the functions cfsetispeed() and
    // cfsetospeed(), passing in a pointer to your tty struct and a enum:
    switch (configs.set_i_baud_rate)
    {
    case 0:     cfsetispeed(&tty, B0);     break;
    case 50:    cfsetispeed(&tty, B50);    break;
    case 75:    cfsetispeed(&tty, B75);    break;
    case 110:   cfsetispeed(&tty, B110);    break;
    case 134:   cfsetispeed(&tty, B134);    break;
    case 150:   cfsetispeed(&tty, B150);    break;
    case 200:   cfsetispeed(&tty, B200);    break;
    case 300:   cfsetispeed(&tty, B300);    break;
    case 600:   cfsetispeed(&tty, B600);    break;
    case 1200:  cfsetispeed(&tty, B1200);    break;
    case 1800:  cfsetispeed(&tty, B1800);    break;
    case 2400:  cfsetispeed(&tty, B2400);    break;
    case 4800:  cfsetispeed(&tty, B4800);    break;
    case 19200: cfsetispeed(&tty, B19200);    break;
    case 38400: cfsetispeed(&tty, B38400);    break;
    case 57600: cfsetispeed(&tty, B57600);    break;
    case 115200: cfsetispeed(&tty, B115200);    break;
    case 230400: cfsetispeed(&tty, B230400);    break;
    case 460800: cfsetispeed(&tty, B460800);    break;
    default:
        cfsetispeed(&tty, B9600);
        break;
    }

    switch (configs.set_o_baud_rate)
    {
    case 0:     cfsetospeed(&tty, B0);     break;
    case 50:    cfsetospeed(&tty, B50);    break;
    case 75:    cfsetospeed(&tty, B75);    break;
    case 110:   cfsetospeed(&tty, B110);    break;
    case 134:   cfsetospeed(&tty, B134);    break;
    case 150:   cfsetospeed(&tty, B150);    break;
    case 200:   cfsetospeed(&tty, B200);    break;
    case 300:   cfsetospeed(&tty, B300);    break;
    case 600:   cfsetospeed(&tty, B600);    break;
    case 1200:  cfsetospeed(&tty, B1200);    break;
    case 1800:  cfsetospeed(&tty, B1800);    break;
    case 2400:  cfsetospeed(&tty, B2400);    break;
    case 4800:  cfsetospeed(&tty, B4800);    break;
    case 19200: cfsetospeed(&tty, B19200);    break;
    case 38400: cfsetospeed(&tty, B38400);    break;
    case 57600: cfsetospeed(&tty, B57600);    break;
    case 115200: cfsetospeed(&tty, B115200);    break;
    case 230400: cfsetospeed(&tty, B230400);    break;
    case 460800: cfsetospeed(&tty, B460800);    break;
    default:
        cfsetospeed(&tty, B9600);
        break;
    }

    // Setup configuration
    if ((tcsetattr(m_ACM_serial_port,TCSAFLUSH, &tty_ACM)) != 0)
    {
        return USBDEV_ERROR;
    }

    return USBDEV_SUCCESS;
}
