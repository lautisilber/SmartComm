/*
 * In this example we can see how to extract a determined value type
 * from the arguments. Arguments are stored as char arrays (strings)
 * but can be cast to a specific type. To do this we use SmartCmdArguments'
 * method <to(argument_number, pointer_to_storing_variable)>. The first
 * argument is the argument number of argument we're trying to get.
 * The second is a pointer to a variable of the correct type.
 * Thanks to generics the method <to> will adapt to cast to the
 * correct type. If the casting cannot be done (for example due to bad
 * formatting of the argument), the method will return false. If the
 * casting was successful, the method returns true, and the cast value is
 * stored in the pointer.
 * 
 * The available types to cast to so far are
 * - Numbers
 *   - int
 *   - unsigned int
 *   - long
 *   - unsigned long
 *   - short
 *   - unsigned short
 *   - char
 *   - unsigned char
 *   - float
 *   - double
 * - Strings
 *   - const char *
 *   - String
 * - Boolean
 *   - bool
 * 
 * In this example, the comments that start with ! are used to show how can
 * you cast to const char *, instead of to String (you may want to do this to
 * avoid heap usage and memory fragmentation)
 */

#include "SmartComm.h"



// create the "OK" command which only answers with "OK"
SMART_CMD_CREATE(cmdArgs, "Args", [](Stream *stream, const SmartCmdArguments *args, const char *cmd) {
    bool b;
    int i;
    float f;
    String str;
    const char *c_str;

    stream->print("Received ");
    stream->print(args->N);
    stream->println(" arguments");

    for (int n = 0; n < args->N; i++)
    {
        stream->print("\tAnalizing argument ");
        stream->println(n);
        if (args->to(n, &b))
        {
            stream->print("\t\tCan be cast to boolean with value ");
            stream->println(b);
        }
        if (args->to(n, &i))
        {
            stream->print("\t\tCan be cast to int with value ");
            stream->println(i);
        }
        if (args->to(n, &f))
        {
            stream->print("\t\tCan be cast to float with value ");
            stream->println(f);
        }
        if (args->to(n, &str))
        {
            stream->print("\t\tCan be cast to String with value ");
            stream->println(str);
        }
        if (args->to(n, &c_str))
        {
            stream->print("\t\tCan be cast to const char* with value ");
            stream->println(str);
        }
    }
});

// create the array of commands (note the & before the class names)
const SmartCmdBase *cmds[] = {
    &cmdArgs
};

// create the CmartComm main class with the length of the cmds array
// between angled brackets <>
// Serial is opional in this case as the default value for stream is Serial
SmartComm<ARRAY_LENGTH(cmds)> sc(cmds, Serial);


void setup() {
    // initialize the Serial interface
    Serial.begin(9600);
}

void loop() {
    // call tick periodically to process the messages
    sc.tick();
}