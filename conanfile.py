from conan import ConanFile
from conan.tools.cmake import cmake_layout


class CompressorRecipe(ConanFile):
    settings = {
        "os",
        "compiler",
        "arch",
        "build_type",
    }
    generators = "CMakeToolchain", "CMakeDeps"

    def build(self):
        if self.settings.compiler == 'msvc':
            self.settings.compiler.runtime="static"

    def build_requirements(self):
        self.tool_requires("doxygen/1.9.4")
        self.tool_requires("ninja/1.12.1")
        self.test_requires("gtest/1.15.0")

    def requirements(self):
        self.requires("cli11/2.4.2")
        self.requires("vorbis/1.3.7")

    def layout(self):
        cmake_layout(self)
