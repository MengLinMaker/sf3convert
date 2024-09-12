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
	sh build/generators/conanbuild.sh
	sh build/generators/conanrun.sh
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
