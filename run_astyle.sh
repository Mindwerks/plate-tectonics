astyle examples/*.hpp
astyle examples/*.cpp
rm examples/*.orig -f
astyle src/*.hpp
astyle src/*.cpp
rm src/*.orig -f
astyle test/*.cpp
rm test/*.orig -f
astyle pybindings/platec_src/*.cpp
rm pybindings/platec_src/*.orig -f
