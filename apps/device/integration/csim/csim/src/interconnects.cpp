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

#define FILE_ROW_NUM (100)
#define FILE_COL_NUM (64) // do not change it
#define APP_QUERY_THRES (1)
#define APP_RECORD_THRES (1)
#define APP_QUERY_LENGTH (32)
#define APP_RECORD_LENGTH (FILE_COL_NUM)
#define READ_BUF_SIZE (2 * 1024 * 1024)
using namespace std;

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

  ST_Queue<bool> reset_app_intg_matcher(4);
  ST_Queue<bool> reset_app_intg_mat_rdc(4);
  ST_Queue<bool> reset_app_intg_rdc_16to8(4);
  ST_Queue<bool> reset_app_intg_rdc_32to16(4);
  ST_Queue<bool> reset_app_intg_rdc_4to2(4);
  ST_Queue<bool> reset_app_intg_rdc_8to4(4);
  ST_Queue<bool> reset_app_intg_verifier(4);
  ST_Queue<bool> reset_app_intg_writer(4);

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

  std::thread t0(command_handler, std::ref(poke), std::ref(reqs_incoming),
                 std::ref(kbuf_addrs), std::ref(dma_read_throttle_params),
                 std::ref(dma_write_throttle_params),
                 std::ref(drive_read_delay_cycle_cnts),
                 std::ref(drive_write_delay_cycle_cnts),
                 std::ref(drive_read_throttle_params),
                 std::ref(drive_write_throttle_params),
                 std::ref(app_file_infos), std::ref(app_buf_addrs),
                 std::ref(app_free_buf), std::ref(app_input_params),
                 std::ref(reset_sigs));
  std::thread t1(pcie_read_resp_passer, std::ref(pcie_read_resp),
                 std::ref(before_throttle_pcie_read_resp));
  std::thread t2(pcie_read_throttle_unit, std::ref(dma_read_throttle_params),
                 std::ref(before_throttle_pcie_read_resp),
                 std::ref(after_throttle_pcie_read_resp));
  std::thread t3(
      pipe0_data_handler, std::ref(kbuf_addrs), std::ref(reqs_incoming),
      std::ref(host_dram_read_req), std::ref(pcie_read_req),
      std::ref(rd_reqs_pipe0_write), std::ref(wr_reqs_pipe0_write),
      std::ref(rd_kbuf_addr_pipe0_write), std::ref(wr_kbuf_addr_pipe0_write));
  std::thread t4(pipe1_data_handler, std::ref(rd_reqs_pipe1_read),
                 std::ref(wr_reqs_pipe1_read), std::ref(host_dram_read_resp),
                 std::ref(after_throttle_pcie_read_resp),
                 std::ref(rd_reqs_pipe1_write), std::ref(wr_reqs_pipe1_write),
                 std::ref(rd_data_valid_pipe1_write),
                 std::ref(wr_data_valid_pipe1_write), std::ref(data_pipe2),
                 std::ref(host_dram_write_req_data));
  std::thread t5(
      pipe2_data_handler, std::ref(rd_reqs_pipe2_read),
      std::ref(wr_reqs_pipe2_read), std::ref(rd_data_valid_pipe2_read),
      std::ref(wr_data_valid_pipe2_read), std::ref(rd_kbuf_addr_pipe2_read),
      std::ref(wr_kbuf_addr_pipe2_read), std::ref(data_pipe2),
      std::ref(host_data_pcie_write_req_apply),
      std::ref(host_data_pcie_write_req_data),
      std::ref(host_rdcmd_fin_pcie_write_req_apply),
      std::ref(host_rdcmd_fin_pcie_write_req_data),
      std::ref(host_dram_write_req_apply), std::ref(wrcmd_kbuf_addrs));
  std::thread t6(
      pipeline_data_passer, std::ref(rd_reqs_pipe0_write),
      std::ref(wr_reqs_pipe0_write), std::ref(rd_reqs_pipe1_read),
      std::ref(wr_reqs_pipe1_read), std::ref(rd_reqs_pipe1_write),
      std::ref(wr_reqs_pipe1_write), std::ref(rd_reqs_pipe2_read),
      std::ref(wr_reqs_pipe2_read), std::ref(rd_data_valid_pipe1_write),
      std::ref(wr_data_valid_pipe1_write), std::ref(rd_data_valid_pipe2_read),
      std::ref(wr_data_valid_pipe2_read), std::ref(rd_kbuf_addr_pipe0_write),
      std::ref(wr_kbuf_addr_pipe0_write), std::ref(rd_kbuf_addr_pipe2_read),
      std::ref(wr_kbuf_addr_pipe2_read));
  std::thread t7(pcie_write_multiplexer,
                 std::ref(host_data_pcie_write_req_data),
                 std::ref(host_data_pcie_write_req_apply),
                 std::ref(before_throttle_pcie_write_req_data),
                 std::ref(before_throttle_pcie_write_req_apply),
                 std::ref(host_rdcmd_fin_pcie_write_req_data),
                 std::ref(host_rdcmd_fin_pcie_write_req_apply),
                 std::ref(host_wrcmd_fin_pcie_write_req_data),
                 std::ref(host_wrcmd_fin_pcie_write_req_apply),
                 std::ref(device_pcie_write_req_data),
                 std::ref(device_pcie_write_req_apply));
  std::thread t8(pcie_write_throttle_unit, std::ref(dma_write_throttle_params),
                 std::ref(before_throttle_pcie_write_req_data),
                 std::ref(before_throttle_pcie_write_req_apply),
                 std::ref(pcie_write_req_data), std::ref(pcie_write_req_apply));
  std::thread t9(dram_read_req_multiplexer, std::ref(host_dram_read_req),
                 std::ref(device_dram_read_req),
                 std::ref(before_delay_unified_dram_read_req),
                 std::ref(dram_read_context));
  std::thread t10(dram_read_resp_multiplexer, std::ref(host_dram_read_resp),
                  std::ref(device_dram_read_resp),
                  std::ref(after_throttle_unified_dram_read_resp),
                  std::ref(dram_read_context));
  std::thread t11(dram_read_throttle_unit, std::ref(drive_read_throttle_params),
                  std::ref(before_throttle_unified_dram_read_resp),
                  std::ref(after_throttle_unified_dram_read_resp));
  std::thread t12(dram_write_req_time_marker,
                  std::ref(host_dram_write_req_data),
                  std::ref(host_dram_write_req_apply),
                  std::ref(before_delay_dram_write_req_data),
                  std::ref(before_delay_dram_write_req_apply_with_time));
  std::thread t13(dram_write_delay_unit, std::ref(drive_write_delay_cycle_cnts),
                  std::ref(before_delay_dram_write_req_data),
                  std::ref(before_delay_dram_write_req_apply_with_time),
                  std::ref(before_throttle_unified_dram_write_req_data),
                  std::ref(before_throttle_unified_dram_write_req_apply));
  std::thread t14(dram_write_throttle_unit,
                  std::ref(drive_write_throttle_params),
                  std::ref(before_throttle_unified_dram_write_req_data),
                  std::ref(before_throttle_unified_dram_write_req_apply),
                  std::ref(after_throttle_unified_dram_write_req_data),
                  std::ref(after_throttle_unified_dram_write_req_apply));
  std::thread t15(dram_read_req_time_marker,
                  std::ref(before_delay_unified_dram_read_req),
                  std::ref(before_delay_unified_dram_read_req_with_time));
  std::thread t16(dram_read_delay_unit, std::ref(drive_read_delay_cycle_cnts),
                  std::ref(before_delay_unified_dram_read_req_with_time),
                  std::ref(after_delay_unified_dram_read_req));
  std::thread t17(pipe0_dram_dispatcher,
                  std::ref(after_delay_unified_dram_read_req),
                  std::ref(after_throttle_unified_dram_write_req_apply),
                  std::ref(dramA_read_req), std::ref(dramA_write_req_apply),
                  std::ref(dramB_read_req), std::ref(dramB_write_req_apply),
                  std::ref(dramC_read_req), std::ref(dramC_write_req_apply),
                  std::ref(dramD_read_req), std::ref(dramD_write_req_apply),
                  std::ref(unified_dram_dispatcher_write_context),
                  std::ref(unified_dram_dispatcher_read_context));
  std::thread t18(pipe1_dram_dispatcher,
                  std::ref(before_throttle_unified_dram_read_resp),
                  std::ref(after_throttle_unified_dram_write_req_data),
                  std::ref(dramA_read_resp), std::ref(dramA_write_req_data),
                  std::ref(dramB_read_resp), std::ref(dramB_write_req_data),
                  std::ref(dramC_read_resp), std::ref(dramC_write_req_data),
                  std::ref(dramD_read_resp), std::ref(dramD_write_req_data),
                  std::ref(unified_dram_dispatcher_write_context),
                  std::ref(unified_dram_dispatcher_read_context),
                  std::ref(host_wrcmd_fin_pcie_write_req_data),
                  std::ref(host_wrcmd_fin_pcie_write_req_apply),
                  std::ref(wrcmd_kbuf_addrs));
  std::thread t19(dram_helper_app, std::ref(app_file_infos),
                  std::ref(device_dram_read_req),
                  std::ref(device_dram_read_resp), std::ref(app_input_data),
                  std::ref(reset_dram_helper_app));
  std::thread t20(pcie_helper_app, std::ref(app_buf_addrs),
                  std::ref(device_pcie_write_req_apply),
                  std::ref(device_pcie_write_req_data),
                  std::ref(app_output_data_splitted),
                  std::ref(app_output_data_meta), std::ref(app_free_buf),
                  std::ref(reset_pcie_helper_app));
  std::thread t21(pcie_data_splitter_app, std::ref(app_output_data),
                  std::ref(app_output_data_splitted),
                  std::ref(app_output_data_meta),
                  std::ref(reset_pcie_data_splitter_app));
  std::thread t22(
      reset_propaganda, std::ref(reset_app_intg_matcher),
      std::ref(reset_app_intg_mat_rdc), std::ref(reset_app_intg_rdc_16to8),
      std::ref(reset_app_intg_rdc_32to16), std::ref(reset_app_intg_rdc_4to2),
      std::ref(reset_app_intg_rdc_8to4), std::ref(reset_app_intg_verifier),
      std::ref(reset_app_intg_writer), std::ref(reset_sigs),
      std::ref(reset_dram_helper_app), std::ref(reset_pcie_helper_app),
      std::ref(reset_pcie_data_splitter_app));
  std::thread t23(app_intg_matcher, std::ref(reset_app_intg_matcher),
                  std::ref(app_input_params), std::ref(app_input_data),
                  std::ref(app_intg_writer_input_param),
                  std::ref(app_intg_verifier_input_param),
                  std::ref(app_intg_mat_rdc_input));
  std::thread t24(app_intg_mat_rdc, std::ref(reset_app_intg_mat_rdc),
                  std::ref(app_intg_mat_rdc_input),
                  std::ref(app_intg_verifier_input_record));
  std::thread t25(app_intg_verifier, std::ref(reset_app_intg_verifier),
                  std::ref(app_intg_verifier_input_param),
                  std::ref(app_intg_verifier_input_record),
                  std::ref(app_intg_rdc_input_32to16));
  std::thread t26(app_intg_rdc_32to16, std::ref(reset_app_intg_rdc_32to16),
                  std::ref(app_intg_rdc_input_32to16),
                  std::ref(app_intg_rdc_input_16to8));
  std::thread t27(app_intg_rdc_16to8, std::ref(reset_app_intg_rdc_16to8),
                  std::ref(app_intg_rdc_input_16to8),
                  std::ref(app_intg_rdc_input_8to4));
  std::thread t28(app_intg_rdc_8to4, std::ref(reset_app_intg_rdc_8to4),
                  std::ref(app_intg_rdc_input_8to4),
                  std::ref(app_intg_rdc_input_4to2));
  std::thread t29(app_intg_rdc_4to2, std::ref(reset_app_intg_rdc_4to2),
                  std::ref(app_intg_rdc_input_4to2),
                  std::ref(app_intg_writer_input_record));
  std::thread t30(app_intg_writer, std::ref(reset_app_intg_writer),
                  std::ref(app_intg_writer_input_param),
                  std::ref(app_intg_writer_input_record),
                  std::ref(app_output_data));
  std::thread t31(host_pcie_simulator);
  std::thread t32(fpga_dramA_simulator);
  std::thread t33(fpga_dramB_simulator);
  std::thread t34(fpga_dramC_simulator);
  std::thread t35(fpga_dramD_simulator);

  simulator();
  while (1)
    ;
}

