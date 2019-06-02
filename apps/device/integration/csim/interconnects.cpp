#include <insider_itc.h>

#include "constant.h"
#include "structure.h"

#include "app_intg_mat_rdc.cpp"
#include "app_intg_matcher.cpp"
#include "app_intg_rdc_16to8.cpp"
#include "app_intg_rdc_32to16.cpp"
#include "app_intg_rdc_4to2.cpp"
#include "app_intg_rdc_8to4.cpp"
#include "app_intg_verifier.cpp"
#include "app_intg_writer.cpp"

#include "insider_itc.h"

#include "command_handler.cpp"
#include "dram_helper_app.cpp"
#include "dram_read_delay_unit.cpp"
#include "dram_read_req_multiplexer.cpp"
#include "dram_read_req_time_marker.cpp"
#include "dram_read_resp_multiplexer.cpp"
#include "dram_read_throttle_unit.cpp"
#include "dram_write_delay_unit.cpp"
#include "dram_write_req_time_marker.cpp"
#include "dram_write_throttle_unit.cpp"
#include "pcie_data_splitter_app.cpp"
#include "pcie_helper_app.cpp"
#include "pcie_read_resp_passer.cpp"
#include "pcie_read_throttle_unit.cpp"
#include "pcie_write_multiplexer.cpp"
#include "pcie_write_throttle_unit.cpp"
#include "pipe0_data_handler.cpp"
#include "pipe0_dram_dispatcher.cpp"
#include "pipe1_data_handler.cpp"
#include "pipe1_dram_dispatcher.cpp"
#include "pipe2_data_handler.cpp"
#include "pipeline_data_passer.cpp"
#include "reset_propaganda.cpp"

