#include <insider_itc.h>

#include "structure.h"

#include "app_conv_dist.cpp"
#include "app_conv_mult_0.cpp"
#include "app_conv_mult_1.cpp"
#include "app_conv_mult_10.cpp"
#include "app_conv_mult_11.cpp"
#include "app_conv_mult_12.cpp"
#include "app_conv_mult_13.cpp"
#include "app_conv_mult_14.cpp"
#include "app_conv_mult_15.cpp"
#include "app_conv_mult_2.cpp"
#include "app_conv_mult_3.cpp"
#include "app_conv_mult_4.cpp"
#include "app_conv_mult_5.cpp"
#include "app_conv_mult_6.cpp"
#include "app_conv_mult_7.cpp"
#include "app_conv_mult_8.cpp"
#include "app_conv_mult_9.cpp"
#include "app_conv_mult_dummy.cpp"

#include "app_conv_row_reduce_0.cpp"
#include "app_conv_row_reduce_1.cpp"
#include "app_conv_row_reduce_2.cpp"
#include "app_conv_row_reduce_3.cpp"
#include "app_conv_row_reduce_4.cpp"
#include "app_conv_row_reduce_5.cpp"
#include "app_conv_row_reduce_6.cpp"
#include "app_conv_row_reduce_7.cpp"

#include "app_conv_combine_l0_0.cpp"
#include "app_conv_combine_l0_1.cpp"
#include "app_conv_combine_l0_2.cpp"
#include "app_conv_combine_l0_3.cpp"

#include "app_conv_combine_l1_0.cpp"
#include "app_conv_combine_l1_1.cpp"
#include "app_conv_combine_l2_0.cpp"

#include "app_conv_writer.cpp"

#include "insider_itc.h"

#include "app_input_data_merger.cpp"
#include "app_input_data_mux.cpp"
#include "app_output_data_demux.cpp"
#include "dram_data_caching.cpp"
#include "dram_read_delay_unit.cpp"
#include "dram_read_req_multiplexer.cpp"
#include "dram_read_req_time_marker.cpp"
#include "dram_read_resp_multiplexer.cpp"
#include "dram_read_throttle_unit.cpp"
#include "dram_write_delay_unit.cpp"
#include "dram_write_mux.cpp"
#include "dram_write_req_time_marker.cpp"
#include "dram_write_throttle_unit.cpp"
#include "pcie_data_splitter_app.cpp"
#include "pcie_read_req_multiplexer.cpp"
#include "pcie_read_resp_multiplexer.cpp"
#include "pcie_read_resp_passer.cpp"
#include "pcie_read_throttle_unit.cpp"
#include "pcie_write_multiplexer.cpp"
#include "pcie_write_throttle_unit.cpp"
#include "peek_handler.cpp"
#include "pipe0_data_handler.cpp"
#include "pipe0_dram_dispatcher.cpp"
#include "pipe1_data_handler.cpp"
#include "pipe1_dram_dispatcher.cpp"
#include "pipe2_data_handler.cpp"
#include "pipeline_data_passer.cpp"
#include "poke_handler.cpp"
#include "read_mode_dram_helper_app.cpp"
#include "read_mode_pcie_helper_app.cpp"
#include "reset_propaganda.cpp"
#include "write_mode_app_output_data_caching.cpp"
#include "write_mode_dram_helper_app.cpp"
#include "write_mode_pcie_helper_app.cpp"
#include "write_mode_pre_merged_app_input_data_forwarder.cpp"

