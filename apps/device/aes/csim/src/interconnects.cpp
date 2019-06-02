#include <insider_itc.h>

#include "structure.h"

#include "app_aes_end.cpp"
#include "app_aes_process_0.cpp"
#include "app_aes_process_1.cpp"
#include "app_aes_process_2.cpp"
#include "app_aes_process_3.cpp"
#include "app_aes_process_4.cpp"
#include "app_aes_process_5.cpp"
#include "app_aes_process_6.cpp"
#include "app_aes_process_7.cpp"
#include "app_aes_process_8.cpp"
#include "app_aes_start.cpp"

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

  ST_Queue<bool> reset_app_aes_end(4);
  ST_Queue<bool> reset_app_aes_process_0(4);
  ST_Queue<bool> reset_app_aes_process_1(4);
  ST_Queue<bool> reset_app_aes_process_2(4);
  ST_Queue<bool> reset_app_aes_process_3(4);
  ST_Queue<bool> reset_app_aes_process_4(4);
  ST_Queue<bool> reset_app_aes_process_5(4);
  ST_Queue<bool> reset_app_aes_process_6(4);
  ST_Queue<bool> reset_app_aes_process_7(4);
  ST_Queue<bool> reset_app_aes_process_8(4);
  ST_Queue<bool> reset_app_aes_start(4);

  ST_Queue<APP_Data> app_aes_proc_0_input_data(8);
  ST_Queue<APP_Data> app_aes_proc_1_input_data(8);
  ST_Queue<APP_Data> app_aes_proc_2_input_data(8);
  ST_Queue<APP_Data> app_aes_proc_3_input_data(8);
  ST_Queue<APP_Data> app_aes_proc_4_input_data(8);
  ST_Queue<APP_Data> app_aes_proc_5_input_data(8);
  ST_Queue<APP_Data> app_aes_proc_6_input_data(8);
  ST_Queue<APP_Data> app_aes_proc_7_input_data(8);
  ST_Queue<APP_Data> app_aes_proc_8_input_data(8);
  ST_Queue<APP_Data> app_aes_end_input_data(8);

  ST_Queue<unsigned int> app_aes_proc_0_params(8);
  ST_Queue<unsigned int> app_aes_proc_1_params(8);
  ST_Queue<unsigned int> app_aes_proc_2_params(8);
  ST_Queue<unsigned int> app_aes_proc_3_params(8);
  ST_Queue<unsigned int> app_aes_proc_4_params(8);
  ST_Queue<unsigned int> app_aes_proc_5_params(8);
  ST_Queue<unsigned int> app_aes_proc_6_params(8);
  ST_Queue<unsigned int> app_aes_proc_7_params(8);
  ST_Queue<unsigned int> app_aes_proc_8_params(8);
  ST_Queue<unsigned int> app_aes_end_params(8);

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
      reset_propaganda, std::ref(reset_app_aes_end),
      std::ref(reset_app_aes_process_0), std::ref(reset_app_aes_process_1),
      std::ref(reset_app_aes_process_2), std::ref(reset_app_aes_process_3),
      std::ref(reset_app_aes_process_4), std::ref(reset_app_aes_process_5),
      std::ref(reset_app_aes_process_6), std::ref(reset_app_aes_process_7),
      std::ref(reset_app_aes_process_8), std::ref(reset_app_aes_start),
      std::ref(reset_sigs), std::ref(reset_read_mode_dram_helper_app),
      std::ref(reset_write_mode_dram_helper_app),
      std::ref(reset_read_mode_pcie_helper_app),
      std::ref(reset_write_mode_pcie_helper_app),
      std::ref(reset_pcie_data_splitter_app),
      std::ref(reset_app_output_data_demux), std::ref(reset_app_input_data_mux),
      std::ref(reset_write_mode_app_output_data_caching),
      std::ref(reset_app_input_data_merger),
      std::ref(reset_write_mode_pre_merged_app_input_data_forwarder));
  std::thread t35(app_aes_start, std::ref(reset_app_aes_start),
                  std::ref(app_input_params), std::ref(app_aes_proc_0_params),
                  std::ref(app_input_data),
                  std::ref(app_aes_proc_0_input_data));
  std::thread t36(
      app_aes_process_0, std::ref(reset_app_aes_process_0),
      std::ref(app_aes_proc_0_params), std::ref(app_aes_proc_1_params),
      std::ref(app_aes_proc_0_input_data), std::ref(app_aes_proc_1_input_data));
  std::thread t37(
      app_aes_process_1, std::ref(reset_app_aes_process_1),
      std::ref(app_aes_proc_1_params), std::ref(app_aes_proc_2_params),
      std::ref(app_aes_proc_1_input_data), std::ref(app_aes_proc_2_input_data));
  std::thread t38(
      app_aes_process_2, std::ref(reset_app_aes_process_2),
      std::ref(app_aes_proc_2_params), std::ref(app_aes_proc_3_params),
      std::ref(app_aes_proc_2_input_data), std::ref(app_aes_proc_3_input_data));
  std::thread t39(
      app_aes_process_3, std::ref(reset_app_aes_process_3),
      std::ref(app_aes_proc_3_params), std::ref(app_aes_proc_4_params),
      std::ref(app_aes_proc_3_input_data), std::ref(app_aes_proc_4_input_data));
  std::thread t40(
      app_aes_process_4, std::ref(reset_app_aes_process_4),
      std::ref(app_aes_proc_4_params), std::ref(app_aes_proc_5_params),
      std::ref(app_aes_proc_4_input_data), std::ref(app_aes_proc_5_input_data));
  std::thread t41(
      app_aes_process_5, std::ref(reset_app_aes_process_5),
      std::ref(app_aes_proc_5_params), std::ref(app_aes_proc_6_params),
      std::ref(app_aes_proc_5_input_data), std::ref(app_aes_proc_6_input_data));
  std::thread t42(
      app_aes_process_6, std::ref(reset_app_aes_process_6),
      std::ref(app_aes_proc_6_params), std::ref(app_aes_proc_7_params),
      std::ref(app_aes_proc_6_input_data), std::ref(app_aes_proc_7_input_data));
  std::thread t43(
      app_aes_process_7, std::ref(reset_app_aes_process_7),
      std::ref(app_aes_proc_7_params), std::ref(app_aes_proc_8_params),
      std::ref(app_aes_proc_7_input_data), std::ref(app_aes_proc_8_input_data));
  std::thread t44(app_aes_process_8, std::ref(reset_app_aes_process_8),
                  std::ref(app_aes_proc_8_params), std::ref(app_aes_end_params),
                  std::ref(app_aes_proc_8_input_data),
                  std::ref(app_aes_end_input_data));
  std::thread t45(app_aes_end, std::ref(reset_app_aes_end),
                  std::ref(app_aes_end_params),
                  std::ref(app_aes_end_input_data), std::ref(app_output_data));
  std::thread t46(host_pcie_simulator);
  std::thread t47(fpga_dramA_simulator);
  std::thread t48(fpga_dramB_simulator);
  std::thread t49(fpga_dramC_simulator);
  std::thread t50(fpga_dramD_simulator);

  simulator();
  while (1)
    ;
}


