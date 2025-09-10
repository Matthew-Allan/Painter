# General
CORE_FILES = src/shader.c src/paths.c src/window.c src/types.c src/glad/glad.c
APP_FILES = src/main.c
FILES = $(APP_FILES) $(CORE_FILES)
ARGS = -fdiagnostics-color=always -g -Wall -Werror -framework CoreFoundation
INCLUDES = -Iinclude -lSDL2
PROG_NAME = Painter
PROG_VER = 1.0
OUT = out
PROG_LOC = $(OUT)/$(PROG_NAME)

# For MAC
APP = $(OUT)/$(PROG_NAME).app
APP_CONTENTS = $(APP)/Contents
APP_RESOURCES = $(APP_CONTENTS)/Resources
APP_FRAMEWORKS = $(APP_CONTENTS)/Frameworks
APP_MAC_OS = $(APP_CONTENTS)/MacOS
BUNDLE_ID = com.suityourselfgames.painter
ICON = Icon.icns
SUBS = -e 's:PROG_NAME:$(PROG_NAME):' -e 's:BUNDLE_ID:$(BUNDLE_ID):' -e 's:PROG_VER:$(PROG_VER):' -e 's:ICON_FILE:$(ICON):'

# Build and run the executable.
all: build run

# Build the executable
build:
	mkdir -p $(OUT)
	ln -sf ../shaders $(OUT)/
	gcc $(ARGS) $(INCLUDES) $(FILES) -o $(PROG_LOC)

# Run the program
run:
	./$(PROG_LOC)

# Clean any files that may have been created on build
clean:
	rm -fr $(OUT)

# Create a .app package
mac: clean build
	mkdir -p $(APP_RESOURCES)
	mkdir $(APP_MAC_OS)
	mkdir $(APP_FRAMEWORKS)
	cp MacOS/libSDL2-2.0.0.dylib $(APP_FRAMEWORKS)/libSDL2-2.0.0.dylib
	cp MacOS/Info.plist $(APP_CONTENTS)/Info.plist
	sed $(SUBS) MacOS/Info.plist > $(APP_CONTENTS)/Info.plist
	cp MacOS/Icon.icns $(APP_RESOURCES)/$(ICON)
	cp -R shaders $(APP_RESOURCES)/shaders
	cp $(PROG_LOC) $(APP_MAC_OS)/$(PROG_NAME)
	install_name_tool -change $(shell brew --prefix sdl2)/lib/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib $(APP_MAC_OS)/$(PROG_NAME)
	zip -r $(PROG_LOC).app.zip $(PROG_LOC).app

