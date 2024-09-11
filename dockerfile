FROM python:3.12.5-slim-bookworm

# Conan c++ package manager
RUN pip install conan --upgrade

# Overcome bad proxy docker
RUN echo "Acquire::http::Pipeline-Depth 0;" > /etc/apt/apt.conf.d/99custom && \
    echo "Acquire::http::No-Cache true;" >> /etc/apt/apt.conf.d/99custom && \
    echo "Acquire::BrokenProxy    true;" >> /etc/apt/apt.conf.d/99custom && \
	apt update -y --fix-missing && \
	# Conan peer dependencies: make, cmake & gcc
	apt install -y --fix-missing make cmake build-essential

# Install dependencies
COPY conanfile.py ./
RUN conan profile detect; \
	conan install . --build=missing -s compiler.cppstd=23

# Build c++ application
COPY CMakeUserPresets.json CMakeLists.txt Makefile ./
COPY src ./src
RUN make prod

# Conversion test
COPY test/sample.sf2 ./test
RUN make test-prod
