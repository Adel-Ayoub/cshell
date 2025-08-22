# CShell

## A comprehensive Unix shell implementation in C with advanced features and trinary tree architecture.

Currently the shell offers:

- Interactive and non-interactive modes with robust signal handling
- 11 built-in commands with advanced features and environment management
- Complete redirection system with input/output and here-documents
- Multi-stage pipe system with process management
- Command chaining with semicolon-separated commands
- Wildcard expansion with pattern matching
- Environment variable expansion with default value support
- Trinary tree architecture for advanced command execution

Please check the usage examples to see how to use the shell and its features!

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
- Readline library (automatically managed by Makefile)

---

## Features

### Core Shell:

| Feature | Status | Description |
|---------|--------|-------------|
| Built-in Commands | [COMPLETE] | 11 essential shell commands |
| External Commands | [COMPLETE] | Execute system binaries |
| Environment Management | [COMPLETE] | Variable expansion and management |
| Redirections | [COMPLETE] | Input/output and here-documents |
| Pipes | [IN PROGRESS] | Multi-stage command pipelines |
| Command Chaining | [COMPLETE] | Semicolon-separated commands |
| Wildcard Expansion | [COMPLETE] | Pattern matching for files |
| Signal Handling | [COMPLETE] | SIGINT, SIGQUIT management |

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

### Basic Operations:
```sh
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

### Advanced Features:
```sh
# Environment management
export PATH=/usr/local/bin:$PATH
echo $HOME
echo ${USER:-guest}

# Redirections
cat < input.txt
echo "hello" > output.txt
echo "world" >> output.txt
cat << EOF
multi-line
input
EOF

# Pipes
echo "hello world" | grep "hello"
ls -la | head -5

# Command chaining
pwd; echo hello; pwd

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

- [ ] Complete pipe system debugging and output production
- [ ] Job control and background process management
- [ ] Logical operators (AND &&, OR ||) for command chaining
- [ ] Priority-based command execution
- [ ] Advanced signal handling and process management
- [ ] Comprehensive testing suite and validation
- [ ] Performance optimization and benchmarking
- [ ] Memory usage profiling and optimization

---

## Known Issues

- **Pipe Output**: Pipes are structurally complete but need debugging for proper output production
- **Status**: Being actively worked on in current development phase



## License

See [LICENSE](LICENSE) for details.
