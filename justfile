set dotenv-load

export EDITOR := 'nvim'

default:
  just --list

all: forbid fmt

fmt:
  clang-format -i -style=file:.clang-format src/*.cpp

forbid:
  ./bin/forbid
