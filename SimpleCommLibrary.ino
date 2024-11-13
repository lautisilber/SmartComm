#include "SmartComm.h"

#define LED_ON() digitalWrite(LED_BUILTIN, HIGH)
#define LED_OFF() digitalWrite(LED_BUILTIN, LOW)

SMART_CMD_CREATE(cmdOk, "OK", [](Stream *stream, const SmartCmdArguments *args, const char *cmd){
    stream->println("OK");
});
SMART_CMD_CREATE(cmdOne, "One", [](Stream *stream, const SmartCmdArguments *args, const char *cmd){
    stream->print("I received the command: 'ONE' ");
    if (!args->N)
    {
        stream->println("without arguments.");
        return;
    }
    stream->println("With the arguments:");
    for (size_t i = 0; i < args->N; i++)
    {
        stream->print("\t ");
        stream->print(i);
        stream->print(": ");
        stream->println(args->arg(i));
    }
});
SMART_CMD_CREATE(cmdTwo, "Two", [](Stream *stream, const SmartCmdArguments *args, const char *cmd){
    stream->print("I received the command: 'Two' ");
    if (!args->N)
    {
        stream->println("without arguments.");
        return;
    }
    stream->println("With the arguments:");
    for (size_t i = 0; i < args->N; i++)
    {
        stream->print("\t ");
        stream->print(i);
        stream->print(": ");
        stream->println(args->arg(i));
    }
});

const SmartCmdBase *cmds[3] = {
    &cmdOk, &cmdOne, &cmdOne
};

SmartComm<3> sc(&Serial, cmds);

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    while (!Serial) { delay(100); }
    Serial.println("begin");

    // sc.addCommand(&cmdOk);
    // sc.addCommand(&cmdOne);
    // sc.addCommands<2>(cmds);
}

void loop()
{
    LED_ON();
    delay(1000);
    sc.tick();
    LED_OFF();
    delay(1000);
    sc.tick();
}