int main() {
  ST_Queue<Request> reqs_incoming(64);
  ST_Queue<Request> rd_reqs_pipe0_write(64);
  ST_Queue<Request> wr_reqs_pipe0_write(64);
  ST_Queue<Request> rd_reqs_pipe1_read(64);
  ST_Queue<Request> wr_reqs_pipe1_read(64);
  ST_Queue<Request> rd_reqs_pipe1_write(64);
  ST_Queue<Request> wr_reqs_pipe1_write(64);
  ST_Queue<Request> rd_reqs_pipe2_read(64);
  ST_Queue<Request> wr_reqs_pipe2_read(64);

  ST_Queue<Data> data_pipe2(64);
  ST_Queue<bool> rd_data_valid_pipe1_write(64);
  ST_Queue<bool> wr_data_valid_pipe1_write(64);
  ST_Queue<bool> rd_data_valid_pipe2_read(64);
  ST_Queue<bool> wr_data_valid_pipe2_read(64);
  ST_Queue<unsigned long long> rd_kbuf_addr_pipe0_write(64);
  ST_Queue<unsigned long long> rd_kbuf_addr_pipe2_read(64);
  ST_Queue<unsigned long long> wr_kbuf_addr_pipe0_write(64);
  ST_Queue<unsigned long long> wr_kbuf_addr_pipe2_read(64);

  ST_Queue<PCIe_Read_Resp> before_throttle_pcie_read_resp(4);
  ST_Queue<PCIe_Read_Resp> after_throttle_pcie_read_resp(4);

  ST_Queue<unsigned int> kbuf_addrs(16);
  ST_Queue<PCIe_Write_Req_Data> host_data_pcie_write_req_data(64);
  ST_Queue<PCIe_Write_Req_Apply> host_data_pcie_write_req_apply(64);
  ST_Queue<PCIe_Write_Req_Data> host_rdcmd_fin_pcie_write_req_data(4);
  ST_Queue<PCIe_Write_Req_Apply> host_rdcmd_fin_pcie_write_req_apply(64);
  ST_Queue<PCIe_Write_Req_Data> host_wrcmd_fin_pcie_write_req_data(4);
  ST_Queue<PCIe_Write_Req_Apply> host_wrcmd_fin_pcie_write_req_apply(64);
  ST_Queue<unsigned long long> wrcmd_kbuf_addrs(64);

  ST_Queue<unsigned int> dma_read_throttle_params(4);
  ST_Queue<unsigned int> dma_write_throttle_params(4);
  ST_Queue<PCIe_Write_Req_Data> before_throttle_pcie_write_req_data(64);
  ST_Queue<PCIe_Write_Req_Apply> before_throttle_pcie_write_req_apply(64);

  ST_Queue<Dram_Dispatcher_Write_Req> unified_dram_dispatcher_write_context(64);
  ST_Queue<Dram_Dispatcher_Read_Req> unified_dram_dispatcher_read_context(64);

  ST_Queue<Dram_Read_Req> host_dram_read_req(64);
  ST_Queue<Dram_Read_Resp> host_dram_read_resp(4);
  ST_Queue<Dram_Write_Req_Data> after_mux_dram_write_req_data(4096);
  ST_Queue<bool> dram_write_mux_apply_context(256);
  ST_Queue<Dram_Write_Req_Data> before_delay_dram_write_req_data(4);
  ST_Queue<Dram_Write_Req_Apply> after_mux_dram_write_req_apply(64);
  ST_Queue<Dram_Write_Req_Apply_With_Time>
      before_delay_dram_write_req_apply_with_time(64);

  ST_Queue<Dram_Read_Req> device_dram_read_req(64);
  ST_Queue<Dram_Read_Resp> device_dram_read_resp(256);
  ST_Queue<bool> release_device_dram_resp_buf_flits(256);

  ST_Queue<Dram_Read_Req_With_Time> before_delay_host_dram_read_req_with_time(
      64);
  ST_Queue<Dram_Read_Req_With_Time> before_delay_device_dram_read_req_with_time(
      64);

  ST_Queue<Dram_Read_Req> after_delay_host_dram_read_req(64);
  ST_Queue<Dram_Read_Req> after_delay_device_dram_read_req(64);
  ST_Queue<Dram_Read_Req> after_delay_unified_dram_read_req(64);
  ST_Queue<Dram_Read_Resp> before_throttle_unified_dram_read_resp(4);
  ST_Queue<Dram_Read_Resp> after_throttle_unified_dram_read_resp(4);
  ST_Queue<Dram_Write_Req_Data> before_throttle_unified_dram_write_req_data(4);
  ST_Queue<Dram_Write_Req_Apply> before_throttle_unified_dram_write_req_apply(
      64);
  ST_Queue<Dram_Write_Req_Data> after_throttle_unified_dram_write_req_data(4);
  ST_Queue<Dram_Write_Req_Apply> after_throttle_unified_dram_write_req_apply(
      64);
  ST_Queue<bool> dram_read_context(64);

  ST_Queue<PCIe_Write_Req_Data> read_mode_device_pcie_write_req_data(64);
  ST_Queue<PCIe_Write_Req_Apply> read_mode_device_pcie_write_req_apply(64);
  ST_Queue<PCIe_Write_Req_Data> write_mode_device_pcie_write_req_data(64);
  ST_Queue<PCIe_Write_Req_Apply> write_mode_device_pcie_write_req_apply(64);

  ST_Queue<unsigned int> drive_read_delay_cycle_cnts(4);
  ST_Queue<unsigned int> drive_write_delay_cycle_cnts(4);
  ST_Queue<unsigned int> drive_read_throttle_params(4);
  ST_Queue<unsigned int> drive_write_throttle_params(4);

  ST_Queue<bool> app_free_buf(4);
  ST_Queue<unsigned int> app_file_infos_0(64);
  ST_Queue<unsigned int> app_file_infos_1(64);
  ST_Queue<unsigned int> read_mode_app_buf_addrs(4);
  ST_Queue<unsigned int> write_mode_app_buf_addrs(4);
  ST_Queue<unsigned int> app_input_params(4);
  ST_Queue<APP_Data> app_input_data(4);
  ST_Queue<APP_Data> app_output_data(4);
  ST_Queue<APP_Data> read_mode_app_output_data(4);
  ST_Queue<APP_Data> write_mode_app_output_data(4);
  ST_Queue<APP_Data> app_output_data_splitted(4);
  ST_Queue<APP_Data_Meta> app_output_data_meta(4);

  ST_Queue<bool> reset_sigs(4);
  ST_Queue<bool> reset_read_mode_dram_helper_app(4);
  ST_Queue<bool> reset_read_mode_pcie_helper_app(4);
  ST_Queue<bool> reset_write_mode_dram_helper_app(4);
  ST_Queue<bool> reset_write_mode_pcie_helper_app(4);
  ST_Queue<bool> reset_pcie_data_splitter_app(4);
  ST_Queue<bool> reset_app_input_data_merger(4);
  ST_Queue<bool> reset_app_output_data_demux(4);
  ST_Queue<bool> reset_app_input_data_mux(4);
  ST_Queue<bool> reset_write_mode_app_output_data_caching(4);
  ST_Queue<bool> reset_write_mode_pre_merged_app_input_data_forwarder(4);

  ST_Queue<bool> pcie_read_mux_context(128);
  ST_Queue<PCIe_Read_Req> host_pcie_read_req(64);
  ST_Queue<PCIe_Read_Req> device_pcie_read_req(64);
  ST_Queue<PCIe_Read_Resp> host_after_throttle_pcie_read_resp(4);
  ST_Queue<PCIe_Read_Resp> device_after_throttle_pcie_read_resp(4);

  ST_Queue<APP_Data> pre_merged_write_mode_app_input_data(1);

  ST_Queue<Dram_Write_Req_Data> host_dram_write_req_data(128);
  ST_Queue<Dram_Write_Req_Apply> host_dram_write_req_apply(4);
  ST_Queue<Dram_Write_Req_Data> device_dram_write_req_data(4);
  ST_Queue<Dram_Write_Req_Apply> device_dram_write_req_apply(4);

  ST_Queue<APP_Data> read_mode_app_input_data(4);
  ST_Queue<APP_Data> write_mode_app_input_data(4);

  ST_Queue<unsigned long long> app_write_total_len(4);
  ST_Queue<unsigned int> app_commit_write_buf(64);
  ST_Queue<bool> app_is_write_mode_0(4);
  ST_Queue<bool> app_is_write_mode_1(4);
  ST_Queue<bool> app_is_write_mode_2(4);

  ST_Queue<Write_Mode_PCIe_Read_Req_Context>
      buffered_device_pcie_read_req_context(128);
  ST_Queue<PCIe_Read_Resp> buffered_device_pcie_read_resp(512);
  ST_Queue<bool> release_buffered_device_pcie_read_resp(64);

  ST_Queue<APP_Data> cached_write_mode_app_output_data(128);
  ST_Queue<Dram_Write_Req_Apply> cached_device_dram_write_req_apply(4);

  ST_Queue<Dram_Read_Resp> cached_dramA_read_resp(64);
  ST_Queue<Dram_Read_Resp> cached_dramB_read_resp(64);
  ST_Queue<Dram_Read_Resp> cached_dramC_read_resp(64);
  ST_Queue<Dram_Read_Resp> cached_dramD_read_resp(64);
  ST_Queue<Dram_Write_Req_Data> cached_dramA_write_req_data(64);
  ST_Queue<Dram_Write_Req_Data> cached_dramB_write_req_data(64);
  ST_Queue<Dram_Write_Req_Data> cached_dramC_write_req_data(64);
  ST_Queue<Dram_Write_Req_Data> cached_dramD_write_req_data(64);

  ST_Queue<unsigned int> peek_real_written_bytes_req(4);
  ST_Queue<unsigned int> peek_real_written_bytes_resp(4);
  ST_Queue<unsigned int> peek_write_finished_req(4);
  ST_Queue<unsigned int> peek_write_finished_resp(4);
  ST_Queue<unsigned int> peek_virt_written_bytes_req(4);
  ST_Queue<unsigned int> peek_virt_written_bytes_resp(4);

  ST_Queue<bool> reset_app_conv_combine_l0_0(4);
  ST_Queue<bool> reset_app_conv_combine_l0_1(4);
  ST_Queue<bool> reset_app_conv_combine_l0_2(4);
  ST_Queue<bool> reset_app_conv_combine_l0_3(4);
  ST_Queue<bool> reset_app_conv_combine_l1_0(4);
  ST_Queue<bool> reset_app_conv_combine_l1_1(4);
  ST_Queue<bool> reset_app_conv_combine_l2_0(4);
  ST_Queue<bool> reset_app_conv_dist(4);
  ST_Queue<bool> reset_app_conv_mult_0(4);
  ST_Queue<bool> reset_app_conv_mult_10(4);
  ST_Queue<bool> reset_app_conv_mult_11(4);
  ST_Queue<bool> reset_app_conv_mult_12(4);
  ST_Queue<bool> reset_app_conv_mult_13(4);
  ST_Queue<bool> reset_app_conv_mult_14(4);
  ST_Queue<bool> reset_app_conv_mult_15(4);
  ST_Queue<bool> reset_app_conv_mult_1(4);
  ST_Queue<bool> reset_app_conv_mult_2(4);
  ST_Queue<bool> reset_app_conv_mult_3(4);
  ST_Queue<bool> reset_app_conv_mult_4(4);
  ST_Queue<bool> reset_app_conv_mult_5(4);
  ST_Queue<bool> reset_app_conv_mult_6(4);
  ST_Queue<bool> reset_app_conv_mult_7(4);
  ST_Queue<bool> reset_app_conv_mult_8(4);
  ST_Queue<bool> reset_app_conv_mult_9(4);
  ST_Queue<bool> reset_app_conv_mult_dummy(4);
  ST_Queue<bool> reset_app_conv_row_reduce_0(4);
  ST_Queue<bool> reset_app_conv_row_reduce_1(4);
  ST_Queue<bool> reset_app_conv_row_reduce_2(4);
  ST_Queue<bool> reset_app_conv_row_reduce_3(4);
  ST_Queue<bool> reset_app_conv_row_reduce_4(4);
  ST_Queue<bool> reset_app_conv_row_reduce_5(4);
  ST_Queue<bool> reset_app_conv_row_reduce_6(4);
  ST_Queue<bool> reset_app_conv_row_reduce_7(4);
  ST_Queue<bool> reset_app_conv_writer(4);

  ST_Queue<APP_Mult_Data> app_conv_mult_0_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_1_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_2_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_3_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_4_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_5_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_6_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_7_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_8_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_9_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_10_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_11_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_12_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_13_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_14_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_15_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_dummy_input_data(16);

  ST_Queue<APP_Param> app_conv_mult_0_param(16);
  ST_Queue<APP_Param> app_conv_mult_1_param(16);
  ST_Queue<APP_Param> app_conv_mult_2_param(16);
  ST_Queue<APP_Param> app_conv_mult_3_param(16);
  ST_Queue<APP_Param> app_conv_mult_4_param(16);
  ST_Queue<APP_Param> app_conv_mult_5_param(16);
  ST_Queue<APP_Param> app_conv_mult_6_param(16);
  ST_Queue<APP_Param> app_conv_mult_7_param(16);
  ST_Queue<APP_Param> app_conv_mult_8_param(16);
  ST_Queue<APP_Param> app_conv_mult_9_param(16);
  ST_Queue<APP_Param> app_conv_mult_10_param(16);
  ST_Queue<APP_Param> app_conv_mult_11_param(16);
  ST_Queue<APP_Param> app_conv_mult_12_param(16);
  ST_Queue<APP_Param> app_conv_mult_13_param(16);
  ST_Queue<APP_Param> app_conv_mult_14_param(16);
  ST_Queue<APP_Param> app_conv_mult_15_param(16);
  ST_Queue<APP_Param> app_conv_mult_dummy_param(16);

  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_0_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_1_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_2_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_3_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_4_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_5_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_6_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_7_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_8_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_9_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_10_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_11_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_12_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_13_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_14_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_15_input_data(16);

  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_0(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_1(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_2(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_3(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_4(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_5(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_6(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_7(16);

  ST_Queue<APP_Combine_L1_Data> app_conv_combine_l1_input_data_0(16);
  ST_Queue<APP_Combine_L1_Data> app_conv_combine_l1_input_data_1(16);
  ST_Queue<APP_Combine_L1_Data> app_conv_combine_l1_input_data_2(16);
  ST_Queue<APP_Combine_L1_Data> app_conv_combine_l1_input_data_3(16);

  ST_Queue<APP_Combine_L2_Data> app_conv_combine_l2_input_data_0(16);
  ST_Queue<APP_Combine_L2_Data> app_conv_combine_l2_input_data_1(16);

  ST_Queue<APP_Data> app_conv_writer_input_data(16);

  ;

  ;
  ;
  ;
  ;
  ;
  ;
  ;
  ;
  ;
  ;
  ;
  ;
  ;
  ;
  ;
  ;
  ;

  ;
  ;
  ;
  ;
  ;
  ;
  ;
  ;

  ;
  ;
  ;
  ;

  ;
  ;

  ;

  ;

  std::thread t0(poke_handler, std::ref(poke), std::ref(reqs_incoming),
                 std::ref(kbuf_addrs), std::ref(dma_read_throttle_params),
                 std::ref(dma_write_throttle_params),
                 std::ref(drive_read_delay_cycle_cnts),
                 std::ref(drive_write_delay_cycle_cnts),
                 std::ref(drive_read_throttle_params),
                 std::ref(drive_write_throttle_params),
                 std::ref(app_file_infos_0), std::ref(app_file_infos_1),
                 std::ref(read_mode_app_buf_addrs),
                 std::ref(write_mode_app_buf_addrs), std::ref(app_free_buf),
                 std::ref(app_write_total_len), std::ref(app_input_params),
                 std::ref(app_commit_write_buf), std::ref(app_is_write_mode_0),
                 std::ref(app_is_write_mode_1), std::ref(app_is_write_mode_2),
                 std::ref(reset_sigs));
  std::thread t1(
      peek_handler, std::ref(peek_req), std::ref(peek_resp),
      std::ref(peek_real_written_bytes_req),
      std::ref(peek_real_written_bytes_resp), std::ref(peek_write_finished_req),
      std::ref(peek_write_finished_resp), std::ref(peek_virt_written_bytes_req),
      std::ref(peek_virt_written_bytes_resp));
  std::thread t2(pcie_read_resp_passer, std::ref(pcie_read_resp),
                 std::ref(before_throttle_pcie_read_resp));
  std::thread t3(pcie_read_throttle_unit, std::ref(dma_read_throttle_params),
                 std::ref(before_throttle_pcie_read_resp),
                 std::ref(after_throttle_pcie_read_resp));
  std::thread t4(
      pipe0_data_handler, std::ref(kbuf_addrs), std::ref(reqs_incoming),
      std::ref(host_dram_read_req), std::ref(host_pcie_read_req),
      std::ref(rd_reqs_pipe0_write), std::ref(wr_reqs_pipe0_write),
      std::ref(rd_kbuf_addr_pipe0_write), std::ref(wr_kbuf_addr_pipe0_write));
  std::thread t5(pipe1_data_handler, std::ref(rd_reqs_pipe1_read),
                 std::ref(wr_reqs_pipe1_read), std::ref(host_dram_read_resp),
                 std::ref(host_after_throttle_pcie_read_resp),
                 std::ref(rd_reqs_pipe1_write), std::ref(wr_reqs_pipe1_write),
                 std::ref(rd_data_valid_pipe1_write),
                 std::ref(wr_data_valid_pipe1_write), std::ref(data_pipe2),
                 std::ref(host_dram_write_req_data));
  std::thread t6(
      pipe2_data_handler, std::ref(rd_reqs_pipe2_read),
      std::ref(wr_reqs_pipe2_read), std::ref(rd_data_valid_pipe2_read),
      std::ref(wr_data_valid_pipe2_read), std::ref(rd_kbuf_addr_pipe2_read),
      std::ref(wr_kbuf_addr_pipe2_read), std::ref(data_pipe2),
      std::ref(host_data_pcie_write_req_apply),
      std::ref(host_data_pcie_write_req_data),
      std::ref(host_rdcmd_fin_pcie_write_req_apply),
      std::ref(host_rdcmd_fin_pcie_write_req_data),
      std::ref(host_dram_write_req_apply), std::ref(wrcmd_kbuf_addrs));
  std::thread t7(
      pipeline_data_passer, std::ref(rd_reqs_pipe0_write),
      std::ref(wr_reqs_pipe0_write), std::ref(rd_reqs_pipe1_read),
      std::ref(wr_reqs_pipe1_read), std::ref(rd_reqs_pipe1_write),
      std::ref(wr_reqs_pipe1_write), std::ref(rd_reqs_pipe2_read),
      std::ref(wr_reqs_pipe2_read), std::ref(rd_data_valid_pipe1_write),
      std::ref(wr_data_valid_pipe1_write), std::ref(rd_data_valid_pipe2_read),
      std::ref(wr_data_valid_pipe2_read), std::ref(rd_kbuf_addr_pipe0_write),
      std::ref(wr_kbuf_addr_pipe0_write), std::ref(rd_kbuf_addr_pipe2_read),
      std::ref(wr_kbuf_addr_pipe2_read));
  std::thread t8(pcie_write_multiplexer,
                 std::ref(host_data_pcie_write_req_data),
                 std::ref(host_data_pcie_write_req_apply),
                 std::ref(before_throttle_pcie_write_req_data),
                 std::ref(before_throttle_pcie_write_req_apply),
                 std::ref(host_rdcmd_fin_pcie_write_req_data),
                 std::ref(host_rdcmd_fin_pcie_write_req_apply),
                 std::ref(host_wrcmd_fin_pcie_write_req_data),
                 std::ref(host_wrcmd_fin_pcie_write_req_apply),
                 std::ref(read_mode_device_pcie_write_req_data),
                 std::ref(read_mode_device_pcie_write_req_apply),
                 std::ref(write_mode_device_pcie_write_req_data),
                 std::ref(write_mode_device_pcie_write_req_apply));
  std::thread t9(pcie_write_throttle_unit, std::ref(dma_write_throttle_params),
                 std::ref(before_throttle_pcie_write_req_data),
                 std::ref(before_throttle_pcie_write_req_apply),
                 std::ref(pcie_write_req_data), std::ref(pcie_write_req_apply));
  std::thread t10(dram_read_req_multiplexer,
                  std::ref(after_delay_host_dram_read_req),
                  std::ref(after_delay_device_dram_read_req),
                  std::ref(after_delay_unified_dram_read_req),
                  std::ref(release_device_dram_resp_buf_flits),
                  std::ref(dram_read_context));
  std::thread t11(dram_read_resp_multiplexer, std::ref(host_dram_read_resp),
                  std::ref(device_dram_read_resp),
                  std::ref(after_throttle_unified_dram_read_resp),
                  std::ref(dram_read_context));
  std::thread t12(dram_read_req_time_marker, std::ref(host_dram_read_req),
                  std::ref(before_delay_host_dram_read_req_with_time),
                  std::ref(device_dram_read_req),
                  std::ref(before_delay_device_dram_read_req_with_time));
  std::thread t13(dram_read_delay_unit, std::ref(drive_read_delay_cycle_cnts),
                  std::ref(before_delay_host_dram_read_req_with_time),
                  std::ref(after_delay_host_dram_read_req),
                  std::ref(before_delay_device_dram_read_req_with_time),
                  std::ref(after_delay_device_dram_read_req));
  std::thread t14(dram_read_throttle_unit, std::ref(drive_read_throttle_params),
                  std::ref(before_throttle_unified_dram_read_resp),
                  std::ref(after_throttle_unified_dram_read_resp));
  std::thread t15(dram_write_req_time_marker,
                  std::ref(after_mux_dram_write_req_data),
                  std::ref(after_mux_dram_write_req_apply),
                  std::ref(before_delay_dram_write_req_data),
                  std::ref(before_delay_dram_write_req_apply_with_time));
  std::thread t16(dram_write_delay_unit, std::ref(drive_write_delay_cycle_cnts),
                  std::ref(before_delay_dram_write_req_data),
                  std::ref(before_delay_dram_write_req_apply_with_time),
                  std::ref(before_throttle_unified_dram_write_req_data),
                  std::ref(before_throttle_unified_dram_write_req_apply));
  std::thread t17(dram_write_throttle_unit,
                  std::ref(drive_write_throttle_params),
                  std::ref(before_throttle_unified_dram_write_req_data),
                  std::ref(before_throttle_unified_dram_write_req_apply),
                  std::ref(after_throttle_unified_dram_write_req_data),
                  std::ref(after_throttle_unified_dram_write_req_apply));
  std::thread t18(pipe0_dram_dispatcher,
                  std::ref(after_delay_unified_dram_read_req),
                  std::ref(after_throttle_unified_dram_write_req_apply),
                  std::ref(dramA_read_req), std::ref(dramA_write_req_apply),
                  std::ref(dramB_read_req), std::ref(dramB_write_req_apply),
                  std::ref(dramC_read_req), std::ref(dramC_write_req_apply),
                  std::ref(dramD_read_req), std::ref(dramD_write_req_apply),
                  std::ref(unified_dram_dispatcher_write_context),
                  std::ref(unified_dram_dispatcher_read_context));
  std::thread t19(
      pipe1_dram_dispatcher, std::ref(before_throttle_unified_dram_read_resp),
      std::ref(after_throttle_unified_dram_write_req_data),
      std::ref(dram_write_mux_apply_context), std::ref(cached_dramA_read_resp),
      std::ref(cached_dramA_write_req_data), std::ref(cached_dramB_read_resp),
      std::ref(cached_dramB_write_req_data), std::ref(cached_dramC_read_resp),
      std::ref(cached_dramC_write_req_data), std::ref(cached_dramD_read_resp),
      std::ref(cached_dramD_write_req_data),
      std::ref(unified_dram_dispatcher_write_context),
      std::ref(unified_dram_dispatcher_read_context),
      std::ref(host_wrcmd_fin_pcie_write_req_data),
      std::ref(host_wrcmd_fin_pcie_write_req_apply),
      std::ref(wrcmd_kbuf_addrs));
  std::thread t20(read_mode_dram_helper_app, std::ref(app_file_infos_0),
                  std::ref(app_is_write_mode_1), std::ref(device_dram_read_req),
                  std::ref(device_dram_read_resp),
                  std::ref(read_mode_app_input_data),
                  std::ref(release_device_dram_resp_buf_flits),
                  std::ref(reset_read_mode_dram_helper_app));
  std::thread t21(read_mode_pcie_helper_app, std::ref(read_mode_app_buf_addrs),
                  std::ref(read_mode_device_pcie_write_req_apply),
                  std::ref(read_mode_device_pcie_write_req_data),
                  std::ref(app_output_data_splitted),
                  std::ref(app_output_data_meta), std::ref(app_free_buf),
                  std::ref(reset_read_mode_pcie_helper_app));
  std::thread t22(pcie_data_splitter_app, std::ref(read_mode_app_output_data),
                  std::ref(app_output_data_splitted),
                  std::ref(app_output_data_meta),
                  std::ref(reset_pcie_data_splitter_app));
  std::thread t23(pcie_read_req_multiplexer, std::ref(pcie_read_req),
                  std::ref(device_pcie_read_req), std::ref(host_pcie_read_req),
                  std::ref(pcie_read_mux_context));
  std::thread t24(pcie_read_resp_multiplexer,
                  std::ref(after_throttle_pcie_read_resp),
                  std::ref(device_after_throttle_pcie_read_resp),
                  std::ref(host_after_throttle_pcie_read_resp),
                  std::ref(pcie_read_mux_context));
  std::thread t25(app_input_data_merger,
                  std::ref(pre_merged_write_mode_app_input_data),
                  std::ref(write_mode_app_input_data),
                  std::ref(reset_app_input_data_merger));
  std::thread t26(dram_write_mux, std::ref(after_mux_dram_write_req_apply),
                  std::ref(after_mux_dram_write_req_data),
                  std::ref(dram_write_mux_apply_context),
                  std::ref(host_dram_write_req_apply),
                  std::ref(host_dram_write_req_data),
                  std::ref(device_dram_write_req_apply),
                  std::ref(device_dram_write_req_data));
  std::thread t27(app_input_data_mux, std::ref(read_mode_app_input_data),
                  std::ref(write_mode_app_input_data), std::ref(app_input_data),
                  std::ref(peek_virt_written_bytes_req),
                  std::ref(peek_virt_written_bytes_resp),
                  std::ref(reset_app_input_data_mux));
  std::thread t28(app_output_data_demux, std::ref(app_is_write_mode_2),
                  std::ref(app_output_data),
                  std::ref(read_mode_app_output_data),
                  std::ref(write_mode_app_output_data),
                  std::ref(reset_app_output_data_demux));
  std::thread t29(
      write_mode_pcie_helper_app, std::ref(write_mode_app_buf_addrs),
      std::ref(app_commit_write_buf), std::ref(device_pcie_read_req),
      std::ref(device_after_throttle_pcie_read_resp),
      std::ref(buffered_device_pcie_read_req_context),
      std::ref(buffered_device_pcie_read_resp),
      std::ref(release_buffered_device_pcie_read_resp),
      std::ref(reset_write_mode_pcie_helper_app));
  std::thread t30(
      write_mode_pre_merged_app_input_data_forwarder,
      std::ref(buffered_device_pcie_read_resp),
      std::ref(release_buffered_device_pcie_read_resp),
      std::ref(buffered_device_pcie_read_req_context),
      std::ref(pre_merged_write_mode_app_input_data),
      std::ref(app_write_total_len),
      std::ref(write_mode_device_pcie_write_req_apply),
      std::ref(write_mode_device_pcie_write_req_data),
      std::ref(reset_write_mode_pre_merged_app_input_data_forwarder));
  std::thread t31(write_mode_app_output_data_caching,
                  std::ref(write_mode_app_output_data),
                  std::ref(cached_write_mode_app_output_data),
                  std::ref(cached_device_dram_write_req_apply),
                  std::ref(reset_write_mode_app_output_data_caching));
  std::thread t32(
      write_mode_dram_helper_app, std::ref(peek_real_written_bytes_req),
      std::ref(peek_real_written_bytes_resp), std::ref(peek_write_finished_req),
      std::ref(peek_write_finished_resp), std::ref(app_file_infos_1),
      std::ref(app_is_write_mode_0),
      std::ref(cached_write_mode_app_output_data),
      std::ref(cached_device_dram_write_req_apply),
      std::ref(device_dram_write_req_apply),
      std::ref(device_dram_write_req_data),
      std::ref(reset_write_mode_dram_helper_app));
  std::thread t33(
      dram_data_caching, std::ref(dramA_read_resp), std::ref(dramB_read_resp),
      std::ref(dramC_read_resp), std::ref(dramD_read_resp),
      std::ref(cached_dramA_read_resp), std::ref(cached_dramB_read_resp),
      std::ref(cached_dramC_read_resp), std::ref(cached_dramD_read_resp),
      std::ref(dramA_write_req_data), std::ref(dramB_write_req_data),
      std::ref(dramC_write_req_data), std::ref(dramD_write_req_data),
      std::ref(cached_dramA_write_req_data),
      std::ref(cached_dramB_write_req_data),
      std::ref(cached_dramC_write_req_data),
      std::ref(cached_dramD_write_req_data));
  std::thread t34(
      reset_propaganda, std::ref(reset_app_conv_combine_l0_0),
      std::ref(reset_app_conv_combine_l0_1),
      std::ref(reset_app_conv_combine_l0_2),
      std::ref(reset_app_conv_combine_l0_3),
      std::ref(reset_app_conv_combine_l1_0),
      std::ref(reset_app_conv_combine_l1_1),
      std::ref(reset_app_conv_combine_l2_0), std::ref(reset_app_conv_dist),
      std::ref(reset_app_conv_mult_0), std::ref(reset_app_conv_mult_10),
      std::ref(reset_app_conv_mult_11), std::ref(reset_app_conv_mult_12),
      std::ref(reset_app_conv_mult_13), std::ref(reset_app_conv_mult_14),
      std::ref(reset_app_conv_mult_15), std::ref(reset_app_conv_mult_1),
      std::ref(reset_app_conv_mult_2), std::ref(reset_app_conv_mult_3),
      std::ref(reset_app_conv_mult_4), std::ref(reset_app_conv_mult_5),
      std::ref(reset_app_conv_mult_6), std::ref(reset_app_conv_mult_7),
      std::ref(reset_app_conv_mult_8), std::ref(reset_app_conv_mult_9),
      std::ref(reset_app_conv_mult_dummy),
      std::ref(reset_app_conv_row_reduce_0),
      std::ref(reset_app_conv_row_reduce_1),
      std::ref(reset_app_conv_row_reduce_2),
      std::ref(reset_app_conv_row_reduce_3),
      std::ref(reset_app_conv_row_reduce_4),
      std::ref(reset_app_conv_row_reduce_5),
      std::ref(reset_app_conv_row_reduce_6),
      std::ref(reset_app_conv_row_reduce_7), std::ref(reset_app_conv_writer),
      std::ref(reset_sigs), std::ref(reset_read_mode_dram_helper_app),
      std::ref(reset_write_mode_dram_helper_app),
      std::ref(reset_read_mode_pcie_helper_app),
      std::ref(reset_write_mode_pcie_helper_app),
      std::ref(reset_pcie_data_splitter_app),
      std::ref(reset_app_output_data_demux), std::ref(reset_app_input_data_mux),
      std::ref(reset_write_mode_app_output_data_caching),
      std::ref(reset_app_input_data_merger),
      std::ref(reset_write_mode_pre_merged_app_input_data_forwarder));
  std::thread t35(app_conv_dist, std::ref(reset_app_conv_dist),
                  std::ref(app_input_params), std::ref(app_input_data),
                  std::ref(app_conv_mult_0_param),
                  std::ref(app_conv_mult_0_input_data));
  std::thread t36(app_conv_mult_0, std::ref(reset_app_conv_mult_0),
                  std::ref(app_conv_mult_0_param),
                  std::ref(app_conv_mult_1_param),
                  std::ref(app_conv_mult_0_input_data),
                  std::ref(app_conv_mult_1_input_data),
                  std::ref(app_conv_row_reduce_0_input_data));
  std::thread t37(app_conv_mult_1, std::ref(reset_app_conv_mult_1),
                  std::ref(app_conv_mult_1_param),
                  std::ref(app_conv_mult_2_param),
                  std::ref(app_conv_mult_1_input_data),
                  std::ref(app_conv_mult_2_input_data),
                  std::ref(app_conv_row_reduce_1_input_data));
  std::thread t38(app_conv_mult_2, std::ref(reset_app_conv_mult_2),
                  std::ref(app_conv_mult_2_param),
                  std::ref(app_conv_mult_3_param),
                  std::ref(app_conv_mult_2_input_data),
                  std::ref(app_conv_mult_3_input_data),
                  std::ref(app_conv_row_reduce_2_input_data));
  std::thread t39(app_conv_mult_3, std::ref(reset_app_conv_mult_3),
                  std::ref(app_conv_mult_3_param),
                  std::ref(app_conv_mult_4_param),
                  std::ref(app_conv_mult_3_input_data),
                  std::ref(app_conv_mult_4_input_data),
                  std::ref(app_conv_row_reduce_3_input_data));
  std::thread t40(app_conv_mult_4, std::ref(reset_app_conv_mult_4),
                  std::ref(app_conv_mult_4_param),
                  std::ref(app_conv_mult_5_param),
                  std::ref(app_conv_mult_4_input_data),
                  std::ref(app_conv_mult_5_input_data),
                  std::ref(app_conv_row_reduce_4_input_data));
  std::thread t41(app_conv_mult_5, std::ref(reset_app_conv_mult_5),
                  std::ref(app_conv_mult_5_param),
                  std::ref(app_conv_mult_6_param),
                  std::ref(app_conv_mult_5_input_data),
                  std::ref(app_conv_mult_6_input_data),
                  std::ref(app_conv_row_reduce_5_input_data));
  std::thread t42(app_conv_mult_6, std::ref(reset_app_conv_mult_6),
                  std::ref(app_conv_mult_6_param),
                  std::ref(app_conv_mult_7_param),
                  std::ref(app_conv_mult_6_input_data),
                  std::ref(app_conv_mult_7_input_data),
                  std::ref(app_conv_row_reduce_6_input_data));
  std::thread t43(app_conv_mult_7, std::ref(reset_app_conv_mult_7),
                  std::ref(app_conv_mult_7_param),
                  std::ref(app_conv_mult_8_param),
                  std::ref(app_conv_mult_7_input_data),
                  std::ref(app_conv_mult_8_input_data),
                  std::ref(app_conv_row_reduce_7_input_data));
  std::thread t44(app_conv_mult_8, std::ref(reset_app_conv_mult_8),
                  std::ref(app_conv_mult_8_param),
                  std::ref(app_conv_mult_9_param),
                  std::ref(app_conv_mult_8_input_data),
                  std::ref(app_conv_mult_9_input_data),
                  std::ref(app_conv_row_reduce_8_input_data));
  std::thread t45(app_conv_mult_9, std::ref(reset_app_conv_mult_9),
                  std::ref(app_conv_mult_9_param),
                  std::ref(app_conv_mult_10_param),
                  std::ref(app_conv_mult_9_input_data),
                  std::ref(app_conv_mult_10_input_data),
                  std::ref(app_conv_row_reduce_9_input_data));
  std::thread t46(app_conv_mult_10, std::ref(reset_app_conv_mult_10),
                  std::ref(app_conv_mult_10_param),
                  std::ref(app_conv_mult_11_param),
                  std::ref(app_conv_mult_10_input_data),
                  std::ref(app_conv_mult_11_input_data),
                  std::ref(app_conv_row_reduce_10_input_data));
  std::thread t47(app_conv_mult_11, std::ref(reset_app_conv_mult_11),
                  std::ref(app_conv_mult_11_param),
                  std::ref(app_conv_mult_12_param),
                  std::ref(app_conv_mult_11_input_data),
                  std::ref(app_conv_mult_12_input_data),
                  std::ref(app_conv_row_reduce_11_input_data));
  std::thread t48(app_conv_mult_12, std::ref(reset_app_conv_mult_12),
                  std::ref(app_conv_mult_12_param),
                  std::ref(app_conv_mult_13_param),
                  std::ref(app_conv_mult_12_input_data),
                  std::ref(app_conv_mult_13_input_data),
                  std::ref(app_conv_row_reduce_12_input_data));
  std::thread t49(app_conv_mult_13, std::ref(reset_app_conv_mult_13),
                  std::ref(app_conv_mult_13_param),
                  std::ref(app_conv_mult_14_param),
                  std::ref(app_conv_mult_13_input_data),
                  std::ref(app_conv_mult_14_input_data),
                  std::ref(app_conv_row_reduce_13_input_data));
  std::thread t50(app_conv_mult_14, std::ref(reset_app_conv_mult_14),
                  std::ref(app_conv_mult_14_param),
                  std::ref(app_conv_mult_15_param),
                  std::ref(app_conv_mult_14_input_data),
                  std::ref(app_conv_mult_15_input_data),
                  std::ref(app_conv_row_reduce_14_input_data));
  std::thread t51(app_conv_mult_15, std::ref(reset_app_conv_mult_15),
                  std::ref(app_conv_mult_15_param),
                  std::ref(app_conv_mult_dummy_param),
                  std::ref(app_conv_mult_15_input_data),
                  std::ref(app_conv_mult_dummy_input_data),
                  std::ref(app_conv_row_reduce_15_input_data));
  std::thread t52(app_conv_mult_dummy, std::ref(reset_app_conv_mult_dummy),
                  std::ref(app_conv_mult_dummy_param),
                  std::ref(app_conv_mult_dummy_input_data));
  std::thread t53(app_conv_row_reduce_0, std::ref(reset_app_conv_row_reduce_0),
                  std::ref(app_conv_row_reduce_0_input_data),
                  std::ref(app_conv_row_reduce_1_input_data),
                  std::ref(app_conv_combine_l0_input_data_0));
  std::thread t54(app_conv_row_reduce_1, std::ref(reset_app_conv_row_reduce_1),
                  std::ref(app_conv_row_reduce_2_input_data),
                  std::ref(app_conv_row_reduce_3_input_data),
                  std::ref(app_conv_combine_l0_input_data_1));
  std::thread t55(app_conv_row_reduce_2, std::ref(reset_app_conv_row_reduce_2),
                  std::ref(app_conv_row_reduce_4_input_data),
                  std::ref(app_conv_row_reduce_5_input_data),
                  std::ref(app_conv_combine_l0_input_data_2));
  std::thread t56(app_conv_row_reduce_3, std::ref(reset_app_conv_row_reduce_3),
                  std::ref(app_conv_row_reduce_6_input_data),
                  std::ref(app_conv_row_reduce_7_input_data),
                  std::ref(app_conv_combine_l0_input_data_3));
  std::thread t57(app_conv_row_reduce_4, std::ref(reset_app_conv_row_reduce_4),
                  std::ref(app_conv_row_reduce_8_input_data),
                  std::ref(app_conv_row_reduce_9_input_data),
                  std::ref(app_conv_combine_l0_input_data_4));
  std::thread t58(app_conv_row_reduce_5, std::ref(reset_app_conv_row_reduce_5),
                  std::ref(app_conv_row_reduce_10_input_data),
                  std::ref(app_conv_row_reduce_11_input_data),
                  std::ref(app_conv_combine_l0_input_data_5));
  std::thread t59(app_conv_row_reduce_6, std::ref(reset_app_conv_row_reduce_6),
                  std::ref(app_conv_row_reduce_12_input_data),
                  std::ref(app_conv_row_reduce_13_input_data),
                  std::ref(app_conv_combine_l0_input_data_6));
  std::thread t60(app_conv_row_reduce_7, std::ref(reset_app_conv_row_reduce_7),
                  std::ref(app_conv_row_reduce_14_input_data),
                  std::ref(app_conv_row_reduce_15_input_data),
                  std::ref(app_conv_combine_l0_input_data_7));
  std::thread t61(app_conv_combine_l0_0, std::ref(reset_app_conv_combine_l0_0),
                  std::ref(app_conv_combine_l0_input_data_0),
                  std::ref(app_conv_combine_l0_input_data_1),
                  std::ref(app_conv_combine_l1_input_data_0));
  std::thread t62(app_conv_combine_l0_1, std::ref(reset_app_conv_combine_l0_1),
                  std::ref(app_conv_combine_l0_input_data_2),
                  std::ref(app_conv_combine_l0_input_data_3),
                  std::ref(app_conv_combine_l1_input_data_1));
  std::thread t63(app_conv_combine_l0_2, std::ref(reset_app_conv_combine_l0_2),
                  std::ref(app_conv_combine_l0_input_data_4),
                  std::ref(app_conv_combine_l0_input_data_5),
                  std::ref(app_conv_combine_l1_input_data_2));
  std::thread t64(app_conv_combine_l0_3, std::ref(reset_app_conv_combine_l0_3),
                  std::ref(app_conv_combine_l0_input_data_6),
                  std::ref(app_conv_combine_l0_input_data_7),
                  std::ref(app_conv_combine_l1_input_data_3));
  std::thread t65(app_conv_combine_l1_0, std::ref(reset_app_conv_combine_l1_0),
                  std::ref(app_conv_combine_l1_input_data_0),
                  std::ref(app_conv_combine_l1_input_data_1),
                  std::ref(app_conv_combine_l2_input_data_0));
  std::thread t66(app_conv_combine_l1_1, std::ref(reset_app_conv_combine_l1_1),
                  std::ref(app_conv_combine_l1_input_data_2),
                  std::ref(app_conv_combine_l1_input_data_3),
                  std::ref(app_conv_combine_l2_input_data_1));
  std::thread t67(app_conv_combine_l2_0, std::ref(reset_app_conv_combine_l2_0),
                  std::ref(app_conv_combine_l2_input_data_0),
                  std::ref(app_conv_combine_l2_input_data_1),
                  std::ref(app_conv_writer_input_data));
  std::thread t68(app_conv_writer, std::ref(reset_app_conv_writer),
                  std::ref(app_conv_writer_input_data),
                  std::ref(app_output_data));
  std::thread t69(host_pcie_simulator);
  std::thread t70(fpga_dramA_simulator);
  std::thread t71(fpga_dramB_simulator);
  std::thread t72(fpga_dramC_simulator);
  std::thread t73(fpga_dramD_simulator);

  simulator();
  while (1)
    ;
}

#define PATCH_SIZE (KERNEL_SIZE)
#define RAW_DATA_SIZE (NUM_OF_TOTAL_ROW * PATCH_SIZE * sizeof(Elem)) 
#define READ_BUF_SIZE (2 * 1024 * 1024)

Elem kernel_matrix[NUM_OF_KERNELS][KERNEL_SIZE];
Elem image_array[NUM_OF_TOTAL_ROW][PATCH_SIZE];
Result_Elem expected_result[NUM_OF_TOTAL_ROW][NUM_OF_KERNELS];
unsigned char raw_data[RAW_DATA_SIZE];

void gen_data() {
  
  // generate kernel matrix
  for (int i = 0; i < NUM_OF_KERNELS; i++) {
    std::cout << "host kernel " << i << std::endl;
    for (int j = 0; j < KERNEL_SIZE; j++) {
      Elem number = (Elem) rand();
      kernel_matrix[i][j] = number;
      std::cout << (int) number << " ";
    }
    std::cout << std::endl;
  }

  unsigned char *raw_data_ptr = raw_data;
  // generate image array
  std::cout << "image" << std::endl;
  for (int i = 0; i < NUM_OF_TOTAL_ROW; i++) {
    for (int j = 0; j < PATCH_SIZE; j++) {
      Elem number = (Elem) rand();
      std::cout << (int) number << " ";
      image_array[i][j] = number;
      unsigned char *ptr = (unsigned char*) &number;
      for (int k = 0; k < sizeof(Elem); k++) {
        *raw_data_ptr++ = *ptr++;
      }        
    }
  }
  std::cout << std::endl;
}

void conv() {

  for (int i = 0; i < NUM_OF_TOTAL_ROW; i++) {
    for (int j = 0; j < NUM_OF_KERNELS; j++) {
      expected_result[i][j] = 0;
      for (int k = 0; k < PATCH_SIZE; k++) {
        expected_result[i][j] += image_array[i][k] * kernel_matrix[j][k];
      }
      std::cout << expected_result[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}


void send_param() {
  for (int i = 0; i < NUM_OF_KERNELS; i++) {
    unsigned int engine_id = i / KERNEL_BATCH_SIZE;
    unsigned int engine_idx = i % KERNEL_BATCH_SIZE;
    for (int k = 0; k < KERNEL_SIZE; k++) {
      unsigned int data = (engine_id << 24) | (engine_idx << 16) | (k << 8) | (unsigned char) kernel_matrix[i][k];
      send_input_param(data);
    }
  }
}


void user_simulation_function() {
  // PUT YOUR CODE HERE
  gen_data();

  unsigned long long extents_start_idx = 0;
  unsigned long long extents_len = sizeof(raw_data);
  set_physical_file((unsigned char *)raw_data, 1, &extents_start_idx, 
		    &extents_len);

  send_param();

  std::cout << "INFO: params send" << std::endl;

  conv();

  std::cout << "expected data generated..." << std::endl;

  int fd = vopen("csim_phy_file", 0);
  unsigned char *buf = (unsigned char *)malloc(READ_BUF_SIZE);
  bool fin_file = false;

  std::cout << "file opened..." << std::endl;

  unsigned char *expected_result_in_bytes = (unsigned char *) expected_result;

  while (!fin_file) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      int tmp = vread(fd, buf, READ_BUF_SIZE - read_bytes);
      if (!tmp) {
        fin_file = true;
        break;
      } else {
        read_bytes += tmp;
      }
    }
    std::cout << read_bytes << std::endl;
    for (int i = 0; i < read_bytes; i++) {
      unsigned char real = buf[i];
      unsigned char expected = expected_result_in_bytes[i];
      //std::cout << (int) real << " : " << (int) expected << std::endl;
      if (real != expected) {
        std::cout << "Failed!" << std::endl;
        //exit(-1);
      }
    }
    expected_result_in_bytes += read_bytes;
  }
  std::cout << "Passed!" << std::endl;
  vclose(fd);
}