void interconnects() {
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
  ST_Queue<Dram_Write_Req_Data> host_dram_write_req_data(4096);
  ST_Queue<Dram_Write_Req_Data> before_delay_dram_write_req_data(4);
  ST_Queue<Dram_Write_Req_Apply> host_dram_write_req_apply(64);
  ST_Queue<Dram_Write_Req_Apply_With_Time>
      before_delay_dram_write_req_apply_with_time(64);

  ST_Queue<Dram_Read_Req> device_dram_read_req(64);
  ST_Queue<Dram_Read_Resp> device_dram_read_resp(4);

  ST_Queue<Dram_Read_Req> before_delay_unified_dram_read_req(64);
  ST_Queue<Dram_Read_Req_With_Time>
      before_delay_unified_dram_read_req_with_time(64);
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

  ST_Queue<PCIe_Write_Req_Data> device_pcie_write_req_data(64);
  ST_Queue<PCIe_Write_Req_Apply> device_pcie_write_req_apply(64);

  ST_Queue<unsigned int> drive_read_delay_cycle_cnts(4);
  ST_Queue<unsigned int> drive_write_delay_cycle_cnts(4);
  ST_Queue<unsigned int> drive_read_throttle_params(4);
  ST_Queue<unsigned int> drive_write_throttle_params(4);

  ST_Queue<bool> app_free_buf(4);
  ST_Queue<unsigned int> app_file_infos(4);
  ST_Queue<unsigned int> app_buf_addrs(4);
  ST_Queue<unsigned int> app_input_params(4);
  ST_Queue<APP_Data> app_input_data(4);
  ST_Queue<APP_Data> app_output_data(4);
  ST_Queue<APP_Data> app_output_data_splitted(4);
  ST_Queue<APP_Data_Meta> app_output_data_meta(4);

  ST_Queue<bool> reset_sigs(4);
  ST_Queue<bool> reset_dram_helper_app(4);
  ST_Queue<bool> reset_pcie_helper_app(4);
  ST_Queue<bool> reset_pcie_data_splitter_app(4);

  command_handler(poke, reqs_incoming, kbuf_addrs, dma_read_throttle_params,
                  dma_write_throttle_params, drive_read_delay_cycle_cnts,
                  drive_write_delay_cycle_cnts, drive_read_throttle_params,
                  drive_write_throttle_params, app_file_infos, app_buf_addrs,
                  app_free_buf, app_input_params, reset_sigs);

  pcie_read_resp_passer(pcie_read_resp, before_throttle_pcie_read_resp);
  pcie_read_throttle_unit(dma_read_throttle_params,
                          before_throttle_pcie_read_resp,
                          after_throttle_pcie_read_resp);
  pipe0_data_handler(kbuf_addrs, reqs_incoming, host_dram_read_req,
                     pcie_read_req, rd_reqs_pipe0_write, wr_reqs_pipe0_write,
                     rd_kbuf_addr_pipe0_write, wr_kbuf_addr_pipe0_write);
  pipe1_data_handler(rd_reqs_pipe1_read, wr_reqs_pipe1_read,
                     host_dram_read_resp, after_throttle_pcie_read_resp,
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
      device_pcie_write_req_data, device_pcie_write_req_apply);

  pcie_write_throttle_unit(dma_write_throttle_params,
                           before_throttle_pcie_write_req_data,
                           before_throttle_pcie_write_req_apply,
                           pcie_write_req_data, pcie_write_req_apply);

  dram_read_req_multiplexer(host_dram_read_req, device_dram_read_req,
                            before_delay_unified_dram_read_req,
                            dram_read_context);
  dram_read_resp_multiplexer(host_dram_read_resp, device_dram_read_resp,
                             after_throttle_unified_dram_read_resp,
                             dram_read_context);
  dram_read_throttle_unit(drive_read_throttle_params,
                          before_throttle_unified_dram_read_resp,
                          after_throttle_unified_dram_read_resp);
  dram_write_req_time_marker(host_dram_write_req_data,
                             host_dram_write_req_apply,
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
  dram_read_req_time_marker(before_delay_unified_dram_read_req,
                            before_delay_unified_dram_read_req_with_time);
  dram_read_delay_unit(drive_read_delay_cycle_cnts,
                       before_delay_unified_dram_read_req_with_time,
                       after_delay_unified_dram_read_req);
  pipe0_dram_dispatcher(
      after_delay_unified_dram_read_req,
      after_throttle_unified_dram_write_req_apply, dramA_read_req,
      dramA_write_req_apply, dramB_read_req, dramB_write_req_apply,
      dramC_read_req, dramC_write_req_apply, dramD_read_req,
      dramD_write_req_apply, unified_dram_dispatcher_write_context,
      unified_dram_dispatcher_read_context);
  pipe1_dram_dispatcher(
      before_throttle_unified_dram_read_resp,
      after_throttle_unified_dram_write_req_data, dramA_read_resp,
      dramA_write_req_data, dramB_read_resp, dramB_write_req_data,
      dramC_read_resp, dramC_write_req_data, dramD_read_resp,
      dramD_write_req_data, unified_dram_dispatcher_write_context,
      unified_dram_dispatcher_read_context, host_wrcmd_fin_pcie_write_req_data,
      host_wrcmd_fin_pcie_write_req_apply, wrcmd_kbuf_addrs);

  dram_helper_app(app_file_infos, device_dram_read_req, device_dram_read_resp,
                  app_input_data, reset_dram_helper_app);
  pcie_helper_app(app_buf_addrs, device_pcie_write_req_apply,
                  device_pcie_write_req_data, app_output_data_splitted,
                  app_output_data_meta, app_free_buf, reset_pcie_helper_app);
  pcie_data_splitter_app(app_output_data, app_output_data_splitted,
                         app_output_data_meta, reset_pcie_data_splitter_app);

  ST_Queue<bool> reset_app_intg_matcher(4);
  ST_Queue<bool> reset_app_intg_mat_rdc(4);
  ST_Queue<bool> reset_app_intg_rdc_16to8(4);
  ST_Queue<bool> reset_app_intg_rdc_32to16(4);
  ST_Queue<bool> reset_app_intg_rdc_4to2(4);
  ST_Queue<bool> reset_app_intg_rdc_8to4(4);
  ST_Queue<bool> reset_app_intg_verifier(4);
  ST_Queue<bool> reset_app_intg_writer(4);
  reset_propaganda(reset_app_intg_matcher, reset_app_intg_mat_rdc,
                   reset_app_intg_rdc_16to8, reset_app_intg_rdc_32to16,
                   reset_app_intg_rdc_4to2, reset_app_intg_rdc_8to4,
                   reset_app_intg_verifier, reset_app_intg_writer, reset_sigs,
                   reset_dram_helper_app, reset_pcie_helper_app,
                   reset_pcie_data_splitter_app);

  ST_Queue<APP_Match> app_intg_mat_rdc_input(64);
  ST_Queue<APP_Ver_Param> app_intg_verifier_input_param(64);
  ST_Queue<APP_Ver_Record> app_intg_verifier_input_record(64);
  ST_Queue<APP_Reduce_Record32> app_intg_rdc_input_32to16(64);
  ST_Queue<APP_Reduce_Record16> app_intg_rdc_input_16to8(64);
  ST_Queue<APP_Reduce_Record8> app_intg_rdc_input_8to4(64);
  ST_Queue<APP_Reduce_Record4> app_intg_rdc_input_4to2(64);
  ST_Queue<APP_Reduce_Record2> app_intg_writer_input_record(64);
  ST_Queue<unsigned int> app_intg_writer_input_param(64);

  ;
  ;
  ;

  ;
  ;
  ;
  ;
  ;
  app_intg_matcher(reset_app_intg_matcher, app_input_params, app_input_data,
                   app_intg_writer_input_param, app_intg_verifier_input_param,
                   app_intg_mat_rdc_input);
  app_intg_mat_rdc(reset_app_intg_mat_rdc, app_intg_mat_rdc_input,
                   app_intg_verifier_input_record);
  app_intg_verifier(reset_app_intg_verifier, app_intg_verifier_input_param,
                    app_intg_verifier_input_record, app_intg_rdc_input_32to16);
  app_intg_rdc_32to16(reset_app_intg_rdc_32to16, app_intg_rdc_input_32to16,
                      app_intg_rdc_input_16to8);
  app_intg_rdc_16to8(reset_app_intg_rdc_16to8, app_intg_rdc_input_16to8,
                     app_intg_rdc_input_8to4);
  app_intg_rdc_8to4(reset_app_intg_rdc_8to4, app_intg_rdc_input_8to4,
                    app_intg_rdc_input_4to2);
  app_intg_rdc_4to2(reset_app_intg_rdc_4to2, app_intg_rdc_input_4to2,
                    app_intg_writer_input_record);
  app_intg_writer(reset_app_intg_writer, app_intg_writer_input_param,
                  app_intg_writer_input_record, app_output_data);
}
