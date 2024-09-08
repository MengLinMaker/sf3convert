#=============================================================================
#  MuseScore sftools
#
#  Copyright (C) 2011-2014 Werner Schweer && Meng Lin
#
#  This work is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Library General Public
#  License as published by the Free Software Foundation; either
#  version 2 of the License, or (at your option) any later version.
#
#  See LICENCE for the licence text and disclaimer of warranty.
#=============================================================================

install:
	conan profile detect; \
	conan install . --build=missing

clean:
	rm -rf build/Build test/piano.sf3

release:
	cmake -B build/Build --preset release
	ninja -C build/Build

check:
	build/Build/sf3convert test/sample.sf2 test/sample.sf3

compose:
	docker-compose up --build
