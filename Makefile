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
	ninja -C build/Prod

windows:
	cmake --preset conan-default
	cmake --build build

#=============================================================================
# TEST
#=============================================================================

# Test release build script locally
compose:
	docker-compose up --build

test-prod:
	build/Prod/sf3convert test/sample.sf2 test/sample-prod.sf3

#=============================================================================
# DOC
#=============================================================================

doc:
	doxygen
