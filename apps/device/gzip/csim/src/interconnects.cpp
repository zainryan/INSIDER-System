#include <insider_itc.h>

#include "app_gzip.cpp"

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

  ST_Queue<bool> reset_app_gzip(4);

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
      reset_propaganda, std::ref(reset_app_gzip), std::ref(reset_sigs),
      std::ref(reset_read_mode_dram_helper_app),
      std::ref(reset_write_mode_dram_helper_app),
      std::ref(reset_read_mode_pcie_helper_app),
      std::ref(reset_write_mode_pcie_helper_app),
      std::ref(reset_pcie_data_splitter_app),
      std::ref(reset_app_output_data_demux), std::ref(reset_app_input_data_mux),
      std::ref(reset_write_mode_app_output_data_caching),
      std::ref(reset_app_input_data_merger),
      std::ref(reset_write_mode_pre_merged_app_input_data_forwarder));
  std::thread t35(app_gzip, std::ref(reset_app_gzip), std::ref(app_input_data),
                  std::ref(app_output_data), std::ref(app_input_params));
  std::thread t36(host_pcie_simulator);
  std::thread t37(fpga_dramA_simulator);
  std::thread t38(fpga_dramB_simulator);
  std::thread t39(fpga_dramC_simulator);
  std::thread t40(fpga_dramD_simulator);

  simulator();
  while (1)
    ;
}

const unsigned long long INPUT_SIZE = 1024;
const unsigned long long READ_BUF_SIZE = 64;
char input[INPUT_SIZE];
char output[INPUT_SIZE];

void user_simulation_function() {
  send_input_param(INPUT_SIZE >> 32);
  send_input_param(INPUT_SIZE & 0xFFFFFFFF);
  puts("Finish sending input param...");
  for (int i = 0; i < INPUT_SIZE; i++) {
    input[i] = i % 2;
  }

  unsigned long long extents_start_idx = 0;
  unsigned long long extents_len = sizeof(input);
  set_physical_file((unsigned char *)input, 1, &extents_start_idx, 
		    &extents_len);

  puts("Finish setting physical file...");
  int fd = vopen("csim_phy_file", 0);
  puts("Finish vopen...");
  unsigned char *buf = (unsigned char *)malloc(INPUT_SIZE);
  int fin_file = 0;

  int total_read_bytes = 0;
  while (!fin_file) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      puts("Before vread...");
      int tmp = vread(fd, buf + read_bytes, READ_BUF_SIZE - read_bytes);
      puts("After vread...");
      if (!tmp) {
        fin_file = 1;
        break;
      } else {
        read_bytes += tmp;
      }
    }
    printf("Read bytes = %d\n", read_bytes);
    total_read_bytes += read_bytes;
  }
  puts("Finish reading virtual file...");
  printf("Original input size = %d\n", INPUT_SIZE);
  printf("Total read bytes = %d\n", total_read_bytes);
  vclose(fd);
  puts("PASSED!");
}

