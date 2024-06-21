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

CPUS = 2

release:
	if test ! -d build.release; then mkdir build.release; fi; \
  cd build.release; \
  cmake -DCMAKE_BUILD_TYPE=RELEASE ..; \
  make VERBOSE=1 -j ${CPUS}

debug:
	if test ! -d build.debug; then mkdir build.debug; fi; \
  cd build.debug ; \
  cmake -DCMAKE_BUILD_TYPE=DEBUG ..; \
  make VERBOSE=1 -j ${CPUS}

clean:
	rm -rf build.debug build.release
