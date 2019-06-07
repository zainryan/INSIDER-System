#include <insider_itc.h>

#include "constant.h"
#include "structure.h"

#include "app_grep_input_preprocessor.cpp"
#include "app_grep_matcher.cpp"
#include "app_grep_reducer.cpp"
#include "app_grep_writer.cpp"

void interconnects() {

  ST_Queue<Grep_Input_Data> app_grep_input_data_chan(16);
  ST_Queue<Matched_Pos> app_grep_matched_pos_chan(64);
  ST_Queue<Matching_Vec> app_grep_matching_vecs(16);
  ST_Queue<Matching_Vec> app_grep_scanned_matching_vecs(16);

  app_grep_input_preprocessor(app_input_data, app_grep_input_data_chan);
  app_grep_matcher(app_input_params, app_grep_input_data_chan,
                   app_grep_matching_vecs);
  app_grep_reducer(app_grep_matching_vecs, app_grep_matched_pos_chan);
  app_grep_writer(app_output_data, app_grep_matched_pos_chan);
}
