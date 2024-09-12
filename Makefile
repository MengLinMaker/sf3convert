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
	cmake -B build/Prod --preset prod
	make -C build/Prod

dev:
	cmake -B build/Dev --preset dev
	ninja -C build/Dev

windows:
	cmake --preset conan-default
	make -C build

#=============================================================================
# TEST
#=============================================================================

# Test release build script locally
compose:
	docker-compose up --build

test-prod:
	build/Prod/sf3convert test/sample.sf2 test/sample-release.sf3

test-dev:
	build/Dev/sf3convert test/sample.sf2 test/sample-dev.sf3

#=============================================================================
# DOC
#=============================================================================

doc:
	doxygen