#define DATA_SIZE 128 // multipy of 64
#define READ_BUF_SIZE (2 * 1024 * 1024)

unsigned char S_matrix[16][16] = {
  0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76, 
  0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
  0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
  0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
  0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
  0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
  0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
  0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
  0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
  0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
  0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
  0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
  0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
  0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
  0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
  0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};
unsigned char mix_matrix[4][4] ={
  0x02, 0x03, 0x01, 0x01,
  0x01, 0x02, 0x03, 0x01,
  0x01, 0x01, 0x02, 0x03,
  0x03, 0x01, 0x01, 0x02
};

unsigned char key_matrix[4][4] ={0};
unsigned char data_matrix[DATA_SIZE]; // 4 * (4 * 4) matrix
unsigned char raw_data[DATA_SIZE];
unsigned char expected_result[DATA_SIZE];

void gen_data() {
  unsigned char * raw_data_ptr = raw_data;
  for (int i = 0; i < DATA_SIZE; i++) {
    Elem number = (Elem) rand();
    data_matrix[i] = number;
    *raw_data_ptr++ = (unsigned char) number;
  }
}

void send_params() {
  // send S_matrix
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j+=4) {
        send_input_param((S_matrix[i][j] << 24) | (S_matrix[i][j + 1] << 16) 
            | (S_matrix[i][j + 2] << 8) | (S_matrix[i][j + 3]));
    }
  }
  
  // send mix matrix
  for (int i = 0; i < 4; i++) {
    send_input_param((mix_matrix[i][0] << 0) | (mix_matrix[i][1] << 8) 
        | (mix_matrix[i][2] << 16) | (mix_matrix[i][3]) << 24);
  }
  
  // send key matrix
  for (int i = 0; i < 4; i++) {
    int j = 0;
    send_input_param((key_matrix[i][j + 0] << 0) | (key_matrix[i][j + 1] << 8) 
        | (key_matrix[i][j + 2] << 16) | (key_matrix[i][j + 3]) << 24);
  }
}

