#=============================================================================
# SETUP
#=============================================================================

clean:
	rm -rf build test/*.sf3

install:
	conan profile detect; \
	conan install . --build=missing -s compiler.cppstd=23

#=============================================================================
# BUILD
#=============================================================================

prod:
ifeq ($(OS),Windows_NT)
	cmake --preset conan-default
	cmake --build build --config Release
	ren "\build\Release" "\build\Prod"
else
	cmake -B build/Prod --preset prod
	ninja -C build/Prod
endif

#=============================================================================
# TEST
#=============================================================================

test-prod:
	build/Prod/sf3convert convert test/sample.sf2 test/sample-prod.sf3
	build/Prod/sf3convert preset test/sample.sf2

#=============================================================================
# DOC
#=============================================================================

doc:
	doxygen
