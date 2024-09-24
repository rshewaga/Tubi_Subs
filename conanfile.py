from conan import ConanFile


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("spdlog/1.14.1")
        self.requires("lyra/1.6.1")
        self.requires("nlohmann_json/3.11.3")