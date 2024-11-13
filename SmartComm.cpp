#include "SmartComm.h"

/// SmartCmdArguments /////////////////////////////////////////////////////////////////////////

SmartCmdArguments::SmartCmdArguments(_smart_comm_size_t n, const char *const args[MAX_ARGUMENTS])
: N(n), _args(args)
{}
const char *SmartCmdArguments::arg(_smart_comm_size_t n) const
{
    if (n >= N) return nullptr;
    return _args[n];
}
bool SmartCmdArguments::toInt(_smart_comm_size_t n, long *i)
{
    const char *str = arg(n);
    if (str == nullptr) return false;

    char *endptr;
    *i = strtol(str, &endptr, 10);
    return str[0] != '\0' && *endptr == '\0';
}
bool SmartCmdArguments::toBool(_smart_comm_size_t n, bool *b)
{
    const char *str = arg(n);
    if (str == nullptr) return false;

    if (
        #if defined(ARDUINO_ARCH_AVR)
        strcmp_P(str, (PGM_P)F("1")) == 0 || strcmp_P(str, (PGM_P)F("true")) == 0 ||
        strcmp_P(str, (PGM_P)F("True")) == 0 || strcmp_P(str, (PGM_P)F("TRUE")) == 0
        #else
        strcmp(str, "1") == 0 || strcmp(str, "true") == 0 ||
        strcmp(str, "True") == 0 || strcmp(str, "TRUE") == 0
        #endif
    )
    {
        *b = true;
        return true;
    }

    if (
        #if defined(ARDUINO_ARCH_AVR)
        strcmp_P(str, (PGM_P)F("0")) == 0 || strcmp_P(str, (PGM_P)F("false")) == 0 ||
        strcmp_P(str, (PGM_P)F("False")) == 0 || strcmp_P(str, (PGM_P)F("FALSE")) == 0
        #else
        strcmp(str, "0") == 0 || strcmp(str, "false") == 0 ||
        strcmp(str, "False") == 0 || strcmp(str, "FALSE") == 0
        #endif
    )
    {
        *b = false;
        return true;
    }
    return false;
}



/// SmartCmds /////////////////////////////////////////////////////////////////////////////////

SmartCmdBase::SmartCmdBase(const char *command, smartCmdCB_t callback): _cmd(command), _cb(callback) {}

SmartCmd::SmartCmd(const char *command, smartCmdCB_t callback): SmartCmdBase(command, callback) {}
bool SmartCmd::is_command(const char *str) const { return strcmp(str, _cmd) == 0; }
void SmartCmd::callback(Stream *stream, const SmartCmdArguments *args) const { _cb(stream, args, _cmd); }

#ifdef PROGMEM
SmartCmdF::SmartCmdF(const PROGMEM char *command, smartCmdCB_t callback): SmartCmdBase(reinterpret_cast<PGM_P>(command), callback) {}
bool SmartCmdF::is_command(const char *str) const { return strcmp_P(str, _cmd) == 0; }
void SmartCmdF::callback(Stream *stream, const SmartCmdArguments *args) const
{
    _smart_comm_size_t len = strlen_P((const char *)_cmd);
    char *cmd = (char *)malloc(len);
    strncpy_P(cmd, (const char *)_cmd, len);
    _cb(stream, args, cmd);
    free(cmd);
}
#endif

void __defaultCommandNotRecognizedCB(Stream *stream, const char *cmd)
{
    #if defined(ARDUINO_ARCH_AVR)
    stream->print(F("ERROR: Unknown command '"));
    stream->print(cmd);
    stream->println(F("'"));
    #else
    stream->print("ERROR: Unknown command '");
    stream->print(cmd);
    stream->println("'");
    #endif
}



/// SmartComm /////////////////////////////////////////////////////////////////////////////////////

static void __trimChar(char *&str, char c)
{
    // it modifies str so that all chatacters c at the start and end of str are removed. if c is space
    // and str is "     hello there  ", after the function, str will be "hello there"
    // this modifies the original str. it returns a new pointer (inside the original str)
    // the str argument should be passed as *&a if a is a (char *)
    // https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
    if (c == '\0' || str == nullptr || *str == '\0') {
        return; // Nothing to trim
    }

    // Move front pointer forward to skip leading `c` characters
    while (*str == c)
        ++str;

    // Define the end pointer to find the end of the trimmed string
    char *endp, *originalEndp;
    originalEndp = endp = str + strlen(str) - 1;

    // Move end pointer backward to skip trailing `c` characters
    while (endp > str && *endp == c)
        --endp;

    // Place the null terminator after the last valid character
    if (endp < originalEndp)
        *(endp + 1) = '\0';
}

