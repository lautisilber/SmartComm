# Arguments

In this example we can see how to extract a determined value type from the arguments. Arguments are stored as char arrays (strings) but can be cast to a specific type. To do this we use ```SmartCmdArguments```' method ```to(argument_number, pointer_to_storing_variable)```. The first argument is the argument number of argument we're trying to get. The second is a pointer to a variable of the correct type. Thanks to generics the method ```to``` will adapt to cast to the correct type. If the casting cannot be done (for example due to bad formatting of the argument), the method will return false. If the casting was successful, the method returns true, and the cast value is stored in the pointer.

The available types to cast to so far are
    - Numbers
        - ```int```
        - ```unsigned int```
        - ```long```
        - ```unsigned long```
        - ```short```
        - ```unsigned short```
        - ```char```
        - ```unsigned char```
        - ```float```
        - ```double```
    - Strings
        - ```const char *```
        - ```String```
    - Boolean
        - ```bool```

In this example, the comments that start with ! are used to show how can you cast to const char *, instead of to String (you may want to do this to avoid heap usage and memory fragmentation)