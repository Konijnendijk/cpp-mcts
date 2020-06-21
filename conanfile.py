from conans import ConanFile


class CPPMCTSConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "catch2/2.12.2"
    generators = "cmake"
