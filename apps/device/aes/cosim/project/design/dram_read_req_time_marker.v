// ==============================================================
// RTL generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2017.4.op
// Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="dram_read_req_time_marker,hls_ip_2017_4_op,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xcvu9p-flgb2104-2-i,HLS_INPUT_CLOCK=4.000000,HLS_INPUT_ARCH=others,HLS_SYN_CLOCK=2.997000,HLS_SYN_LAT=-1,HLS_SYN_TPT=none,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=506,HLS_SYN_LUT=392}" *)

module dram_read_req_time_marker (
        ap_clk,
        ap_rst,
        ap_start,
        ap_done,
ap_continue,
        ap_idle,
        ap_ready,
        before_delay_host_dram_read_req_V_num_dout,
        before_delay_host_dram_read_req_V_num_empty_n,
        before_delay_host_dram_read_req_V_num_read,
        before_delay_host_dram_read_req_V_addr_dout,
        before_delay_host_dram_read_req_V_addr_empty_n,
        before_delay_host_dram_read_req_V_addr_read,
        before_delay_host_dram_read_req_with_time_V_req_num_din,
        before_delay_host_dram_read_req_with_time_V_req_num_full_n,
        before_delay_host_dram_read_req_with_time_V_req_num_write,
        before_delay_host_dram_read_req_with_time_V_req_addr_din,
        before_delay_host_dram_read_req_with_time_V_req_addr_full_n,
        before_delay_host_dram_read_req_with_time_V_req_addr_write,
        before_delay_host_dram_read_req_with_time_V_time_din,
        before_delay_host_dram_read_req_with_time_V_time_full_n,
        before_delay_host_dram_read_req_with_time_V_time_write,
        before_delay_device_dram_read_req_V_num_dout,
        before_delay_device_dram_read_req_V_num_empty_n,
        before_delay_device_dram_read_req_V_num_read,
        before_delay_device_dram_read_req_V_addr_dout,
        before_delay_device_dram_read_req_V_addr_empty_n,
        before_delay_device_dram_read_req_V_addr_read,
        before_delay_device_dram_read_req_with_time_V_req_num_din,
        before_delay_device_dram_read_req_with_time_V_req_num_full_n,
        before_delay_device_dram_read_req_with_time_V_req_num_write,
        before_delay_device_dram_read_req_with_time_V_req_addr_din,
        before_delay_device_dram_read_req_with_time_V_req_addr_full_n,
        before_delay_device_dram_read_req_with_time_V_req_addr_write,
        before_delay_device_dram_read_req_with_time_V_time_din,
        before_delay_device_dram_read_req_with_time_V_time_full_n,
        before_delay_device_dram_read_req_with_time_V_time_write
);

parameter    ap_ST_fsm_state1 = 2'd1;
parameter    ap_ST_fsm_pp0_stage0 = 2'd2;

input   ap_clk;
input   ap_rst;
input   ap_start;
output   ap_done;
output   ap_idle;
output ap_ready; output ap_continue;
input  [7:0] before_delay_host_dram_read_req_V_num_dout;
input   before_delay_host_dram_read_req_V_num_empty_n;
output   before_delay_host_dram_read_req_V_num_read;
input  [63:0] before_delay_host_dram_read_req_V_addr_dout;
input   before_delay_host_dram_read_req_V_addr_empty_n;
output   before_delay_host_dram_read_req_V_addr_read;
output  [7:0] before_delay_host_dram_read_req_with_time_V_req_num_din;
input   before_delay_host_dram_read_req_with_time_V_req_num_full_n;
output   before_delay_host_dram_read_req_with_time_V_req_num_write;
output  [63:0] before_delay_host_dram_read_req_with_time_V_req_addr_din;
input   before_delay_host_dram_read_req_with_time_V_req_addr_full_n;
output   before_delay_host_dram_read_req_with_time_V_req_addr_write;
output  [63:0] before_delay_host_dram_read_req_with_time_V_time_din;
input   before_delay_host_dram_read_req_with_time_V_time_full_n;
output   before_delay_host_dram_read_req_with_time_V_time_write;
input  [7:0] before_delay_device_dram_read_req_V_num_dout;
input   before_delay_device_dram_read_req_V_num_empty_n;
output   before_delay_device_dram_read_req_V_num_read;
input  [63:0] before_delay_device_dram_read_req_V_addr_dout;
input   before_delay_device_dram_read_req_V_addr_empty_n;
output   before_delay_device_dram_read_req_V_addr_read;
output  [7:0] before_delay_device_dram_read_req_with_time_V_req_num_din;
input   before_delay_device_dram_read_req_with_time_V_req_num_full_n;
output   before_delay_device_dram_read_req_with_time_V_req_num_write;
output  [63:0] before_delay_device_dram_read_req_with_time_V_req_addr_din;
input   before_delay_device_dram_read_req_with_time_V_req_addr_full_n;
output   before_delay_device_dram_read_req_with_time_V_req_addr_write;
output  [63:0] before_delay_device_dram_read_req_with_time_V_time_din;
input   before_delay_device_dram_read_req_with_time_V_time_full_n;
output   before_delay_device_dram_read_req_with_time_V_time_write;

