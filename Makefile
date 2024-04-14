MAKEFLAGS=--no-print-directory --quiet

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CFLAGS:=-Wall -Werror -LC:\\Binaries\\tcc\\lib -IC:\\Binaries\\tcc\\include -std=c99 -Isrc

OUTPUT_DIR:=out
TEST_OUTPUT_DIR:=tests-out

APP_SOURCES:=$(wildcard src/*.c)
app_name=$(notdir $(basename $1))
app_exe=$(OUTPUT_DIR)\\$(call app_name,$1).exe
APPS=$(foreach s,$(APP_SOURCES),$(call app_name,$(s)))

COMMON_SOURCES:=$(filter-out $(APP_SOURCES),$(call rwildcard,src,*.c))
TEST_SOURCES:=$(call rwildcard,tests,*.c)

args?=

default:
	echo Run "make <app>" to build and run the program.
	echo Available programs are: $(APPS)

$(APP_SOURCES): $(OUTPUT_DIR)
	xcopy /e /c /q /r /y assets out\\assets\\ > nul 2>&1 || (exit 0)
	tcc $(CFLAGS) $(COMMON_SOURCES) $@ -o $(call app_exe,$@)

$(APPS): $(APP_SOURCES)  # FIXME: build only the required app source file
	$(call app_exe,$@) $(args)

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
	tcc $(CFLAGS) -Itests $@ $(COMMON_SOURCES) -o $(TEST_OUTPUT_DIR)\\$(basename $(notdir $@)).exe
	$(TEST_OUTPUT_DIR)\\$(basename $(notdir $@)).exe

.PHONY: default build run test clean $(APP_SOURCES) $(TEST_SOURCES)
