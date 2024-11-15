# Simplest

If you run this example as is, you can send messages tou your device through the Serial interface. The messages are ended by the default endChar ```'\n'``` which si basically just the enter key. The first word of the message is the command (OK, Hello and One are the available commands in this example) and the following words are the arguments of the command. Words are separated by the default sepChar ```' '``` which is just a space. (You can use the One command to see how arguments are received)


There are multiple ways in which you can define the ```SmartCmd``` classes. I think the easiest one is this one. I provide a macro that hides away some boiler code that is used to improve the memory footprint of the library. Basically, the macro has the following definition:

```c
SMART_CMD_CREATE(className, "command", callback)
```

where the ```className``` is the name of the SmartCmd class being defined, ```"command"``` is the literal string that is the command that will trigger the ```callback``` function, when it's received. This macro should be treated as if ```SMART_CMD_CREATE(className, "command", callback)``` was completely equivalent to ```SmartCmd className("command", callback)``` (in some cases it is, in fact, completely equivalent)

The ```SmartCmd``` classes take in a callback function with the definition of ```void(*)(Stream*, const SmartCmdArguments*, const char*)``` which means a function like the followig will work perfectly:

```c
void commandCallback(Stream *stream, const SmartCmdArguments *argrs, const char *cmd)
{
    ...
}
```


In this example I decided to use lambda functions instead of traditional ones because I think the resulting code is much more compact. However, there is absolutely no problem with using traditional functions. For example, the ```cmdOk``` class could be defined in the following way:

```c
void cmdOkCb(Stream *stream, const SmartCmdArguments *argrs, const char *cmd) {
    stream->println("OK");
}
SMART_CMD_CREATE(cmdOk, "OK", cmdOkCb);
```

which would result in exactly the same functionality.


The cmds array is necessary for the library to work. I chose this approach because this way it is possible to avoid heap allocation, and ensure the array containing all the SmartCmd is as small as possible. So we need to declare a const fixed-sized array of SmartCmdBase pointers. We use SmartCmdBase instead of SmartCmd because this allows me to define different functionality for when resources are available to reduce RAM memory usage (that functionality is hidden behind the SMART_CMD_CREATE macro, so you don't have to worry).


Lastly we need to pass the cmds array to the SmartComm main class. Once that is done, there's no more configuration needed. We only have to make sure to call SmartComm's tick() method to process incoming messages. This method has negligible performance impact when no data has to be processed and it only processes the message once it is input, so you can call this function as often as you want without having to worry about it slowing your sketch down. Note the angled brackets <> on the class declaration. Because SmartComm is a templated class, you have to indicate the number of SmartCmd in the array inside the angled brackets. You could do this manually by writing ```SmartComm<3> sc(cmds);```. However, this is a bit prone to mistakes (like forgetting to update this number when you change the number of ```SmartCmd```) so the ```ARRAY_LENGTH``` macro allows you to get the length of the coms array at compile time, so you don't have to worry about it.


In this particular example we're using the ```Serial``` interface to communicate. If you'd prefer to use anohter interface, check out the other examples.