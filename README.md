# The RP-Lang Compiler

RP-Lang is a programming language that can be used to define and evaluate primitively recursive functions.

```
#include mult.rp

def ^(x, y):
-> ^(x, 0) = 1
-> ^(x, y + 1) = *(x, ^(x, y))
```

## Environment

Set the following environment variables to control and configure the behaviour of the application:

|Name|Default|Description|
|-|:-:|-|
|`LOG_IGNORED_LEXEMES`|`true`|When `true`, logs all of the ignored lexemes found with Flex at DEBUGGING level. To remove those logs from the console output set it to `false`.|
|`LOGGING_LEVEL`|`INFORMATION`|The minimum level to log in the console output. From lower to higher, the available levels are: `ALL`, `DEBUGGING`, `INFORMATION`, `WARNING`, `ERROR` and `CRITICAL`.|
|`PRINT_AST`|`YES`|When `YES` prints the generated AST. A few intermidiate nodes don't print anything.|
