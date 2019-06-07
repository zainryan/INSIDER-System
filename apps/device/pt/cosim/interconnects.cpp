#include <insider_itc.h>

#include "app_pt.cpp"

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

void interconnects() {
  ST_Queue<Dram_Read_Req> cosim_dramA_read_req(128);
  ST_Queue<Dram_Read_Resp> cosim_dramA_read_resp(128);
  ST_Queue<Dram_Write_Req_Data> cosim_dramA_write_req_data(128);
  ST_Queue<Dram_Write_Req_Apply> cosim_dramA_write_req_apply(128);

  ST_Queue<Dram_Read_Req> cosim_dramB_read_req(128);
  ST_Queue<Dram_Read_Resp> cosim_dramB_read_resp(128);
  ST_Queue<Dram_Write_Req_Data> cosim_dramB_write_req_data(128);
  ST_Queue<Dram_Write_Req_Apply> cosim_dramB_write_req_apply(128);

  ST_Queue<Dram_Read_Req> cosim_dramC_read_req(128);
  ST_Queue<Dram_Read_Resp> cosim_dramC_read_resp(128);
  ST_Queue<Dram_Write_Req_Data> cosim_dramC_write_req_data(128);
  ST_Queue<Dram_Write_Req_Apply> cosim_dramC_write_req_apply(128);

  ST_Queue<Dram_Read_Req> cosim_dramD_read_req(128);
  ST_Queue<Dram_Read_Resp> cosim_dramD_read_resp(128);
  ST_Queue<Dram_Write_Req_Data> cosim_dramD_write_req_data(128);
  ST_Queue<Dram_Write_Req_Apply> cosim_dramD_write_req_apply(128);

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
  ST_Queue<Dram_Write_Req_Data> before_delay_dram_write_req_data(4);
  ST_Queue<Dram_Write_Req_Apply> after_mux_dram_write_req_apply(64);
  ST_Queue<Dram_Write_Req_Apply_With_Time>
      before_delay_dram_write_req_apply_with_time(64);

  ST_Queue<Dram_Read_Req> device_dram_read_req(64);
  ST_Queue<Dram_Read_Resp> device_dram_read_resp(4);
  ST_Queue<bool> release_device_dram_resp_buf_flits(128);

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

  ST_Queue<unsigned char> pcie_read_mux_context(64);
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

  poke_handler(poke, reqs_incoming, kbuf_addrs, dma_read_throttle_params,
               dma_write_throttle_params, drive_read_delay_cycle_cnts,
               drive_write_delay_cycle_cnts, drive_read_throttle_params,
               drive_write_throttle_params, app_file_infos_0, app_file_infos_1,
               read_mode_app_buf_addrs, write_mode_app_buf_addrs, app_free_buf,
               app_write_total_len, app_input_params, app_commit_write_buf,
               app_is_write_mode_0, app_is_write_mode_1, app_is_write_mode_2,
               reset_sigs);

  peek_handler(peek_req, peek_resp, peek_real_written_bytes_req,
               peek_real_written_bytes_resp, peek_write_finished_req,
               peek_write_finished_resp, peek_virt_written_bytes_req,
               peek_virt_written_bytes_resp);

  pcie_read_resp_passer(pcie_read_resp, before_throttle_pcie_read_resp);
  pcie_read_throttle_unit(dma_read_throttle_params,
                          before_throttle_pcie_read_resp,
                          after_throttle_pcie_read_resp);
  pipe0_data_handler(kbuf_addrs, reqs_incoming, host_dram_read_req,
                     host_pcie_read_req, rd_reqs_pipe0_write,
                     wr_reqs_pipe0_write, rd_kbuf_addr_pipe0_write,
                     wr_kbuf_addr_pipe0_write);
  pipe1_data_handler(rd_reqs_pipe1_read, wr_reqs_pipe1_read,
                     host_dram_read_resp, host_after_throttle_pcie_read_resp,
                     rd_reqs_pipe1_write, wr_reqs_pipe1_write,
                     rd_data_valid_pipe1_write, wr_data_valid_pipe1_write,
                     data_pipe2, host_dram_write_req_data);
  pipe2_data_handler(
      rd_reqs_pipe2_read, wr_reqs_pipe2_read, rd_data_valid_pipe2_read,
      wr_data_valid_pipe2_read, rd_kbuf_addr_pipe2_read,
      wr_kbuf_addr_pipe2_read, data_pipe2, host_data_pcie_write_req_apply,
      host_data_pcie_write_req_data, host_rdcmd_fin_pcie_write_req_apply,
      host_rdcmd_fin_pcie_write_req_data, host_dram_write_req_apply,
      wrcmd_kbuf_addrs);
  pipeline_data_passer(rd_reqs_pipe0_write, wr_reqs_pipe0_write,
                       rd_reqs_pipe1_read, wr_reqs_pipe1_read,
                       rd_reqs_pipe1_write, wr_reqs_pipe1_write,
                       rd_reqs_pipe2_read, wr_reqs_pipe2_read,
                       rd_data_valid_pipe1_write, wr_data_valid_pipe1_write,
                       rd_data_valid_pipe2_read, wr_data_valid_pipe2_read,
                       rd_kbuf_addr_pipe0_write, wr_kbuf_addr_pipe0_write,
                       rd_kbuf_addr_pipe2_read, wr_kbuf_addr_pipe2_read);

  pcie_write_multiplexer(
      host_data_pcie_write_req_data, host_data_pcie_write_req_apply,
      before_throttle_pcie_write_req_data, before_throttle_pcie_write_req_apply,
      host_rdcmd_fin_pcie_write_req_data, host_rdcmd_fin_pcie_write_req_apply,
      host_wrcmd_fin_pcie_write_req_data, host_wrcmd_fin_pcie_write_req_apply,
      read_mode_device_pcie_write_req_data,
      read_mode_device_pcie_write_req_apply,
      write_mode_device_pcie_write_req_data,
      write_mode_device_pcie_write_req_apply, cosim_dramA_write_req_data,
      cosim_dramA_write_req_apply, cosim_dramB_write_req_data,
      cosim_dramB_write_req_apply, cosim_dramC_write_req_data,
      cosim_dramC_write_req_apply, cosim_dramD_write_req_data,
      cosim_dramD_write_req_apply);

  pcie_write_throttle_unit(dma_write_throttle_params,
                           before_throttle_pcie_write_req_data,
                           before_throttle_pcie_write_req_apply,
                           pcie_write_req_data, pcie_write_req_apply);

  dram_read_req_multiplexer(
      after_delay_host_dram_read_req, after_delay_device_dram_read_req,
      after_delay_unified_dram_read_req, release_device_dram_resp_buf_flits,
      dram_read_context);
  dram_read_resp_multiplexer(host_dram_read_resp, device_dram_read_resp,
                             release_device_dram_resp_buf_flits,
                             after_throttle_unified_dram_read_resp,
                             dram_read_context);
  dram_read_throttle_unit(drive_read_throttle_params,
                          before_throttle_unified_dram_read_resp,
                          after_throttle_unified_dram_read_resp);
  dram_write_req_time_marker(after_mux_dram_write_req_data,
                             after_mux_dram_write_req_apply,
                             before_delay_dram_write_req_data,
                             before_delay_dram_write_req_apply_with_time);
  dram_write_delay_unit(drive_write_delay_cycle_cnts,
                        before_delay_dram_write_req_data,
                        before_delay_dram_write_req_apply_with_time,
                        before_throttle_unified_dram_write_req_data,
                        before_throttle_unified_dram_write_req_apply);
  dram_write_throttle_unit(drive_write_throttle_params,
                           before_throttle_unified_dram_write_req_data,
                           before_throttle_unified_dram_write_req_apply,
                           after_throttle_unified_dram_write_req_data,
                           after_throttle_unified_dram_write_req_apply);
  dram_read_req_time_marker(
      host_dram_read_req, before_delay_host_dram_read_req_with_time,
      device_dram_read_req, before_delay_device_dram_read_req_with_time);
  dram_read_delay_unit(drive_read_delay_cycle_cnts,
                       before_delay_host_dram_read_req_with_time,
                       after_delay_host_dram_read_req,
                       before_delay_device_dram_read_req_with_time,
                       after_delay_device_dram_read_req);
  pipe0_dram_dispatcher(after_delay_unified_dram_read_req,
                        after_throttle_unified_dram_write_req_apply,
                        cosim_dramA_read_req, cosim_dramA_write_req_apply,
                        cosim_dramB_read_req, cosim_dramB_write_req_apply,
                        cosim_dramC_read_req, cosim_dramC_write_req_apply,
                        cosim_dramD_read_req, cosim_dramD_write_req_apply,
                        unified_dram_dispatcher_write_context,
                        unified_dram_dispatcher_read_context);
  pipe1_dram_dispatcher(
      before_throttle_unified_dram_read_resp,
      after_throttle_unified_dram_write_req_data, cached_dramA_read_resp,
      cached_dramA_write_req_data, cached_dramB_read_resp,
      cached_dramB_write_req_data, cached_dramC_read_resp,
      cached_dramC_write_req_data, cached_dramD_read_resp,
      cached_dramD_write_req_data, unified_dram_dispatcher_write_context,
      unified_dram_dispatcher_read_context, host_wrcmd_fin_pcie_write_req_data,
      host_wrcmd_fin_pcie_write_req_apply, wrcmd_kbuf_addrs);

  read_mode_dram_helper_app(app_file_infos_0, app_is_write_mode_1,
                            device_dram_read_req, device_dram_read_resp,
                            read_mode_app_input_data,
                            reset_read_mode_dram_helper_app);
  read_mode_pcie_helper_app(
      read_mode_app_buf_addrs, read_mode_device_pcie_write_req_apply,
      read_mode_device_pcie_write_req_data, app_output_data_splitted,
      app_output_data_meta, app_free_buf, reset_read_mode_pcie_helper_app);
  pcie_data_splitter_app(read_mode_app_output_data, app_output_data_splitted,
                         app_output_data_meta, reset_pcie_data_splitter_app);

  pcie_read_req_multiplexer(pcie_read_req, device_pcie_read_req,
                            host_pcie_read_req, cosim_dramA_read_req,
                            cosim_dramB_read_req, cosim_dramC_read_req,
                            cosim_dramD_read_req, pcie_read_mux_context);
  pcie_read_resp_multiplexer(
      after_throttle_pcie_read_resp, device_after_throttle_pcie_read_resp,
      host_after_throttle_pcie_read_resp, cosim_dramA_read_resp,
      cosim_dramB_read_resp, cosim_dramC_read_resp, cosim_dramD_read_resp,
      pcie_read_mux_context);

  app_input_data_merger(pre_merged_write_mode_app_input_data,
                        write_mode_app_input_data, reset_app_input_data_merger);

  dram_write_mux(after_mux_dram_write_req_apply, after_mux_dram_write_req_data,
                 host_dram_write_req_apply, host_dram_write_req_data,
                 device_dram_write_req_apply, device_dram_write_req_data);

  app_input_data_mux(read_mode_app_input_data, write_mode_app_input_data,
                     app_input_data, peek_virt_written_bytes_req,
                     peek_virt_written_bytes_resp, reset_app_input_data_mux);
  app_output_data_demux(app_is_write_mode_2, app_output_data,
                        read_mode_app_output_data, write_mode_app_output_data,
                        reset_app_output_data_demux);

  write_mode_pcie_helper_app(
      write_mode_app_buf_addrs, app_commit_write_buf, device_pcie_read_req,
      device_after_throttle_pcie_read_resp,
      buffered_device_pcie_read_req_context, buffered_device_pcie_read_resp,
      release_buffered_device_pcie_read_resp, reset_write_mode_pcie_helper_app);
  write_mode_pre_merged_app_input_data_forwarder(
      buffered_device_pcie_read_resp, release_buffered_device_pcie_read_resp,
      buffered_device_pcie_read_req_context,
      pre_merged_write_mode_app_input_data, app_write_total_len,
      write_mode_device_pcie_write_req_apply,
      write_mode_device_pcie_write_req_data,
      reset_write_mode_pre_merged_app_input_data_forwarder);
  write_mode_app_output_data_caching(write_mode_app_output_data,
                                     cached_write_mode_app_output_data,
                                     cached_device_dram_write_req_apply,
                                     reset_write_mode_app_output_data_caching);
  write_mode_dram_helper_app(
      peek_real_written_bytes_req, peek_real_written_bytes_resp,
      peek_write_finished_req, peek_write_finished_resp, app_file_infos_1,
      app_is_write_mode_0, cached_write_mode_app_output_data,
      cached_device_dram_write_req_apply, device_dram_write_req_apply,
      device_dram_write_req_data, reset_write_mode_dram_helper_app);

  dram_data_caching(cosim_dramA_read_resp, cosim_dramB_read_resp,
                    cosim_dramC_read_resp, cosim_dramD_read_resp,
                    cached_dramA_read_resp, cached_dramB_read_resp,
                    cached_dramC_read_resp, cached_dramD_read_resp,
                    cosim_dramA_write_req_data, cosim_dramB_write_req_data,
                    cosim_dramC_write_req_data, cosim_dramD_write_req_data,
                    cached_dramA_write_req_data, cached_dramB_write_req_data,
                    cached_dramC_write_req_data, cached_dramD_write_req_data);

  ST_Queue<bool> reset_app_pt(4);
  reset_propaganda(
      reset_app_pt, reset_sigs, reset_read_mode_dram_helper_app,
      reset_write_mode_dram_helper_app, reset_read_mode_pcie_helper_app,
      reset_write_mode_pcie_helper_app, reset_pcie_data_splitter_app,
      reset_app_output_data_demux, reset_app_input_data_mux,
      reset_write_mode_app_output_data_caching, reset_app_input_data_merger,
      reset_write_mode_pre_merged_app_input_data_forwarder);

  ;
  app_pt(reset_app_pt, app_input_data, app_output_data, app_input_params);
}
