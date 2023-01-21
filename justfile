set dotenv-load

export EDITOR := 'nvim'

default:
  just --list

all: forbid fmt

fmt:
  clang-format -i -style=file:.clang-format src/*.cpp

forbid:
  ./bin/forbid

run name:
  g++ -std=c++11 src/{{name}}.cpp
  ./a.out
  rm -rf a.out
