MAKEFLAGS=--no-print-directory --quiet

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CFLAGS:=-Wall -Werror -LC:\\Binaries\\tcc\\lib -IC:\\Binaries\\tcc\\include -std=c99 -Isrc
OUTPUT_DIR:=out
TEST_OUTPUT_DIR:=tests-out
OUTPUT_BIN:=$(OUTPUT_DIR)\\mip8.exe
MAIN_SOURCE:=src/main.c
SOURCES:=$(filter-out $(MAIN_SOURCE),$(call rwildcard,src,*.c))
TEST_SOURCES:=$(call rwildcard,tests,*.c)

args?=

default: build run

build: $(OUTPUT_DIR)
	xcopy /e /c /q /r /y assets out\\assets\\ > nul 2>&1 || (exit 0)
	tcc $(CFLAGS) $(MAIN_SOURCE) $(SOURCES) -o $(OUTPUT_BIN)

run: $(OUTPUT_DIR)
	xcopy /e /c /q /r /y assets out\\assets\\ > nul 2>&1 || (exit 0)
	$(OUTPUT_BIN) $(args)

test: $(TEST_OUTPUT_DIR) $(TEST_SOURCES)
	xcopy /e /c /q /r /y assets out\\assets\\ > nul 2>&1 || (exit 0)

clean:
	rmdir /s /q $(OUTPUT_DIR) > nul 2>&1 || (exit 0)
	rmdir /s /q $($(TEST_OUTPUT_DIR)) > nul 2>&1 || (exit 0)

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR) > nul 2>&1 || (exit 0)
	xcopy /e /c /q /r /y assets $(OUTPUT_DIR)\\assets\\ > nul 2>&1 || (exit 0)

$(TEST_OUTPUT_DIR):
	mkdir $(TEST_OUTPUT_DIR) > nul 2>&1 || (exit 0)
	xcopy /e /c /q /r /y assets $(TEST_OUTPUT_DIR)\\assets\\ > nul 2>&1 || (exit 0)

$(TEST_SOURCES):
	tcc $(CFLAGS) -Itests $@ $(SOURCES) -o $(TEST_OUTPUT_DIR)\\$(basename $(notdir $@)).exe
	$(TEST_OUTPUT_DIR)\\$(basename $(notdir $@)).exe

.PHONY: default build run test clean $(TEST_SOURCES)