unsigned char query[32];
unsigned char raw_data[FILE_COL_NUM * FILE_ROW_NUM];
unsigned char expected_output_buf[FILE_ROW_NUM * APP_RECORD_LENGTH];

char random_char() {
  int min_ascii = 65;
  int max_ascii = 90;
  return min_ascii + rand() % (max_ascii - min_ascii + 1);
}

void integration() {
  unsigned char *expected_buf_ptr = expected_output_buf;
  unsigned char *record = raw_data;
  int numOfMatch = 0;
  bool match;
  for (int record_idx = 0; record_idx < FILE_ROW_NUM; record_idx++) {
    match = false;
    for (int i = 0; i < APP_QUERY_THRES; i++) {
      for (int j = 0; j < APP_RECORD_THRES; j++) {
        if (query[i] == record[j]) {
          match = true;
        }
      }
    }
    if (match) {
      int overlap = 0;
      for (int i = 0; i < APP_QUERY_LENGTH; i++) {
        for (int j = 0; j < APP_QUERY_LENGTH; j++) {
          if (query[i] == record[j]) {
            overlap++;
          }
        }
      }
      if (overlap > APP_QUERY_LENGTH - APP_QUERY_THRES) {
        numOfMatch++;
        for (int i = 0; i < APP_RECORD_LENGTH; i++) {
          *expected_buf_ptr++ = record[i];
        }
      }
    }
    record += APP_RECORD_LENGTH;
  }
  cout << "expected number of matches is: " << numOfMatch << " ("
       << FILE_ROW_NUM << " in total)" << endl;
}

