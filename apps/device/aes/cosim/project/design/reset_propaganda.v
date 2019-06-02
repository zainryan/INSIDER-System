// ==============================================================
// RTL generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2017.4.op
// Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="reset_propaganda,hls_ip_2017_4_op,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xcvu9p-flgb2104-2-i,HLS_INPUT_CLOCK=4.000000,HLS_INPUT_ARCH=others,HLS_SYN_CLOCK=1.458000,HLS_SYN_LAT=-1,HLS_SYN_TPT=none,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=5,HLS_SYN_LUT=240}" *)

module reset_propaganda (
        ap_clk,
        ap_rst,
        ap_start,
        ap_done,
ap_continue,
        ap_idle,
        ap_ready,
        reset_app_aes_end_V_din,
        reset_app_aes_end_V_full_n,
        reset_app_aes_end_V_write,
        reset_app_aes_process_0_V_din,
        reset_app_aes_process_0_V_full_n,
        reset_app_aes_process_0_V_write,
        reset_app_aes_process_1_V_din,
        reset_app_aes_process_1_V_full_n,
        reset_app_aes_process_1_V_write,
        reset_app_aes_process_2_V_din,
        reset_app_aes_process_2_V_full_n,
        reset_app_aes_process_2_V_write,
        reset_app_aes_process_3_V_din,
        reset_app_aes_process_3_V_full_n,
        reset_app_aes_process_3_V_write,
        reset_app_aes_process_4_V_din,
        reset_app_aes_process_4_V_full_n,
        reset_app_aes_process_4_V_write,
        reset_app_aes_process_5_V_din,
        reset_app_aes_process_5_V_full_n,
        reset_app_aes_process_5_V_write,
        reset_app_aes_process_6_V_din,
        reset_app_aes_process_6_V_full_n,
        reset_app_aes_process_6_V_write,
        reset_app_aes_process_7_V_din,
        reset_app_aes_process_7_V_full_n,
        reset_app_aes_process_7_V_write,
        reset_app_aes_process_8_V_din,
        reset_app_aes_process_8_V_full_n,
        reset_app_aes_process_8_V_write,
        reset_app_aes_start_V_din,
        reset_app_aes_start_V_full_n,
        reset_app_aes_start_V_write,
        reset_sigs_V_dout,
        reset_sigs_V_empty_n,
        reset_sigs_V_read,
        reset_read_mode_dram_helper_app_V_din,
        reset_read_mode_dram_helper_app_V_full_n,
        reset_read_mode_dram_helper_app_V_write,
        reset_write_mode_dram_helper_app_V_din,
        reset_write_mode_dram_helper_app_V_full_n,
        reset_write_mode_dram_helper_app_V_write,
        reset_read_mode_pcie_helper_app_V_din,
        reset_read_mode_pcie_helper_app_V_full_n,
        reset_read_mode_pcie_helper_app_V_write,
        reset_write_mode_pcie_helper_app_V_din,
        reset_write_mode_pcie_helper_app_V_full_n,
        reset_write_mode_pcie_helper_app_V_write,
        reset_pcie_data_splitter_app_V_din,
        reset_pcie_data_splitter_app_V_full_n,
        reset_pcie_data_splitter_app_V_write,
        reset_app_output_data_demux_V_din,
        reset_app_output_data_demux_V_full_n,
        reset_app_output_data_demux_V_write,
        reset_app_input_data_mux_V_din,
        reset_app_input_data_mux_V_full_n,
        reset_app_input_data_mux_V_write,
        reset_write_mode_app_output_data_caching_V_din,
        reset_write_mode_app_output_data_caching_V_full_n,
        reset_write_mode_app_output_data_caching_V_write,
        reset_app_input_data_merger_V_din,
        reset_app_input_data_merger_V_full_n,
        reset_app_input_data_merger_V_write,
        reset_write_mode_pre_merged_app_input_data_forwarder_V_din,
        reset_write_mode_pre_merged_app_input_data_forwarder_V_full_n,
        reset_write_mode_pre_merged_app_input_data_forwarder_V_write
);

parameter    ap_ST_fsm_state1 = 2'd1;
parameter    ap_ST_fsm_pp0_stage0 = 2'd2;

