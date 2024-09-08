from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, cmake_layout


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def build_requirements(self):
        self.tool_requires("doxygen/1.9.4")
        self.tool_requires("ninja/1.12.1")
        self.test_requires("gtest/1.15.0")

    def requirements(self):
        self.requires("ogg/1.3.5")
        self.requires("libsndfile/1.2.2")
        self.requires("vorbis/1.3.7")

    def layout(self):
        cmake_layout(self)
