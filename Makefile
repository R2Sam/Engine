UNAME := $(shell uname -s)

ifeq ($(UNAME), Linux)
debug:
	mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && $(MAKE) -j12 -s

release:
	mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && $(MAKE) -j12 -s

clear:
	-mv build/compile_commands.json compile_commands.json
	-rm -r build
	-mkdir build
	-mv compile_commands.json build/compile_commands.json

format:
	find src -name "*.cpp" -o -name "*.h" | xargs clang-format --dry-run --Werror
	find src -name "*.cpp" -o -name "*.h" | xargs run-clang-tidy -quiet -p build

run:
	gnome-terminal -- zsh -c "cd bin && ./main; exec zsh"

else
debug:
	-mkdir build
	cd build && cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug .. && $(MAKE) -j12 -s

release:
	-mkdir build
	cd build && cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release .. && $(MAKE) -j12 -s

clear:
	-mv build/compile_commands.json compile_commands.json
	-rmdir /s /q build
	-mkdir build
	-mv compile_commands.json build/compile_commands.json

run:
	cd bin && ./main.exe

endif

clean:
	$(MAKE) -s -C build clean