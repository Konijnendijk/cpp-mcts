from conans import ConanFile, CMake


class CPPMCTSConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = ["catch2/2.12.2", "qt/5.12.9@bincrafters/stable"]
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        cmake.definitions["CMAKE_EXPORT_COMPILE_COMMANDS"] = "TRUE"
        cmake.configure()
        self.run("cmake -DBUILD_TESTING=TRUE . {}".format(cmake.command_line))
        self.run("cmake --build . {}".format(cmake.build_config))
        cmake.test(args=['--', 'ARGS=-T Test'])
