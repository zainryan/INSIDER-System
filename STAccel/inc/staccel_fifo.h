#ifndef STACCEL_FIFO_
#define STACCEL_FIFO_

#ifndef STACCEL_SYN
// DRAM-A interface
ST_Queue<Dram_Read_Req> dramA_read_req(1);
ST_Queue<Dram_Read_Resp> dramA_read_resp(1);
ST_Queue<Dram_Write_Req_Data> dramA_write_req_data(1);
ST_Queue<Dram_Write_Req_Apply> dramA_write_req_apply(1);

// DRAM-B interface
ST_Queue<Dram_Read_Req> dramB_read_req(1);
ST_Queue<Dram_Read_Resp> dramB_read_resp(1);
ST_Queue<Dram_Write_Req_Data> dramB_write_req_data(1);
ST_Queue<Dram_Write_Req_Apply> dramB_write_req_apply(1);

// DRAM-C intnerface
ST_Queue<Dram_Read_Req> dramC_read_req(1);
ST_Queue<Dram_Read_Resp> dramC_read_resp(1);
ST_Queue<Dram_Write_Req_Data> dramC_write_req_data(1);
ST_Queue<Dram_Write_Req_Apply> dramC_write_req_apply(1);

// DRAM-D interface
ST_Queue<Dram_Read_Req> dramD_read_req(1);
ST_Queue<Dram_Read_Resp> dramD_read_resp(1);
ST_Queue<Dram_Write_Req_Data> dramD_write_req_data(1);
ST_Queue<Dram_Write_Req_Apply> dramD_write_req_apply(1);

// PCIe interface
ST_Queue<PCIe_Read_Req> pcie_read_req(16);
ST_Queue<PCIe_Read_Resp> pcie_read_resp(16);
ST_Queue<PCIe_Write_Req_Data> pcie_write_req_data(16);
ST_Queue<PCIe_Write_Req_Apply> pcie_write_req_apply(16);

// Poke-Peek interface
ST_Queue<Poke_Info> poke(1);
ST_Queue<Peek_Info> peek_req(1);
ST_Queue<Peek_Info> peek_resp(1);

#else
// DRAM-A interface
hls::stream<Dram_Read_Req> dramA_read_req;
hls::stream<Dram_Read_Resp> dramA_read_resp;
hls::stream<Dram_Write_Req_Data> dramA_write_req_data;
hls::stream<Dram_Write_Req_Apply> dramA_write_req_apply;

// DRAM-B interface
hls::stream<Dram_Read_Req> dramB_read_req;
hls::stream<Dram_Read_Resp> dramB_read_resp;
hls::stream<Dram_Write_Req_Data> dramB_write_req_data;
hls::stream<Dram_Write_Req_Apply> dramB_write_req_apply;

// DRAM-C intnerface
hls::stream<Dram_Read_Req> dramC_read_req;
hls::stream<Dram_Read_Resp> dramC_read_resp;
hls::stream<Dram_Write_Req_Data> dramC_write_req_data;
hls::stream<Dram_Write_Req_Apply> dramC_write_req_apply;

// DRAM-D interface
hls::stream<Dram_Read_Req> dramD_read_req;
hls::stream<Dram_Read_Resp> dramD_read_resp;
hls::stream<Dram_Write_Req_Data> dramD_write_req_data;
hls::stream<Dram_Write_Req_Apply> dramD_write_req_apply;

// PCIe interface
hls::stream<PCIe_Read_Req> pcie_read_req;
hls::stream<PCIe_Read_Resp> pcie_read_resp;
hls::stream<PCIe_Write_Req_Data> pcie_write_req_data;
hls::stream<PCIe_Write_Req_Apply> pcie_write_req_apply;

// Poke-Peek interface
hls::stream<Poke_Info> poke;
hls::stream<Peek_Info> peek_req;
hls::stream<Peek_Info> peek_resp;
#endif

#endif