static void __removeConsecutiveDuplicates(char *&str, char c)
{
    // modifies str such that there are no multiple consecutive characters c. That means that if c
    // is a space, and str is "  hi how     are  you  ", after running this function, str will be
    // " hi how are you "
    //
    // the idea of the implementation is the following:
    // we traverse the string. if we find c in position i, we start counting how many contiguous
    // characters c are after index i. if its one, we continue incrementing i. if there are more,
    // we count how many there are before the caracter is no longer c. then we move the whole str
    // that's on the right side of i to overwrite the excess of c characters

    if (c == '\0' || str == nullptr || *str == '\0') {
        return; // Nothing to remove
    }
    _smart_comm_size_t len = strlen(str);
    if (len == 1) return;
    else if (len == 2)
    {
        if (*str == c && *(str + 1) == c)
        {
            *(str + 1) = '\0';
            return;
        }
    }

    char *endp, *p;
    endp = str + len - 1;
    
    #ifndef __AVR__ // maybe a bit overkill for avr program size
    // remove all duplicates from start
    while (*str == c && *(str + 1) == c)
        ++str;
    // remove all duplicates from end
    while (*endp == c && *(endp - 1) == c)
        --endp;
    *(endp + 1) = '\0';
    #endif

    // remove duplicates from center
    _smart_comm_size_t nConsec = 0;
    len = endp - str + 1;
    for (p = str; p < endp; ++p)
    {
        // count how many consecutives
        // nConsec = 0;
        if (*p == c)
        {
            while (*(p + nConsec + 1) == c)
                ++nConsec;
        }

        // shift all chars to the left nConsec places
        if (nConsec)
        {
            // for (_smart_comm_size_t i = 0; i < len; i++)
            //     *(p+i) = *(p+i+nConsec);
            memmove(p, p+nConsec, len);
            // update strlen(p) == len accordingly
            len -= nConsec;
            nConsec = 0;
        }
        --len;
    }
}

static bool __isCharUnwanted(char c, char endChar, char sepChar)
{
    return !(c == endChar || c == sepChar || (c > 32 && c < 127) || c == '\0');
}

static void __removeUnwantedChars(char *&str, char endChar, char sepChar)
{
    if (str == nullptr || *str == '\0') {
        return; // Nothing to remove
    }

    _smart_comm_size_t len = strlen(str);
    if (len == 1)
    {
        if (__isCharUnwanted(*str, endChar, sepChar))
            *str = '\0';
        return;
    }

    char *endp, *originalEndp, *p;
    originalEndp = endp = str + len - 1;

    #ifndef __AVR__ // maybe a bit overkill for avr program size
    // remove from front
    while (__isCharUnwanted(*str, endChar, sepChar))
        ++str;

    // remove from tail
    while (__isCharUnwanted(*endp, endChar, sepChar) && endp > str)
        --endp;
    #endif

    // remove from center
    _smart_comm_size_t nConsec = 0;
    len = endp - str + 1;
    for (p = str; p < endp; ++p)
    {
        if (__isCharUnwanted(*p, endChar, sepChar))
        {
            while (__isCharUnwanted(*(p + nConsec + 1), endChar, sepChar))
                ++nConsec;
        }

        // shift all chars to the left nConsec places
        if (nConsec)
        {
            // for (_smart_comm_size_t i = 0; i < len; i++)
            //     *(p+i) = *(p+i+nConsec);
            memmove(p, p+nConsec, len);
            // update strlen(p) == len accordingly
            len -= nConsec;
            nConsec = 0;
        }
        --len;
    }
}

bool __extractArguments(char *buffer, char endChar, char sepChar, char *&command, char *args[MAX_ARGUMENTS], _smart_comm_size_t &nArgs)
{
    // return true if arguments were found. populates nArgs and populates the args array of arguments.
    // buffer cannot be used afterward because it is modified to store the arguments pointed to by args

    // trim leading and trailing sepChars from _buffer
    __trimChar(buffer, sepChar);

    // remove duplicate sepChars
    __removeConsecutiveDuplicates(buffer, sepChar);

    // remove unwanted chars
    __removeUnwantedChars(buffer, endChar, sepChar);

    _SMART_COMM_DEBUG_PRINT_STATIC("SMARTCOMM DEBUG: After conditioning, the message is '");_SMART_COMM_DEBUG_PRINT(buffer);_SMART_COMM_DEBUG_PRINT_STATIC("'\n");

    command = buffer;

    _smart_comm_size_t len = strlen(buffer);

    if (*buffer == '\0' || len == 0)
    {
        _SMART_COMM_DEBUG_PRINT_STATIC("SMARTCOMM DEBUG: No message structure found\n");
        return false;
    }

    // find all arguments
    char *sepPtr = buffer; // ptr of the last separation char found + 1
    // char *args[MAX_ARGUMENTS] = {0}; // ptrs of all separation characters found
    // _smart_comm_size_t nArgs = 0; // amount of arguments
    nArgs = 0;
    char *ptr;
    for (;;)
    {
        ptr = strchr(sepPtr, sepChar); // this returns the pointer to the first ocurrence

        if (ptr == NULL) break;

        // later we split the buffer into several strings,
        // each being an argument (the first is the command)
        // to reuse the same char array, we can change the
        // sep chars for '\0' and save pointers to the next char
        *ptr = '\0';
        
        // -2 because we dont want sepIndex to point to the ending '\0'
        if (ptr >= buffer+len-1) break;
        sepPtr = ptr+1;
        
        if (nArgs >= MAX_ARGUMENTS) break;
        args[nArgs++] = sepPtr;
    }

    #ifdef _SMART_COMM_DEBUG
    _SMART_COMM_DEBUG_PRINT_STATIC("SMARTCOMM DEBUG: The command is '");_SMART_COMM_DEBUG_PRINT(command);
    if (nArgs)
    {
        _SMART_COMM_DEBUG_PRINT_STATIC("' and the arguments are:\n");
    }
    else
    {
        _SMART_COMM_DEBUG_PRINT_STATIC("' with no arguments\n");
    }
    for (_smart_comm_size_t i = 0; i < nArgs; ++i)
    {
        _SMART_COMM_DEBUG_PRINT_STATIC("\t");
        _SMART_COMM_DEBUG_PRINT(i);
        _SMART_COMM_DEBUG_PRINT_STATIC(": '");
        _SMART_COMM_DEBUG_PRINT(args[i]);
        _SMART_COMM_DEBUG_PRINT_STATIC("'\n");
    }
    #endif

    return true;
}