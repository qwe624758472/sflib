set_languages("c++20")

target("http_client")
    set_kind("binary")
    add_includedirs("../")
    add_files("http_client/*.cpp")