void host_shiftrows_helper(unsigned char * arr, int times) {
  for (int i = 0; i < times; i++) {
    unsigned char head = arr[0];
    for (int j = 1; j < 4; j++) {
      arr[j - 1] = arr[j];
    }
    arr[3] = head;
  }
}

unsigned char host_GFMul2(unsigned char a){
  unsigned char msb = (a >> 7)& 1;
  a <<= 1;
  if (!msb) a ^= 0x1B;
  return a;    
}

unsigned char host_GFMul3(unsigned char a){
    return host_GFMul2(a) ^ a;
}

unsigned char host_GFMul(unsigned char a, unsigned char b){
  if (a == 1) return b;
  else if(a == 2) return host_GFMul2(b);
  else if (a == 3) return host_GFMul3(b);
}

inline void host_subbytes(unsigned char data_chuck[][4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
        unsigned int row_id = ((data_chuck[i][j]) >> 4) & 0xf;
        unsigned int col_id = (data_chuck[i][j]) & 0xf;
        data_chuck[i][j] = S_matrix[row_id][col_id];
    }
  }
}

inline void host_shiftrows(unsigned char data_chuck[][4]) {
  for (int i = 0; i < 4; i++) { 
   host_shiftrows_helper(data_chuck[i], i);
  }
}

inline void host_mixcols(unsigned char data_chuck[][4]) {
  unsigned char new_data_chuck[4][4]= {0};
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      new_data_chuck[i][j] = 0;
      for (int k = 0; k < 4; k++) {
        new_data_chuck[i][j] ^= host_GFMul(mix_matrix[i][k], data_chuck[k][j]); 
      }
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      data_chuck[i][j] = new_data_chuck[i][j];
    }
  }
}

inline void host_addkeys(unsigned char data_chuck[][4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4;j ++) {
      data_chuck[i][j] = data_chuck[i][j] ^ key_matrix[i][j];
    }
  }
}

void host_process(unsigned char data_chuck[][4], int round_id) {

  if (round_id == 0) {
    host_addkeys(data_chuck);
  } else if (round_id == 10) {
    host_subbytes(data_chuck);
    host_shiftrows(data_chuck);
    host_addkeys(data_chuck);
  } else if (round_id > 0 && round_id < 10) { 
    host_subbytes(data_chuck);
    host_shiftrows(data_chuck);
    host_mixcols(data_chuck);
    host_addkeys(data_chuck);
  }

}

void aes() {
  for (int base = 0; base < DATA_SIZE; base += 16) {

    unsigned char data_chuck[4][4];
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        data_chuck[i][j] = *(data_matrix + base + 4 * i + j);
      }
    }

    for (int rd = 0; rd < 11; rd++) {
      host_process(data_chuck, rd);
    }

    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4;j ++) {
        *(expected_result + base + 4 * i + j) = data_chuck[i][j];
      }
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

  send_params();

  aes();

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
      std::cout << (int) real << " : " << (int) expected << std::endl;
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
