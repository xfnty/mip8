MAKEFLAGS=--no-print-directory --quiet

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CFLAGS:=-Wall -Werror -LC:\\Binaries\\tcc\\lib -IC:\\Binaries\\tcc\\include -std=c99 -Isrc
OUTPUT_DIR:=out
OUTPUT_BIN:=$(OUTPUT_DIR)\\mip8.exe
SOURCES:=$(call rwildcard,src,*.c)

args?=

default: build run

build: $(OUTPUT_DIR)
	xcopy /e /c /q /r /y assets out\\assets\\ > nul 2>&1 || (exit 0)
	tcc $(CFLAGS) $(SOURCES) -o $(OUTPUT_BIN)

run: $(OUTPUT_DIR)
	xcopy /e /c /q /r /y assets out\\assets\\ > nul 2>&1 || (exit 0)
	$(OUTPUT_BIN) $(args)

clean:
	rmdir /s /q $(OUTPUT_DIR) > nul 2>&1 || (exit 0)

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR) > nul 2>&1 || (exit 0)
	xcopy /e /c /q /r /y assets out\\assets\\ > nul 2>&1 || (exit 0)

.PHONY: default build run clean
