# CShell

## A Unix shell implementation in C.

## Installation

```sh
# Clone
git clone https://github.com/Adel-Ayoub/cshell.git
cd cshell

# Build
make

# Run
./cshell
```

---

## Requirements

- C compiler (GCC/Clang)
- Make

---

## Features

### Completed Features

#### Core Shell Functionality
- Command Execution: Built-in and external command support
- Environment Variables: Full environment variable management
- Signal Handling: Proper signal handling for interactive shell
- History: Command history with readline integration
- Exit Status: Proper exit status handling and calculation

#### Advanced Wildcard System
- Star Patterns: * for glob matching (e.g., *.c, src/*.h)
- Question Mark: ? for single character matching (e.g., c?????.h → cshell.h)
- Bracket Patterns: [set] for character sets (e.g., [abc], [1-5], [^xyz])
- Range Expansion: Automatic expansion of [a-c] to [abc]
- Mixed Patterns: Complex combinations like [abc]?*.txt
- Negated Sets: [^abc] to exclude characters
- Directory Support: Patterns like src/input_handling/*.c work perfectly
- Pipe Integration: Wildcards work seamlessly in pipe commands

#### Logical Operators & Job Control
- AND Operator: && for command chaining with short-circuit evaluation
- OR Operator: || for command chaining with short-circuit evaluation
- Background Jobs: & for background process execution
- Job Management: jobs, fg, bg commands for process control
- Process Groups: Proper process group management for job control

#### Pipes & Redirections
- Pipeline Support: Multiple command chaining with pipes
- File Redirections: Input/output redirection (<, >, >>)
- Here Documents: << for inline document input
- Error Redirections: 2> for stderr redirection
- Combined Redirections: Complex redirection scenarios

#### Built-in Commands
- cd: Directory navigation with ~ expansion
- echo: Text output with options
- pwd: Print working directory
- export: Environment variable export
- unset: Environment variable removal
- env: Environment variable display
- exit: Shell exit with status code
- jobs: Background job management
- fg/bg: Foreground/background job control

### In Progress
- Recursive Directory Patterns: src/*/*.c support (not yet implemented)
- Memory Leak Testing: Valgrind profiling needed
- Error Handling Functions: Several utility functions incomplete

### Planned Features
- Advanced Wildcards: ** for recursive matching
- Enhanced Error Messages: User-friendly error handling
- Performance Optimization: Large directory scanning optimization

---

## Built-in Commands

| Command | Description |
|---------|-------------|
| `echo [text]` | Display text with environment expansion |
| `cd [directory]` | Change directory with tilde expansion |
| `pwd` | Print working directory |
| `env` | Display environment variables |
| `export [var=value]` | Set environment variables |
| `unset [variable]` | Remove environment variables |
| `exit [status]` | Exit shell with status |
| `history` | Show command history |
| `type [command]` | Show command type |
| `help [command]` | Show help information |
| `jobs` | Show background processes |

---

## Usage Examples

### Installation
```sh
# Clone
git clone https://github.com/Adel-Ayoub/cshell.git
cd cshell

# Build
make

# Run
./cshell
```

### Basic Operations
```bash
# Navigation
pwd
cd /home/user
ls -la

# File operations
mkdir projects
cp file.txt backup.txt
mv old_name.txt new_name.txt
rm unwanted.txt
```

### Wildcard Patterns
```bash
# Basic patterns
ls *.c                    # List all .c files
ls src/*.h               # List all .h files in src directory
ls c?????.h              # List files like cshell.h

# Bracket patterns
ls test[abc].txt         # List testa.txt, testb.txt, testc.txt
ls test[1-5].txt         # List test1.txt through test5.txt
ls test[^abc]*.txt       # List files NOT starting with a, b, or c

# Complex patterns
ls test[abc]?*.txt       # Complex mixed wildcard patterns
ls src/[a-z]*.c          # Files in src starting with lowercase letters
```

### Logical Operators
```bash
# AND operator (both must succeed)
mkdir test && cd test && echo "Success"

# OR operator (first success wins)
command1 || command2 || command3

# Background jobs
sleep 30 &                # Run sleep in background
jobs                      # List background jobs
fg 1                      # Bring job 1 to foreground
bg 1                      # Send job 1 to background
```

### Pipes & Redirections
```bash
# Basic pipes
ls *.c | grep main       # List .c files containing "main"
cat file.txt | wc -l     # Count lines in file

# Complex pipes with wildcards
ls test[abc]*.txt | wc -l    # Count matching files
ls src/*.c | grep -c "\.h"   # Count .c files in src

# Redirections
echo "Hello" > output.txt     # Write to file
cat < input.txt               # Read from file
ls *.c 2> errors.log          # Redirect errors to file

# Here documents
cat << EOF
multi-line
input
EOF
```

### Environment & Variables
```bash
export PATH=$PATH:/usr/local/bin
echo $HOME                   # Print home directory
cd ~                         # Go to home directory
unset TEMP_VAR               # Remove variable
env                          # List all environment variables
```

### Advanced Features
```bash
# Command chaining
pwd; echo hello; pwd

# Complex logical operators
echo "Test" && echo "Success" || echo "Failure"

# Multiple background jobs
sleep 30 & sleep 25 & sleep 20 &
jobs

# Wildcard expansion
ls *.txt
cp file?.txt backup/
```

---

## Build System

| Target | Description |
|--------|-------------|
| `make` | Build with optimizations |
| `make debug` | Build with debug symbols |
| `make clean` | Remove build artifacts |
| `make fclean` | Remove all artifacts and libraries |
| `make re` | Clean build from scratch |

---

## Future Improvements

- [x] Job control and background process management
- [x] Logical operators (AND &&, OR ||) for command chaining
- [x] Advanced wildcard system with bracket patterns and range expansion
- [x] Wildcard integration with pipes and complex command chains
- [ ] Priority-based command execution
- [ ] Advanced signal handling and process management
- [ ] Comprehensive testing suite and validation
- [ ] Performance optimization and benchmarking
- [ ] Memory usage profiling and optimization

---

## License

See [LICENSE](LICENSE) for details.
