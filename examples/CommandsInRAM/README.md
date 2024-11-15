# Commands in RAM

This examples show you the use of the macro ```SMART_CMD_CREATE_RAM```. Usually, the macro ```SMART_CMD_CREATE``` will check if your target architecture (board) is capable of using flash-stored variables, and will use them to store the command string data. This may (or may not in some cases) reduce the RAM usage of the library. However, if the number of commands is not high, and the length of each command is not too great, it might be more memory efficient not to store the commands in flash and keep it all in RAM. For those cases, the macro ```SMART_CMD_CREATE_RAM``` can be employed. It works the same as ```SMART_CMD_CREATE```, but forces the command to stay in RAM, which is exactly the same as defining the class yourself. That means that

```c
SMART_CMD_CREATE_RAM(cmdOk, "OK", callback);
```

and

```c
SmartCmd cmdOk("OK", callback);
```

Are literally the same. When in doubt I encourage you to try both SMART_CMD_CREATE and SMART_CMD_CREATE_RAM and choose the one that fits your project the best.