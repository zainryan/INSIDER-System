open_project interconnect
set_top interconnect
add_files interconnect.cpp
open_solution "solution1"
set_part {xcvu9p-flgb2104-2-i}
create_clock -period 4 -name default
set_clock_uncertainty 0.7 default
csynth_design
