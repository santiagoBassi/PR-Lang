# The RP-Lang Compiler

RP-Lang is a programming language that can be used to define and evaluate primitively recursive functions.

```
#include mult.rp

def ^(x, y):
-> ^(x, 0) = 1
-> ^(x, y + 1) = *(x, ^(x, y))

^(2, 3)
```

## Running the compiler

The compiler can be built using the `build.sh` script.

The generated binary expects an input and output file as parameters. It can be run is the following ways.

```
./build/Compiler in_file out_file
./script/ubuntu/start.sh in_file out_file
```

If you have `gcc` installed you can use the `generate-binary.sh` script to also generate a binary.

```
./script/ubuntu/generate-binary.sh in_file
```

## Environment

Set the following environment variables to control and configure the behaviour of the application:

|Name|Default|Description|
|-|:-:|-|
|`LOG_IGNORED_LEXEMES`|`true`|When `true`, logs all of the ignored lexemes found with Flex at DEBUGGING level. To remove those logs from the console output set it to `false`.|
|`LOGGING_LEVEL`|`INFORMATION`|The minimum level to log in the console output. From lower to higher, the available levels are: `ALL`, `DEBUGGING`, `INFORMATION`, `WARNING`, `ERROR` and `CRITICAL`.|
|`PRINT_AST`|`YES`|When `YES` prints the generated AST. A few intermidiate nodes don't print anything.|

## Syntax Highlighting

If you are using VSCode you can install the RP-Lang extension to enable syntax highlighting. In the `syntax-highlighting` folder you will find the `.vsix` file. In VSCode go to `Extensions > More options (dots) > Install from VSIX` and select the extension file.