input   ap_clk;
input   ap_rst;
input   ap_start;
output   ap_done;
output   ap_idle;
output ap_ready; output ap_continue;
output   reset_app_aes_end_V_din;
input   reset_app_aes_end_V_full_n;
output   reset_app_aes_end_V_write;
output   reset_app_aes_process_0_V_din;
input   reset_app_aes_process_0_V_full_n;
output   reset_app_aes_process_0_V_write;
output   reset_app_aes_process_1_V_din;
input   reset_app_aes_process_1_V_full_n;
output   reset_app_aes_process_1_V_write;
output   reset_app_aes_process_2_V_din;
input   reset_app_aes_process_2_V_full_n;
output   reset_app_aes_process_2_V_write;
output   reset_app_aes_process_3_V_din;
input   reset_app_aes_process_3_V_full_n;
output   reset_app_aes_process_3_V_write;
output   reset_app_aes_process_4_V_din;
input   reset_app_aes_process_4_V_full_n;
output   reset_app_aes_process_4_V_write;
output   reset_app_aes_process_5_V_din;
input   reset_app_aes_process_5_V_full_n;
output   reset_app_aes_process_5_V_write;
output   reset_app_aes_process_6_V_din;
input   reset_app_aes_process_6_V_full_n;
output   reset_app_aes_process_6_V_write;
output   reset_app_aes_process_7_V_din;
input   reset_app_aes_process_7_V_full_n;
output   reset_app_aes_process_7_V_write;
output   reset_app_aes_process_8_V_din;
input   reset_app_aes_process_8_V_full_n;
output   reset_app_aes_process_8_V_write;
output   reset_app_aes_start_V_din;
input   reset_app_aes_start_V_full_n;
output   reset_app_aes_start_V_write;
input   reset_sigs_V_dout;
input   reset_sigs_V_empty_n;
output   reset_sigs_V_read;
output   reset_read_mode_dram_helper_app_V_din;
input   reset_read_mode_dram_helper_app_V_full_n;
output   reset_read_mode_dram_helper_app_V_write;
output   reset_write_mode_dram_helper_app_V_din;
input   reset_write_mode_dram_helper_app_V_full_n;
output   reset_write_mode_dram_helper_app_V_write;
output   reset_read_mode_pcie_helper_app_V_din;
input   reset_read_mode_pcie_helper_app_V_full_n;
output   reset_read_mode_pcie_helper_app_V_write;
output   reset_write_mode_pcie_helper_app_V_din;
input   reset_write_mode_pcie_helper_app_V_full_n;
output   reset_write_mode_pcie_helper_app_V_write;
output   reset_pcie_data_splitter_app_V_din;
input   reset_pcie_data_splitter_app_V_full_n;
output   reset_pcie_data_splitter_app_V_write;
output   reset_app_output_data_demux_V_din;
input   reset_app_output_data_demux_V_full_n;
output   reset_app_output_data_demux_V_write;
output   reset_app_input_data_mux_V_din;
input   reset_app_input_data_mux_V_full_n;
output   reset_app_input_data_mux_V_write;
output   reset_write_mode_app_output_data_caching_V_din;
input   reset_write_mode_app_output_data_caching_V_full_n;
output   reset_write_mode_app_output_data_caching_V_write;
output   reset_app_input_data_merger_V_din;
input   reset_app_input_data_merger_V_full_n;
output   reset_app_input_data_merger_V_write;
output   reset_write_mode_pre_merged_app_input_data_forwarder_V_din;
input   reset_write_mode_pre_merged_app_input_data_forwarder_V_full_n;
output   reset_write_mode_pre_merged_app_input_data_forwarder_V_write;

reg ap_idle;
reg reset_app_aes_end_V_write;
reg reset_app_aes_process_0_V_write;
reg reset_app_aes_process_1_V_write;
reg reset_app_aes_process_2_V_write;
reg reset_app_aes_process_3_V_write;
reg reset_app_aes_process_4_V_write;
reg reset_app_aes_process_5_V_write;
reg reset_app_aes_process_6_V_write;
reg reset_app_aes_process_7_V_write;
reg reset_app_aes_process_8_V_write;
reg reset_app_aes_start_V_write;
reg reset_sigs_V_read;
reg reset_read_mode_dram_helper_app_V_write;
reg reset_write_mode_dram_helper_app_V_write;
reg reset_read_mode_pcie_helper_app_V_write;
reg reset_write_mode_pcie_helper_app_V_write;
reg reset_pcie_data_splitter_app_V_write;
reg reset_app_output_data_demux_V_write;
reg reset_app_input_data_mux_V_write;
reg reset_write_mode_app_output_data_caching_V_write;
reg reset_app_input_data_merger_V_write;
reg reset_write_mode_pre_merged_app_input_data_forwarder_V_write;

