
add_rules("mode.debug", "mode.release")
add_requires(
    "pdcursesmod", {
        configs = {
            port = "wincon"
        }
    })

target("mini_lisp")
add_packages("pdcursesmod")
add_files("src/*.cpp")
set_languages("c++20")
set_targetdir("bin")
add_cxflags("-Wall -Wextra -Wno-potentially-evaluated-expression -Wno-unused-parameter")

if is_plat("windows") then
    add_cxflags("/utf-8", "/Zc:preprocessor")
end