int gen_data() {
  for (int i = 0; i < FILE_ROW_NUM * FILE_COL_NUM; i++) {
    raw_data[i] = random_char();
  }
  for (int i = 0; i < 32; i++) {
    query[i] = random_char();
  }
}

void user_simulation_function() {
  // PUT YOUR CODE HERE
  gen_data();
  set_physical_file((unsigned char *)raw_data, sizeof(raw_data));

  send_input_param(((APP_RECORD_THRES << 16) | (APP_QUERY_THRES)));
  unsigned int *query_ptr_in_4B = (unsigned int *)query;
  for (int i = 0; i < 8; i++) {
    send_input_param(query_ptr_in_4B[i]);
  }

  cout << "send input params ends..." << endl;

  integration();

  cout << "expected data generated..." << endl;

  int fd = vopen("csim_phy_file", 0);
  unsigned char *buf = (unsigned char *)malloc(READ_BUF_SIZE);
  bool fin_file = false;

  cout << "file opened..." << endl;

  unsigned char *expected_output_buf_in_bytes =
      (unsigned char *)expected_output_buf;

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
    cout << read_bytes << endl;
    for (int i = 0; i < read_bytes; i++) {
      unsigned char real = buf[i];
      unsigned char expected = expected_output_buf_in_bytes[i];
      cout << real << " : " << expected << endl;
      if (real != expected) {
        std::cout << "Failed!" << std::endl;
        exit(-1);
      }
    }
    expected_output_buf_in_bytes += read_bytes;
  }
  std::cout << "Passed!" << std::endl;
  vclose(fd);
}