(* fsm_encoding = "none" *) reg   [1:0] ap_CS_fsm;
wire    ap_CS_fsm_state1;
reg    reset_app_aes_end_V_blk_n;
wire    ap_CS_fsm_pp0_stage0;
reg    ap_enable_reg_pp0_iter1;
wire    ap_block_pp0_stage0;
reg   [0:0] empty_n_reg_522;
reg    reset_app_aes_process_0_V_blk_n;
reg    reset_app_aes_process_1_V_blk_n;
reg    reset_app_aes_process_2_V_blk_n;
reg    reset_app_aes_process_3_V_blk_n;
reg    reset_app_aes_process_4_V_blk_n;
reg    reset_app_aes_process_5_V_blk_n;
reg    reset_app_aes_process_6_V_blk_n;
reg    reset_app_aes_process_7_V_blk_n;
reg    reset_app_aes_process_8_V_blk_n;
reg    reset_app_aes_start_V_blk_n;
reg    reset_read_mode_dram_helper_app_V_blk_n;
reg    reset_write_mode_dram_helper_app_V_blk_n;
reg    reset_read_mode_pcie_helper_app_V_blk_n;
reg    reset_write_mode_pcie_helper_app_V_blk_n;
reg    reset_pcie_data_splitter_app_V_blk_n;
reg    reset_app_output_data_demux_V_blk_n;
reg    reset_app_input_data_mux_V_blk_n;
reg    reset_write_mode_app_output_data_caching_V_blk_n;
reg    reset_app_input_data_merger_V_blk_n;
reg    reset_write_mode_pre_merged_app_input_data_forwarder_V_blk_n;
wire    ap_block_state2_pp0_stage0_iter0;
reg    ap_block_state3_pp0_stage0_iter1;
reg    ap_block_pp0_stage0_11001;
reg    ap_enable_reg_pp0_iter0;
reg    ap_block_pp0_stage0_subdone;
reg    ap_block_pp0_stage0_01001;
reg   [1:0] ap_NS_fsm;
reg    ap_idle_pp0;
wire    ap_enable_pp0;

