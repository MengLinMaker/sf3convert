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
	cmd build/generators/conanbuild.bat
	cmd build/generators/conanrun.bat
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

#=============================================================================
# DOC
#=============================================================================

doc:
	doxygen