reg ap_idle;

(* fsm_encoding = "none" *) reg   [1:0] ap_CS_fsm;
wire    ap_CS_fsm_state1;
reg   [0:0] valid_host_read_req_reg_228;
reg   [0:0] valid_device_read_req_reg_240;
reg   [63:0] p_09_1_s_reg_252;
reg   [7:0] p_09_0_s_reg_263;
reg   [63:0] p_0_1_s_reg_274;
reg   [7:0] p_0_0_s_reg_285;
reg   [63:0] read_req_with_time_time_reg_296;
wire   [0:0] empty_n_4_fu_423_p1;
reg   [0:0] empty_n_4_reg_467;
wire    ap_CS_fsm_pp0_stage0;
wire    ap_block_state2_pp0_stage0_iter0;
wire    ap_block_state3_pp0_stage0_iter1;
wire    ap_block_pp0_stage0_11001;
reg   [0:0] ap_phi_mux_valid_host_read_req_phi_fu_232_p4;
wire   [0:0] empty_n_5_fu_435_p1;
reg   [0:0] empty_n_5_reg_484;
reg   [0:0] ap_phi_mux_valid_device_read_req_phi_fu_244_p4;
wire   [63:0] time_fu_447_p2;
reg    ap_enable_reg_pp0_iter1;
reg    ap_enable_reg_pp0_iter0;
wire    ap_block_pp0_stage0_subdone;
reg   [0:0] ap_phi_mux_valid_host_read_req_2_phi_fu_359_p4;
wire    ap_block_pp0_stage0;
reg   [0:0] ap_phi_mux_valid_device_read_req_2_phi_fu_416_p4;
reg   [63:0] ap_phi_mux_p_09_1_2_phi_fu_392_p4;
reg   [7:0] ap_phi_mux_p_09_0_2_phi_fu_404_p4;
reg   [63:0] ap_phi_mux_p_0_1_2_phi_fu_335_p4;
reg   [7:0] ap_phi_mux_p_0_0_2_phi_fu_347_p4;
reg   [63:0] ap_phi_mux_read_req_with_time_req_addr_phi_fu_312_p4;
reg   [63:0] ap_phi_reg_pp0_iter1_read_req_with_time_req_addr_reg_309;
wire   [63:0] ap_phi_reg_pp0_iter0_read_req_with_time_req_addr_reg_309;
reg   [7:0] ap_phi_mux_read_req_with_time_req_num_phi_fu_323_p4;
reg   [7:0] ap_phi_reg_pp0_iter1_read_req_with_time_req_num_reg_320;
wire   [7:0] ap_phi_reg_pp0_iter0_read_req_with_time_req_num_reg_320;
reg   [63:0] ap_phi_reg_pp0_iter1_p_0_1_2_reg_331;
wire   [63:0] ap_phi_reg_pp0_iter0_p_0_1_2_reg_331;
reg   [7:0] ap_phi_reg_pp0_iter1_p_0_0_2_reg_343;
wire   [7:0] ap_phi_reg_pp0_iter0_p_0_0_2_reg_343;
wire   [0:0] not_full_n_i_fu_453_p2;
reg   [0:0] ap_phi_reg_pp0_iter1_valid_host_read_req_2_reg_355;
wire   [0:0] ap_phi_reg_pp0_iter0_valid_host_read_req_2_reg_355;
reg   [63:0] ap_phi_mux_read_req_with_time_req_addr_1_phi_fu_369_p4;
reg   [63:0] ap_phi_reg_pp0_iter1_read_req_with_time_req_addr_1_reg_366;
wire   [63:0] ap_phi_reg_pp0_iter0_read_req_with_time_req_addr_1_reg_366;
reg   [7:0] ap_phi_mux_read_req_with_time_req_num_1_phi_fu_380_p4;
reg   [7:0] ap_phi_reg_pp0_iter1_read_req_with_time_req_num_1_reg_377;
wire   [7:0] ap_phi_reg_pp0_iter0_read_req_with_time_req_num_1_reg_377;
reg   [63:0] ap_phi_reg_pp0_iter1_p_09_1_2_reg_388;
wire   [63:0] ap_phi_reg_pp0_iter0_p_09_1_2_reg_388;
reg   [7:0] ap_phi_reg_pp0_iter1_p_09_0_2_reg_400;
wire   [7:0] ap_phi_reg_pp0_iter0_p_09_0_2_reg_400;
wire   [0:0] not_full_n_i5_fu_460_p2;
reg   [0:0] ap_phi_reg_pp0_iter1_valid_device_read_req_2_reg_412;
wire   [0:0] ap_phi_reg_pp0_iter0_valid_device_read_req_2_reg_412;
reg    before_delay_host_dram_read_req_V_num0_update;
wire   [0:0] empty_n_nbread_fu_186_p3_0;
reg    before_delay_device_dram_read_req_V_num0_update;
wire   [0:0] empty_n_1_nbread_fu_194_p3_0;
reg    before_delay_host_dram_read_req_with_time_V_time1_update;
wire   [0:0] full_n_nbwrite_fu_202_p7;
wire    ap_block_pp0_stage0_01001;
reg    before_delay_device_dram_read_req_with_time_V_time1_update;
wire   [0:0] full_n_1_nbwrite_fu_215_p7;
reg   [1:0] ap_NS_fsm;
reg    ap_idle_pp0;
wire    ap_enable_pp0;
reg    ap_condition_164;

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
    if ((1'b1 == ap_condition_164)) begin
        if (((ap_phi_mux_valid_device_read_req_phi_fu_244_p4 == 1'd0) & (empty_n_5_fu_435_p1 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_p_09_0_2_reg_400 <= before_delay_device_dram_read_req_V_num_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_p_09_0_2_reg_400 <= ap_phi_reg_pp0_iter0_p_09_0_2_reg_400;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_164)) begin
        if (((ap_phi_mux_valid_device_read_req_phi_fu_244_p4 == 1'd0) & (empty_n_5_fu_435_p1 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_p_09_1_2_reg_388 <= before_delay_device_dram_read_req_V_addr_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_p_09_1_2_reg_388 <= ap_phi_reg_pp0_iter0_p_09_1_2_reg_388;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_164)) begin
        if (((empty_n_4_fu_423_p1 == 1'd0) & (ap_phi_mux_valid_host_read_req_phi_fu_232_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_p_0_0_2_reg_343 <= before_delay_host_dram_read_req_V_num_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_p_0_0_2_reg_343 <= ap_phi_reg_pp0_iter0_p_0_0_2_reg_343;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_164)) begin
        if (((empty_n_4_fu_423_p1 == 1'd0) & (ap_phi_mux_valid_host_read_req_phi_fu_232_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_p_0_1_2_reg_331 <= before_delay_host_dram_read_req_V_addr_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_p_0_1_2_reg_331 <= ap_phi_reg_pp0_iter0_p_0_1_2_reg_331;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_164)) begin
        if (((empty_n_5_fu_435_p1 == 1'd1) & (ap_phi_mux_valid_device_read_req_phi_fu_244_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_read_req_with_time_req_addr_1_reg_366 <= before_delay_device_dram_read_req_V_addr_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_read_req_with_time_req_addr_1_reg_366 <= ap_phi_reg_pp0_iter0_read_req_with_time_req_addr_1_reg_366;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_164)) begin
        if (((empty_n_4_fu_423_p1 == 1'd1) & (ap_phi_mux_valid_host_read_req_phi_fu_232_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_read_req_with_time_req_addr_reg_309 <= before_delay_host_dram_read_req_V_addr_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_read_req_with_time_req_addr_reg_309 <= ap_phi_reg_pp0_iter0_read_req_with_time_req_addr_reg_309;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_164)) begin
        if (((empty_n_5_fu_435_p1 == 1'd1) & (ap_phi_mux_valid_device_read_req_phi_fu_244_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_read_req_with_time_req_num_1_reg_377 <= before_delay_device_dram_read_req_V_num_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_read_req_with_time_req_num_1_reg_377 <= ap_phi_reg_pp0_iter0_read_req_with_time_req_num_1_reg_377;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_164)) begin
        if (((empty_n_4_fu_423_p1 == 1'd1) & (ap_phi_mux_valid_host_read_req_phi_fu_232_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_read_req_with_time_req_num_reg_320 <= before_delay_host_dram_read_req_V_num_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_read_req_with_time_req_num_reg_320 <= ap_phi_reg_pp0_iter0_read_req_with_time_req_num_reg_320;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_164)) begin
        if (((ap_phi_mux_valid_device_read_req_phi_fu_244_p4 == 1'd0) & (empty_n_5_fu_435_p1 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_valid_device_read_req_2_reg_412 <= empty_n_1_nbread_fu_194_p3_0;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_valid_device_read_req_2_reg_412 <= ap_phi_reg_pp0_iter0_valid_device_read_req_2_reg_412;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_164)) begin
        if (((empty_n_4_fu_423_p1 == 1'd0) & (ap_phi_mux_valid_host_read_req_phi_fu_232_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_valid_host_read_req_2_reg_355 <= empty_n_nbread_fu_186_p3_0;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_valid_host_read_req_2_reg_355 <= ap_phi_reg_pp0_iter0_valid_host_read_req_2_reg_355;
        end
    end
end

always @ (posedge ap_clk) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        read_req_with_time_time_reg_296 <= time_fu_447_p2;
    end else if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        read_req_with_time_time_reg_296 <= 64'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        valid_device_read_req_reg_240 <= ap_phi_mux_valid_device_read_req_2_phi_fu_416_p4;
    end else if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        valid_device_read_req_reg_240 <= 1'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        valid_host_read_req_reg_228 <= ap_phi_mux_valid_host_read_req_2_phi_fu_359_p4;
    end else if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        valid_host_read_req_reg_228 <= 1'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_phi_mux_valid_host_read_req_phi_fu_232_p4 == 1'd0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        empty_n_4_reg_467 <= empty_n_nbread_fu_186_p3_0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_phi_mux_valid_device_read_req_phi_fu_244_p4 == 1'd0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        empty_n_5_reg_484 <= empty_n_1_nbread_fu_194_p3_0;
    end
end

always @ (posedge ap_clk) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        p_09_0_s_reg_263 <= ap_phi_mux_p_09_0_2_phi_fu_404_p4;
        p_09_1_s_reg_252 <= ap_phi_mux_p_09_1_2_phi_fu_392_p4;
        p_0_0_s_reg_285 <= ap_phi_mux_p_0_0_2_phi_fu_347_p4;
        p_0_1_s_reg_274 <= ap_phi_mux_p_0_1_2_phi_fu_335_p4;
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
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0) & ((valid_device_read_req_reg_240 == 1'd1) | (empty_n_5_reg_484 == 1'd1)))) begin
        ap_phi_mux_p_09_0_2_phi_fu_404_p4 = ap_phi_mux_read_req_with_time_req_num_1_phi_fu_380_p4;
    end else begin
        ap_phi_mux_p_09_0_2_phi_fu_404_p4 = ap_phi_reg_pp0_iter1_p_09_0_2_reg_400;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0) & ((valid_device_read_req_reg_240 == 1'd1) | (empty_n_5_reg_484 == 1'd1)))) begin
        ap_phi_mux_p_09_1_2_phi_fu_392_p4 = ap_phi_mux_read_req_with_time_req_addr_1_phi_fu_369_p4;
    end else begin
        ap_phi_mux_p_09_1_2_phi_fu_392_p4 = ap_phi_reg_pp0_iter1_p_09_1_2_reg_388;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0) & ((empty_n_4_reg_467 == 1'd1) | (valid_host_read_req_reg_228 == 1'd1)))) begin
        ap_phi_mux_p_0_0_2_phi_fu_347_p4 = ap_phi_mux_read_req_with_time_req_num_phi_fu_323_p4;
    end else begin
        ap_phi_mux_p_0_0_2_phi_fu_347_p4 = ap_phi_reg_pp0_iter1_p_0_0_2_reg_343;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0) & ((empty_n_4_reg_467 == 1'd1) | (valid_host_read_req_reg_228 == 1'd1)))) begin
        ap_phi_mux_p_0_1_2_phi_fu_335_p4 = ap_phi_mux_read_req_with_time_req_addr_phi_fu_312_p4;
    end else begin
        ap_phi_mux_p_0_1_2_phi_fu_335_p4 = ap_phi_reg_pp0_iter1_p_0_1_2_reg_331;
    end
end

always @ (*) begin
    if (((valid_device_read_req_reg_240 == 1'd1) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_read_req_with_time_req_addr_1_phi_fu_369_p4 = p_09_1_s_reg_252;
    end else begin
        ap_phi_mux_read_req_with_time_req_addr_1_phi_fu_369_p4 = ap_phi_reg_pp0_iter1_read_req_with_time_req_addr_1_reg_366;
    end
end

always @ (*) begin
    if (((valid_host_read_req_reg_228 == 1'd1) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_read_req_with_time_req_addr_phi_fu_312_p4 = p_0_1_s_reg_274;
    end else begin
        ap_phi_mux_read_req_with_time_req_addr_phi_fu_312_p4 = ap_phi_reg_pp0_iter1_read_req_with_time_req_addr_reg_309;
    end
end

always @ (*) begin
    if (((valid_device_read_req_reg_240 == 1'd1) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_read_req_with_time_req_num_1_phi_fu_380_p4 = p_09_0_s_reg_263;
    end else begin
        ap_phi_mux_read_req_with_time_req_num_1_phi_fu_380_p4 = ap_phi_reg_pp0_iter1_read_req_with_time_req_num_1_reg_377;
    end
end

always @ (*) begin
    if (((valid_host_read_req_reg_228 == 1'd1) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_read_req_with_time_req_num_phi_fu_323_p4 = p_0_0_s_reg_285;
    end else begin
        ap_phi_mux_read_req_with_time_req_num_phi_fu_323_p4 = ap_phi_reg_pp0_iter1_read_req_with_time_req_num_reg_320;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0) & ((valid_device_read_req_reg_240 == 1'd1) | (empty_n_5_reg_484 == 1'd1)))) begin
        ap_phi_mux_valid_device_read_req_2_phi_fu_416_p4 = not_full_n_i5_fu_460_p2;
    end else begin
        ap_phi_mux_valid_device_read_req_2_phi_fu_416_p4 = ap_phi_reg_pp0_iter1_valid_device_read_req_2_reg_412;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_valid_device_read_req_phi_fu_244_p4 = ap_phi_mux_valid_device_read_req_2_phi_fu_416_p4;
    end else begin
        ap_phi_mux_valid_device_read_req_phi_fu_244_p4 = valid_device_read_req_reg_240;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0) & ((empty_n_4_reg_467 == 1'd1) | (valid_host_read_req_reg_228 == 1'd1)))) begin
        ap_phi_mux_valid_host_read_req_2_phi_fu_359_p4 = not_full_n_i_fu_453_p2;
    end else begin
        ap_phi_mux_valid_host_read_req_2_phi_fu_359_p4 = ap_phi_reg_pp0_iter1_valid_host_read_req_2_reg_355;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_valid_host_read_req_phi_fu_232_p4 = ap_phi_mux_valid_host_read_req_2_phi_fu_359_p4;
    end else begin
        ap_phi_mux_valid_host_read_req_phi_fu_232_p4 = valid_host_read_req_reg_228;
    end
end

always @ (*) begin
    if ((((before_delay_device_dram_read_req_V_num_empty_n & before_delay_device_dram_read_req_V_addr_empty_n) == 1'b1) & (ap_enable_reg_pp0_iter0 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_phi_mux_valid_device_read_req_phi_fu_244_p4 == 1'd0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        before_delay_device_dram_read_req_V_num0_update = 1'b1;
    end else begin
        before_delay_device_dram_read_req_V_num0_update = 1'b0;
    end
end

always @ (*) begin
    if ((((before_delay_device_dram_read_req_with_time_V_time_full_n & before_delay_device_dram_read_req_with_time_V_req_num_full_n & before_delay_device_dram_read_req_with_time_V_req_addr_full_n) == 1'b1) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0_11001) & ((valid_device_read_req_reg_240 == 1'd1) | (empty_n_5_reg_484 == 1'd1)))) begin
        before_delay_device_dram_read_req_with_time_V_time1_update = 1'b1;
    end else begin
        before_delay_device_dram_read_req_with_time_V_time1_update = 1'b0;
    end
end

always @ (*) begin
    if ((((before_delay_host_dram_read_req_V_num_empty_n & before_delay_host_dram_read_req_V_addr_empty_n) == 1'b1) & (ap_enable_reg_pp0_iter0 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_phi_mux_valid_host_read_req_phi_fu_232_p4 == 1'd0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        before_delay_host_dram_read_req_V_num0_update = 1'b1;
    end else begin
        before_delay_host_dram_read_req_V_num0_update = 1'b0;
    end
end

always @ (*) begin
    if ((((before_delay_host_dram_read_req_with_time_V_time_full_n & before_delay_host_dram_read_req_with_time_V_req_num_full_n & before_delay_host_dram_read_req_with_time_V_req_addr_full_n) == 1'b1) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0_11001) & ((empty_n_4_reg_467 == 1'd1) | (valid_host_read_req_reg_228 == 1'd1)))) begin
        before_delay_host_dram_read_req_with_time_V_time1_update = 1'b1;
    end else begin
        before_delay_host_dram_read_req_with_time_V_time1_update = 1'b0;
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

assign ap_block_pp0_stage0_01001 = ~(1'b1 == 1'b1);

assign ap_block_pp0_stage0_11001 = ~(1'b1 == 1'b1);

assign ap_block_pp0_stage0_subdone = ~(1'b1 == 1'b1);

assign ap_block_state2_pp0_stage0_iter0 = ~(1'b1 == 1'b1);

assign ap_block_state3_pp0_stage0_iter1 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_condition_164 = ((ap_enable_reg_pp0_iter0 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b0 == ap_block_pp0_stage0_11001));
end

assign ap_done = 1'b0;

assign ap_enable_pp0 = (ap_idle_pp0 ^ 1'b1);

assign ap_phi_reg_pp0_iter0_p_09_0_2_reg_400 = 'bx;

assign ap_phi_reg_pp0_iter0_p_09_1_2_reg_388 = 'bx;

assign ap_phi_reg_pp0_iter0_p_0_0_2_reg_343 = 'bx;

assign ap_phi_reg_pp0_iter0_p_0_1_2_reg_331 = 'bx;

assign ap_phi_reg_pp0_iter0_read_req_with_time_req_addr_1_reg_366 = 'bx;

assign ap_phi_reg_pp0_iter0_read_req_with_time_req_addr_reg_309 = 'bx;

assign ap_phi_reg_pp0_iter0_read_req_with_time_req_num_1_reg_377 = 'bx;

assign ap_phi_reg_pp0_iter0_read_req_with_time_req_num_reg_320 = 'bx;

assign ap_phi_reg_pp0_iter0_valid_device_read_req_2_reg_412 = 'bx;

assign ap_phi_reg_pp0_iter0_valid_host_read_req_2_reg_355 = 'bx;

assign ap_ready = 1'b0;

assign before_delay_device_dram_read_req_V_addr_read = before_delay_device_dram_read_req_V_num0_update;

assign before_delay_device_dram_read_req_V_num_read = before_delay_device_dram_read_req_V_num0_update;

assign before_delay_device_dram_read_req_with_time_V_req_addr_din = ap_phi_mux_read_req_with_time_req_addr_1_phi_fu_369_p4;

assign before_delay_device_dram_read_req_with_time_V_req_addr_write = before_delay_device_dram_read_req_with_time_V_time1_update;

assign before_delay_device_dram_read_req_with_time_V_req_num_din = ap_phi_mux_read_req_with_time_req_num_1_phi_fu_380_p4;

assign before_delay_device_dram_read_req_with_time_V_req_num_write = before_delay_device_dram_read_req_with_time_V_time1_update;

assign before_delay_device_dram_read_req_with_time_V_time_din = read_req_with_time_time_reg_296;

assign before_delay_device_dram_read_req_with_time_V_time_write = before_delay_device_dram_read_req_with_time_V_time1_update;

assign before_delay_host_dram_read_req_V_addr_read = before_delay_host_dram_read_req_V_num0_update;

assign before_delay_host_dram_read_req_V_num_read = before_delay_host_dram_read_req_V_num0_update;

assign before_delay_host_dram_read_req_with_time_V_req_addr_din = ap_phi_mux_read_req_with_time_req_addr_phi_fu_312_p4;

assign before_delay_host_dram_read_req_with_time_V_req_addr_write = before_delay_host_dram_read_req_with_time_V_time1_update;

assign before_delay_host_dram_read_req_with_time_V_req_num_din = ap_phi_mux_read_req_with_time_req_num_phi_fu_323_p4;

assign before_delay_host_dram_read_req_with_time_V_req_num_write = before_delay_host_dram_read_req_with_time_V_time1_update;

assign before_delay_host_dram_read_req_with_time_V_time_din = read_req_with_time_time_reg_296;

assign before_delay_host_dram_read_req_with_time_V_time_write = before_delay_host_dram_read_req_with_time_V_time1_update;

assign empty_n_1_nbread_fu_194_p3_0 = (before_delay_device_dram_read_req_V_num_empty_n & before_delay_device_dram_read_req_V_addr_empty_n);

assign empty_n_4_fu_423_p1 = empty_n_nbread_fu_186_p3_0;

assign empty_n_5_fu_435_p1 = empty_n_1_nbread_fu_194_p3_0;

assign empty_n_nbread_fu_186_p3_0 = (before_delay_host_dram_read_req_V_num_empty_n & before_delay_host_dram_read_req_V_addr_empty_n);

assign full_n_1_nbwrite_fu_215_p7 = (before_delay_device_dram_read_req_with_time_V_time_full_n & before_delay_device_dram_read_req_with_time_V_req_num_full_n & before_delay_device_dram_read_req_with_time_V_req_addr_full_n);

assign full_n_nbwrite_fu_202_p7 = (before_delay_host_dram_read_req_with_time_V_time_full_n & before_delay_host_dram_read_req_with_time_V_req_num_full_n & before_delay_host_dram_read_req_with_time_V_req_addr_full_n);

assign not_full_n_i5_fu_460_p2 = (full_n_1_nbwrite_fu_215_p7 ^ 1'd1);

assign not_full_n_i_fu_453_p2 = (full_n_nbwrite_fu_202_p7 ^ 1'd1);

assign time_fu_447_p2 = (read_req_with_time_time_reg_296 + 64'd1);

endmodule //dram_read_req_time_marker