// power-on initialization
initial begin
#0 ap_CS_fsm = 2'd1;
#0 ap_enable_reg_pp0_iter1 = 1'b0;
#0 ap_enable_reg_pp0_iter0 = 1'b0;
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_CS_fsm <= ap_ST_fsm_state1;
    end else begin
        ap_CS_fsm <= ap_NS_fsm;
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter0 <= 1'b0;
    end else begin
        if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
            ap_enable_reg_pp0_iter0 <= 1'b1;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter1 <= 1'b0;
    end else begin
        if ((1'b0 == ap_block_pp0_stage0_subdone)) begin
            ap_enable_reg_pp0_iter1 <= ap_enable_reg_pp0_iter0;
        end else if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
            ap_enable_reg_pp0_iter1 <= 1'b0;
        end
    end
end

always @ (posedge ap_clk) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        empty_n_reg_522 <= reset_sigs_V_empty_n;
    end
end

always @ (*) begin
    if (((ap_start == 1'b0) & (1'b1 == ap_CS_fsm_state1))) begin
        ap_idle = 1'b1;
    end else begin
        ap_idle = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter0 == 1'b0) & (ap_enable_reg_pp0_iter1 == 1'b0))) begin
        ap_idle_pp0 = 1'b1;
    end else begin
        ap_idle_pp0 = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_aes_end_V_blk_n = reset_app_aes_end_V_full_n;
    end else begin
        reset_app_aes_end_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_aes_end_V_write = 1'b1;
    end else begin
        reset_app_aes_end_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_aes_process_0_V_blk_n = reset_app_aes_process_0_V_full_n;
    end else begin
        reset_app_aes_process_0_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_aes_process_0_V_write = 1'b1;
    end else begin
        reset_app_aes_process_0_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_aes_process_1_V_blk_n = reset_app_aes_process_1_V_full_n;
    end else begin
        reset_app_aes_process_1_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_aes_process_1_V_write = 1'b1;
    end else begin
        reset_app_aes_process_1_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_aes_process_2_V_blk_n = reset_app_aes_process_2_V_full_n;
    end else begin
        reset_app_aes_process_2_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_aes_process_2_V_write = 1'b1;
    end else begin
        reset_app_aes_process_2_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_aes_process_3_V_blk_n = reset_app_aes_process_3_V_full_n;
    end else begin
        reset_app_aes_process_3_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_aes_process_3_V_write = 1'b1;
    end else begin
        reset_app_aes_process_3_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_aes_process_4_V_blk_n = reset_app_aes_process_4_V_full_n;
    end else begin
        reset_app_aes_process_4_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_aes_process_4_V_write = 1'b1;
    end else begin
        reset_app_aes_process_4_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_aes_process_5_V_blk_n = reset_app_aes_process_5_V_full_n;
    end else begin
        reset_app_aes_process_5_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_aes_process_5_V_write = 1'b1;
    end else begin
        reset_app_aes_process_5_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_aes_process_6_V_blk_n = reset_app_aes_process_6_V_full_n;
    end else begin
        reset_app_aes_process_6_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_aes_process_6_V_write = 1'b1;
    end else begin
        reset_app_aes_process_6_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_aes_process_7_V_blk_n = reset_app_aes_process_7_V_full_n;
    end else begin
        reset_app_aes_process_7_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_aes_process_7_V_write = 1'b1;
    end else begin
        reset_app_aes_process_7_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_aes_process_8_V_blk_n = reset_app_aes_process_8_V_full_n;
    end else begin
        reset_app_aes_process_8_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_aes_process_8_V_write = 1'b1;
    end else begin
        reset_app_aes_process_8_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_aes_start_V_blk_n = reset_app_aes_start_V_full_n;
    end else begin
        reset_app_aes_start_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_aes_start_V_write = 1'b1;
    end else begin
        reset_app_aes_start_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_input_data_merger_V_blk_n = reset_app_input_data_merger_V_full_n;
    end else begin
        reset_app_input_data_merger_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_input_data_merger_V_write = 1'b1;
    end else begin
        reset_app_input_data_merger_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_input_data_mux_V_blk_n = reset_app_input_data_mux_V_full_n;
    end else begin
        reset_app_input_data_mux_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_input_data_mux_V_write = 1'b1;
    end else begin
        reset_app_input_data_mux_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_app_output_data_demux_V_blk_n = reset_app_output_data_demux_V_full_n;
    end else begin
        reset_app_output_data_demux_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_app_output_data_demux_V_write = 1'b1;
    end else begin
        reset_app_output_data_demux_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_pcie_data_splitter_app_V_blk_n = reset_pcie_data_splitter_app_V_full_n;
    end else begin
        reset_pcie_data_splitter_app_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_pcie_data_splitter_app_V_write = 1'b1;
    end else begin
        reset_pcie_data_splitter_app_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_read_mode_dram_helper_app_V_blk_n = reset_read_mode_dram_helper_app_V_full_n;
    end else begin
        reset_read_mode_dram_helper_app_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_read_mode_dram_helper_app_V_write = 1'b1;
    end else begin
        reset_read_mode_dram_helper_app_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_read_mode_pcie_helper_app_V_blk_n = reset_read_mode_pcie_helper_app_V_full_n;
    end else begin
        reset_read_mode_pcie_helper_app_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_read_mode_pcie_helper_app_V_write = 1'b1;
    end else begin
        reset_read_mode_pcie_helper_app_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (reset_sigs_V_empty_n == 1'b1) & (ap_enable_reg_pp0_iter0 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        reset_sigs_V_read = 1'b1;
    end else begin
        reset_sigs_V_read = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_write_mode_app_output_data_caching_V_blk_n = reset_write_mode_app_output_data_caching_V_full_n;
    end else begin
        reset_write_mode_app_output_data_caching_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_write_mode_app_output_data_caching_V_write = 1'b1;
    end else begin
        reset_write_mode_app_output_data_caching_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_write_mode_dram_helper_app_V_blk_n = reset_write_mode_dram_helper_app_V_full_n;
    end else begin
        reset_write_mode_dram_helper_app_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_write_mode_dram_helper_app_V_write = 1'b1;
    end else begin
        reset_write_mode_dram_helper_app_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_write_mode_pcie_helper_app_V_blk_n = reset_write_mode_pcie_helper_app_V_full_n;
    end else begin
        reset_write_mode_pcie_helper_app_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_write_mode_pcie_helper_app_V_write = 1'b1;
    end else begin
        reset_write_mode_pcie_helper_app_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1) & (1'b0 == ap_block_pp0_stage0))) begin
        reset_write_mode_pre_merged_app_input_data_forwarder_V_blk_n = reset_write_mode_pre_merged_app_input_data_forwarder_V_full_n;
    end else begin
        reset_write_mode_pre_merged_app_input_data_forwarder_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (empty_n_reg_522 == 1'd1))) begin
        reset_write_mode_pre_merged_app_input_data_forwarder_V_write = 1'b1;
    end else begin
        reset_write_mode_pre_merged_app_input_data_forwarder_V_write = 1'b0;
    end
end

always @ (*) begin
    case (ap_CS_fsm)
        ap_ST_fsm_state1 : begin
            if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
                ap_NS_fsm = ap_ST_fsm_pp0_stage0;
            end else begin
                ap_NS_fsm = ap_ST_fsm_state1;
            end
        end
        ap_ST_fsm_pp0_stage0 : begin
            ap_NS_fsm = ap_ST_fsm_pp0_stage0;
        end
        default : begin
            ap_NS_fsm = 'bx;
        end
    endcase
end

assign ap_CS_fsm_pp0_stage0 = ap_CS_fsm[32'd1];

assign ap_CS_fsm_state1 = ap_CS_fsm[32'd0];

assign ap_block_pp0_stage0 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_pp0_stage0_01001 = ((ap_enable_reg_pp0_iter1 == 1'b1) & (((reset_app_aes_start_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_8_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_7_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_6_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_5_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_4_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_3_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_2_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_1_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_0_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_end_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_pre_merged_app_input_data_forwarder_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_input_data_merger_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_app_output_data_caching_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_input_data_mux_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_output_data_demux_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_pcie_data_splitter_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_pcie_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_read_mode_pcie_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_dram_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_read_mode_dram_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1))));
end

always @ (*) begin
    ap_block_pp0_stage0_11001 = ((ap_enable_reg_pp0_iter1 == 1'b1) & (((reset_app_aes_start_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_8_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_7_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_6_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_5_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_4_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_3_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_2_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_1_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_0_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_end_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_pre_merged_app_input_data_forwarder_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_input_data_merger_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_app_output_data_caching_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_input_data_mux_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_output_data_demux_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_pcie_data_splitter_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_pcie_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_read_mode_pcie_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_dram_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_read_mode_dram_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1))));
end

always @ (*) begin
    ap_block_pp0_stage0_subdone = ((ap_enable_reg_pp0_iter1 == 1'b1) & (((reset_app_aes_start_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_8_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_7_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_6_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_5_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_4_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_3_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_2_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_1_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_0_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_end_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_pre_merged_app_input_data_forwarder_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_input_data_merger_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_app_output_data_caching_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_input_data_mux_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_output_data_demux_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_pcie_data_splitter_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_pcie_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_read_mode_pcie_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_dram_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_read_mode_dram_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1))));
end

assign ap_block_state2_pp0_stage0_iter0 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_state3_pp0_stage0_iter1 = (((reset_app_aes_start_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_8_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_7_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_6_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_5_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_4_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_3_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_2_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_1_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_process_0_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_aes_end_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_pre_merged_app_input_data_forwarder_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_input_data_merger_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_app_output_data_caching_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_input_data_mux_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_app_output_data_demux_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_pcie_data_splitter_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_pcie_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_read_mode_pcie_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_write_mode_dram_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)) | ((reset_read_mode_dram_helper_app_V_full_n == 1'b0) & (empty_n_reg_522 == 1'd1)));
end

assign ap_done = 1'b0;

assign ap_enable_pp0 = (ap_idle_pp0 ^ 1'b1);

assign ap_ready = 1'b0;

assign reset_app_aes_end_V_din = 1'd0;

assign reset_app_aes_process_0_V_din = 1'd0;

assign reset_app_aes_process_1_V_din = 1'd0;

assign reset_app_aes_process_2_V_din = 1'd0;

assign reset_app_aes_process_3_V_din = 1'd0;

assign reset_app_aes_process_4_V_din = 1'd0;

assign reset_app_aes_process_5_V_din = 1'd0;

assign reset_app_aes_process_6_V_din = 1'd0;

assign reset_app_aes_process_7_V_din = 1'd0;

assign reset_app_aes_process_8_V_din = 1'd0;

assign reset_app_aes_start_V_din = 1'd0;

assign reset_app_input_data_merger_V_din = 1'd0;

assign reset_app_input_data_mux_V_din = 1'd0;

assign reset_app_output_data_demux_V_din = 1'd0;

assign reset_pcie_data_splitter_app_V_din = 1'd0;

assign reset_read_mode_dram_helper_app_V_din = 1'd0;

assign reset_read_mode_pcie_helper_app_V_din = 1'd0;

assign reset_write_mode_app_output_data_caching_V_din = 1'd0;

assign reset_write_mode_dram_helper_app_V_din = 1'd0;

assign reset_write_mode_pcie_helper_app_V_din = 1'd0;

assign reset_write_mode_pre_merged_app_input_data_forwarder_V_din = 1'd0;

endmodule //reset_propaganda
