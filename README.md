# CShell

## A Modern Unix Shell Implementation in C

CShell is a clean command-line shell built with C that provides essential Unix shell functionality. Features built-in commands, external program execution, and background process support.

## How to use CShell

### Interactive Command Line Interface

CShell provides a full-featured interactive shell experience:

1. **File Operations** - Navigate and manipulate files and directories
2. **System Navigation** - Change directories and view current location
3. **Text Processing** - Display file contents and search patterns
4. **Permission Management** - Modify file and directory permissions

The shell provides comprehensive functionality:
- **Built-in command execution** for core shell operations
- **External command support** executes system binaries
- **Background process handling** with `&` operator
- **Real-time command feedback** with informative error messages
- **Familiar Unix interface** with standard command syntax

## How to install CShell

### Prerequisites

Make sure you have a C compiler (GCC) and Make installed on your system.

### From source

You can clone this repository using git:

```bash
git clone https://github.com/Adel-Ayoub/cshell.git
cd cshell
```

After cloning, you can use the following commands:

| Command | Description |
| ------- | ----------- |
| **`make`** | Compiles the project with optimizations |
| **`make debug`** | Builds with debug symbols and flags |
| **`make clean`** | Removes build artifacts and executable |
| **`make install`** | Installs cshell to /usr/local/bin |
| **`make rebuild`** | Clean build from scratch |

### Binary Release

> W.I.P - Pre-compiled binaries will be available for download

## Usage Examples

```bash
# Start CShell
./cshell

# Navigate filesystem
pwd
cd /home/user
ls -la

# File operations
mkdir projects
cp file.txt backup.txt
mv old_name.txt new_name.txt
rm unwanted.txt

# Text processing
cat document.txt
grep "pattern" file.txt

# Permission management
chmod 755 script.sh

# Background processes
sleep 10 &

# External commands
ps aux
top

# Exit shell
exit
```

## More Information about CShell

### The Features

CShell includes the following functionality:

#### Core Features

| Feature | Description | Scope |
| ------- | ----------- | ----- |
| **Built-in Commands** | Native implementation of essential shell commands | Core functionality |
| **External Command Support** | Execute any system binary or script | System integration |
| **Background Processing** | Run processes in background with `&` operator | Process management |
| **Memory Management** | Proper allocation and cleanup of resources | System stability |

#### Built-in Commands

| Command | Description | Function |
| ------- | ----------- | -------- |
| **`pwd`** | Print working directory | Shows current location |
| **`cd <dir>`** | Change directory | Navigate filesystem |
| **`ls [files]`** | List directory contents | File and directory listing |
| **`cat <files>`** | Display file contents | Text file viewing |
| **`mkdir <dir>`** | Create directory | Directory creation |
| **`mv <src> <dest>`** | Move/rename files | File operations |
| **`cp [-r] <src> <dest>`** | Copy files or directories | File duplication |
| **`rm [-r] <files>`** | Remove files or directories | File deletion |
| **`chmod <mode> <file>`** | Change file permissions | Permission management |
| **`grep <pattern> [files]`** | Search text patterns | Text processing |

### Technical Details

The application is built using:

- **C99 Standard** - Modern C programming with proper practices
- **Modular Design** - Separate modules for parsing, execution, and commands
- **System Calls** - Direct interaction with Unix system calls
- **Process Management** - Fork/exec model for command execution

### Project Structure

```
src/
├── main.c              # Entry point
├── shell.h             # Main header with all declarations
├── shell.c             # Core shell loop and logic
├── parser.c/h          # Input parsing and tokenization
├── executor.c/h        # Command execution and process management
└── commands/           # Individual command implementations
    ├── pwd.c           # Print working directory
    ├── cd.c            # Change directory
    ├── ls.c            # List directory contents
    ├── cat.c           # Display file contents
    ├── mkdir.c         # Create directories
    ├── mv.c            # Move/rename files
    ├── cp.c            # Copy files and directories
    ├── rm.c            # Remove files and directories
    ├── chmod.c         # Change permissions
    └── grep.c          # Pattern matching and utilities
```

## Known Issues

> There are currently no known issues. Please report any bugs you encounter.
