// ==============================================================
// RTL generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2017.4.op
// Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="dram_write_req_time_marker,hls_ip_2017_4_op,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xcvu9p-flgb2104-2-i,HLS_INPUT_CLOCK=4.000000,HLS_INPUT_ARCH=others,HLS_SYN_CLOCK=2.997000,HLS_SYN_LAT=-1,HLS_SYN_TPT=none,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=1830,HLS_SYN_LUT=419}" *)

module dram_write_req_time_marker (
        ap_clk,
        ap_rst,
        ap_start,
        ap_done,
ap_continue,
        ap_idle,
        ap_ready,
        host_dram_write_req_data_V_last_dout,
        host_dram_write_req_data_V_last_empty_n,
        host_dram_write_req_data_V_last_read,
        host_dram_write_req_data_V_data_V_dout,
        host_dram_write_req_data_V_data_V_empty_n,
        host_dram_write_req_data_V_data_V_read,
        host_dram_write_req_apply_V_num_dout,
        host_dram_write_req_apply_V_num_empty_n,
        host_dram_write_req_apply_V_num_read,
        host_dram_write_req_apply_V_addr_dout,
        host_dram_write_req_apply_V_addr_empty_n,
        host_dram_write_req_apply_V_addr_read,
        before_delay_dram_write_req_data_V_last_din,
        before_delay_dram_write_req_data_V_last_full_n,
        before_delay_dram_write_req_data_V_last_write,
        before_delay_dram_write_req_data_V_data_V_din,
        before_delay_dram_write_req_data_V_data_V_full_n,
        before_delay_dram_write_req_data_V_data_V_write,
        before_delay_dram_write_req_apply_with_time_V_req_apply_num_din,
        before_delay_dram_write_req_apply_with_time_V_req_apply_num_full_n,
        before_delay_dram_write_req_apply_with_time_V_req_apply_num_write,
        before_delay_dram_write_req_apply_with_time_V_req_apply_addr_din,
        before_delay_dram_write_req_apply_with_time_V_req_apply_addr_full_n,
        before_delay_dram_write_req_apply_with_time_V_req_apply_addr_write,
        before_delay_dram_write_req_apply_with_time_V_time_din,
        before_delay_dram_write_req_apply_with_time_V_time_full_n,
        before_delay_dram_write_req_apply_with_time_V_time_write
);

parameter    ap_ST_fsm_state1 = 2'd1;
parameter    ap_ST_fsm_pp0_stage0 = 2'd2;

input   ap_clk;
input   ap_rst;
input   ap_start;
output   ap_done;
output   ap_idle;
output ap_ready; output ap_continue;
input   host_dram_write_req_data_V_last_dout;
input   host_dram_write_req_data_V_last_empty_n;
output   host_dram_write_req_data_V_last_read;
input  [511:0] host_dram_write_req_data_V_data_V_dout;
input   host_dram_write_req_data_V_data_V_empty_n;
output   host_dram_write_req_data_V_data_V_read;
input  [7:0] host_dram_write_req_apply_V_num_dout;
input   host_dram_write_req_apply_V_num_empty_n;
output   host_dram_write_req_apply_V_num_read;
input  [63:0] host_dram_write_req_apply_V_addr_dout;
input   host_dram_write_req_apply_V_addr_empty_n;
output   host_dram_write_req_apply_V_addr_read;
output   before_delay_dram_write_req_data_V_last_din;
input   before_delay_dram_write_req_data_V_last_full_n;
output   before_delay_dram_write_req_data_V_last_write;
output  [511:0] before_delay_dram_write_req_data_V_data_V_din;
input   before_delay_dram_write_req_data_V_data_V_full_n;
output   before_delay_dram_write_req_data_V_data_V_write;
output  [7:0] before_delay_dram_write_req_apply_with_time_V_req_apply_num_din;
input   before_delay_dram_write_req_apply_with_time_V_req_apply_num_full_n;
output   before_delay_dram_write_req_apply_with_time_V_req_apply_num_write;
output  [63:0] before_delay_dram_write_req_apply_with_time_V_req_apply_addr_din;
input   before_delay_dram_write_req_apply_with_time_V_req_apply_addr_full_n;
output   before_delay_dram_write_req_apply_with_time_V_req_apply_addr_write;
output  [63:0] before_delay_dram_write_req_apply_with_time_V_time_din;
input   before_delay_dram_write_req_apply_with_time_V_time_full_n;
output   before_delay_dram_write_req_apply_with_time_V_time_write;

reg ap_idle;

(* fsm_encoding = "none" *) reg   [1:0] ap_CS_fsm;
wire    ap_CS_fsm_state1;
reg   [0:0] valid_write_req_apply_reg_219;
reg   [0:0] valid_write_req_data_reg_231;
reg   [511:0] write_req_data_1_s_reg_243;
reg   [0:0] write_req_data_0_s_reg_254;
reg   [63:0] p_0_1_s_reg_265;
reg   [7:0] p_0_0_s_reg_276;
reg   [63:0] write_req_apply_with_time_time_reg_287;
wire   [0:0] empty_n_4_fu_423_p1;
reg   [0:0] empty_n_4_reg_473;
wire    ap_CS_fsm_pp0_stage0;
wire    ap_block_state2_pp0_stage0_iter0;
wire    ap_block_state3_pp0_stage0_iter1;
wire    ap_block_pp0_stage0_11001;
reg   [0:0] ap_phi_mux_valid_write_req_apply_phi_fu_223_p4;
wire   [0:0] empty_n_5_fu_435_p1;
reg   [0:0] empty_n_5_reg_490;
reg   [0:0] ap_phi_mux_valid_write_req_data_phi_fu_235_p4;
wire   [63:0] time_fu_447_p2;
reg    ap_enable_reg_pp0_iter1;
reg    ap_enable_reg_pp0_iter0;
wire    ap_block_pp0_stage0_subdone;
reg   [0:0] ap_phi_mux_valid_write_req_apply_2_phi_fu_349_p4;
wire    ap_block_pp0_stage0;
reg   [0:0] ap_phi_mux_valid_write_req_data_3_phi_fu_416_p4;
reg   [511:0] ap_phi_mux_write_req_data_1_2_phi_fu_392_p4;
reg   [0:0] ap_phi_mux_write_req_data_0_2_phi_fu_404_p4;
reg   [63:0] ap_phi_mux_p_0_1_2_phi_fu_325_p4;
reg   [7:0] ap_phi_mux_p_0_0_2_phi_fu_337_p4;
reg   [63:0] ap_phi_mux_write_req_apply_with_time_req_apply_addr_phi_fu_302_p4;
reg   [63:0] ap_phi_reg_pp0_iter1_write_req_apply_with_time_req_apply_addr_reg_299;
wire   [63:0] ap_phi_reg_pp0_iter0_write_req_apply_with_time_req_apply_addr_reg_299;
reg   [7:0] ap_phi_mux_write_req_apply_with_time_req_apply_num_phi_fu_313_p4;
reg   [7:0] ap_phi_reg_pp0_iter1_write_req_apply_with_time_req_apply_num_reg_310;
wire   [7:0] ap_phi_reg_pp0_iter0_write_req_apply_with_time_req_apply_num_reg_310;
reg   [63:0] ap_phi_reg_pp0_iter1_p_0_1_2_reg_321;
wire   [63:0] ap_phi_reg_pp0_iter0_p_0_1_2_reg_321;
reg   [7:0] ap_phi_reg_pp0_iter1_p_0_0_2_reg_333;
wire   [7:0] ap_phi_reg_pp0_iter0_p_0_0_2_reg_333;
wire   [0:0] not_full_n_i_fu_453_p2;
reg   [0:0] ap_phi_reg_pp0_iter1_valid_write_req_apply_2_reg_345;
wire   [0:0] ap_phi_reg_pp0_iter0_valid_write_req_apply_2_reg_345;
reg   [511:0] ap_phi_mux_tmp_data_V_phi_fu_359_p4;
reg   [511:0] ap_phi_reg_pp0_iter1_tmp_data_V_reg_356;
wire   [511:0] ap_phi_reg_pp0_iter0_tmp_data_V_reg_356;
reg   [0:0] ap_phi_mux_tmp_last_phi_fu_370_p4;
reg   [0:0] ap_phi_reg_pp0_iter1_tmp_last_reg_367;
wire   [0:0] ap_phi_reg_pp0_iter0_tmp_last_reg_367;
reg   [0:0] ap_phi_mux_valid_write_req_data_1_phi_fu_381_p4;
reg   [0:0] ap_phi_reg_pp0_iter1_valid_write_req_data_1_reg_378;
wire   [0:0] ap_phi_reg_pp0_iter0_valid_write_req_data_1_reg_378;
reg   [511:0] ap_phi_reg_pp0_iter1_write_req_data_1_2_reg_388;
wire   [511:0] ap_phi_reg_pp0_iter0_write_req_data_1_2_reg_388;
reg   [0:0] ap_phi_reg_pp0_iter1_write_req_data_0_2_reg_400;
wire   [0:0] ap_phi_reg_pp0_iter0_write_req_data_0_2_reg_400;
wire   [0:0] p_valid_write_req_data_1_fu_466_p2;
reg   [0:0] ap_phi_reg_pp0_iter1_valid_write_req_data_3_reg_412;
wire   [0:0] ap_phi_reg_pp0_iter0_valid_write_req_data_3_reg_412;
reg    host_dram_write_req_apply_V_num0_update;
wire   [0:0] empty_n_nbread_fu_180_p3_0;
reg    host_dram_write_req_data_V_last0_update;
wire   [0:0] empty_n_1_nbread_fu_188_p3_0;
reg    before_delay_dram_write_req_apply_with_time_V_time1_update;
wire   [0:0] full_n_nbwrite_fu_196_p7;
wire    ap_block_pp0_stage0_01001;
reg    before_delay_dram_write_req_data_V_last1_update;
wire   [0:0] full_n_1_nbwrite_fu_209_p5;
wire   [0:0] not_full_n_i5_fu_460_p2;
reg   [1:0] ap_NS_fsm;
reg    ap_idle_pp0;
wire    ap_enable_pp0;
reg    ap_condition_162;

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
    if ((1'b1 == ap_condition_162)) begin
        if (((empty_n_4_fu_423_p1 == 1'd0) & (ap_phi_mux_valid_write_req_apply_phi_fu_223_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_p_0_0_2_reg_333 <= host_dram_write_req_apply_V_num_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_p_0_0_2_reg_333 <= ap_phi_reg_pp0_iter0_p_0_0_2_reg_333;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_162)) begin
        if (((empty_n_4_fu_423_p1 == 1'd0) & (ap_phi_mux_valid_write_req_apply_phi_fu_223_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_p_0_1_2_reg_321 <= host_dram_write_req_apply_V_addr_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_p_0_1_2_reg_321 <= ap_phi_reg_pp0_iter0_p_0_1_2_reg_321;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_162)) begin
        if (((empty_n_5_fu_435_p1 == 1'd1) & (ap_phi_mux_valid_write_req_data_phi_fu_235_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_tmp_data_V_reg_356 <= host_dram_write_req_data_V_data_V_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_tmp_data_V_reg_356 <= ap_phi_reg_pp0_iter0_tmp_data_V_reg_356;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_162)) begin
        if (((empty_n_5_fu_435_p1 == 1'd1) & (ap_phi_mux_valid_write_req_data_phi_fu_235_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_tmp_last_reg_367 <= host_dram_write_req_data_V_last_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_tmp_last_reg_367 <= ap_phi_reg_pp0_iter0_tmp_last_reg_367;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_162)) begin
        if (((empty_n_4_fu_423_p1 == 1'd0) & (ap_phi_mux_valid_write_req_apply_phi_fu_223_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_valid_write_req_apply_2_reg_345 <= empty_n_nbread_fu_180_p3_0;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_valid_write_req_apply_2_reg_345 <= ap_phi_reg_pp0_iter0_valid_write_req_apply_2_reg_345;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_162)) begin
        if (((empty_n_5_fu_435_p1 == 1'd1) & (ap_phi_mux_valid_write_req_data_phi_fu_235_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_valid_write_req_data_1_reg_378 <= empty_n_1_nbread_fu_188_p3_0;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_valid_write_req_data_1_reg_378 <= ap_phi_reg_pp0_iter0_valid_write_req_data_1_reg_378;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_162)) begin
        if (((ap_phi_mux_valid_write_req_data_phi_fu_235_p4 == 1'd0) & (empty_n_5_fu_435_p1 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_valid_write_req_data_3_reg_412 <= empty_n_1_nbread_fu_188_p3_0;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_valid_write_req_data_3_reg_412 <= ap_phi_reg_pp0_iter0_valid_write_req_data_3_reg_412;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_162)) begin
        if (((empty_n_4_fu_423_p1 == 1'd1) & (ap_phi_mux_valid_write_req_apply_phi_fu_223_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_write_req_apply_with_time_req_apply_addr_reg_299 <= host_dram_write_req_apply_V_addr_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_write_req_apply_with_time_req_apply_addr_reg_299 <= ap_phi_reg_pp0_iter0_write_req_apply_with_time_req_apply_addr_reg_299;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_162)) begin
        if (((empty_n_4_fu_423_p1 == 1'd1) & (ap_phi_mux_valid_write_req_apply_phi_fu_223_p4 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_write_req_apply_with_time_req_apply_num_reg_310 <= host_dram_write_req_apply_V_num_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_write_req_apply_with_time_req_apply_num_reg_310 <= ap_phi_reg_pp0_iter0_write_req_apply_with_time_req_apply_num_reg_310;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_162)) begin
        if (((ap_phi_mux_valid_write_req_data_phi_fu_235_p4 == 1'd0) & (empty_n_5_fu_435_p1 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_write_req_data_0_2_reg_400 <= host_dram_write_req_data_V_last_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_write_req_data_0_2_reg_400 <= ap_phi_reg_pp0_iter0_write_req_data_0_2_reg_400;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_162)) begin
        if (((ap_phi_mux_valid_write_req_data_phi_fu_235_p4 == 1'd0) & (empty_n_5_fu_435_p1 == 1'd0))) begin
            ap_phi_reg_pp0_iter1_write_req_data_1_2_reg_388 <= host_dram_write_req_data_V_data_V_dout;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_write_req_data_1_2_reg_388 <= ap_phi_reg_pp0_iter0_write_req_data_1_2_reg_388;
        end
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        valid_write_req_apply_reg_219 <= ap_phi_mux_valid_write_req_apply_2_phi_fu_349_p4;
    end else if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        valid_write_req_apply_reg_219 <= 1'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        valid_write_req_data_reg_231 <= ap_phi_mux_valid_write_req_data_3_phi_fu_416_p4;
    end else if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        valid_write_req_data_reg_231 <= 1'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        write_req_apply_with_time_time_reg_287 <= time_fu_447_p2;
    end else if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        write_req_apply_with_time_time_reg_287 <= 64'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_phi_mux_valid_write_req_apply_phi_fu_223_p4 == 1'd0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        empty_n_4_reg_473 <= empty_n_nbread_fu_180_p3_0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_phi_mux_valid_write_req_data_phi_fu_235_p4 == 1'd0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        empty_n_5_reg_490 <= empty_n_1_nbread_fu_188_p3_0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        p_0_0_s_reg_276 <= ap_phi_mux_p_0_0_2_phi_fu_337_p4;
        p_0_1_s_reg_265 <= ap_phi_mux_p_0_1_2_phi_fu_325_p4;
        write_req_data_0_s_reg_254 <= ap_phi_mux_write_req_data_0_2_phi_fu_404_p4;
        write_req_data_1_s_reg_243 <= ap_phi_mux_write_req_data_1_2_phi_fu_392_p4;
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
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0) & ((empty_n_4_reg_473 == 1'd1) | (valid_write_req_apply_reg_219 == 1'd1)))) begin
        ap_phi_mux_p_0_0_2_phi_fu_337_p4 = ap_phi_mux_write_req_apply_with_time_req_apply_num_phi_fu_313_p4;
    end else begin
        ap_phi_mux_p_0_0_2_phi_fu_337_p4 = ap_phi_reg_pp0_iter1_p_0_0_2_reg_333;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0) & ((empty_n_4_reg_473 == 1'd1) | (valid_write_req_apply_reg_219 == 1'd1)))) begin
        ap_phi_mux_p_0_1_2_phi_fu_325_p4 = ap_phi_mux_write_req_apply_with_time_req_apply_addr_phi_fu_302_p4;
    end else begin
        ap_phi_mux_p_0_1_2_phi_fu_325_p4 = ap_phi_reg_pp0_iter1_p_0_1_2_reg_321;
    end
end

always @ (*) begin
    if (((valid_write_req_data_reg_231 == 1'd1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_tmp_data_V_phi_fu_359_p4 = write_req_data_1_s_reg_243;
    end else begin
        ap_phi_mux_tmp_data_V_phi_fu_359_p4 = ap_phi_reg_pp0_iter1_tmp_data_V_reg_356;
    end
end

always @ (*) begin
    if (((valid_write_req_data_reg_231 == 1'd1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_tmp_last_phi_fu_370_p4 = write_req_data_0_s_reg_254;
    end else begin
        ap_phi_mux_tmp_last_phi_fu_370_p4 = ap_phi_reg_pp0_iter1_tmp_last_reg_367;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0) & ((empty_n_4_reg_473 == 1'd1) | (valid_write_req_apply_reg_219 == 1'd1)))) begin
        ap_phi_mux_valid_write_req_apply_2_phi_fu_349_p4 = not_full_n_i_fu_453_p2;
    end else begin
        ap_phi_mux_valid_write_req_apply_2_phi_fu_349_p4 = ap_phi_reg_pp0_iter1_valid_write_req_apply_2_reg_345;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_valid_write_req_apply_phi_fu_223_p4 = ap_phi_mux_valid_write_req_apply_2_phi_fu_349_p4;
    end else begin
        ap_phi_mux_valid_write_req_apply_phi_fu_223_p4 = valid_write_req_apply_reg_219;
    end
end

always @ (*) begin
    if (((valid_write_req_data_reg_231 == 1'd1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_valid_write_req_data_1_phi_fu_381_p4 = valid_write_req_data_reg_231;
    end else begin
        ap_phi_mux_valid_write_req_data_1_phi_fu_381_p4 = ap_phi_reg_pp0_iter1_valid_write_req_data_1_reg_378;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0) & ((valid_write_req_data_reg_231 == 1'd1) | (empty_n_5_reg_490 == 1'd1)))) begin
        ap_phi_mux_valid_write_req_data_3_phi_fu_416_p4 = p_valid_write_req_data_1_fu_466_p2;
    end else begin
        ap_phi_mux_valid_write_req_data_3_phi_fu_416_p4 = ap_phi_reg_pp0_iter1_valid_write_req_data_3_reg_412;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_valid_write_req_data_phi_fu_235_p4 = ap_phi_mux_valid_write_req_data_3_phi_fu_416_p4;
    end else begin
        ap_phi_mux_valid_write_req_data_phi_fu_235_p4 = valid_write_req_data_reg_231;
    end
end

always @ (*) begin
    if (((valid_write_req_apply_reg_219 == 1'd1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_write_req_apply_with_time_req_apply_addr_phi_fu_302_p4 = p_0_1_s_reg_265;
    end else begin
        ap_phi_mux_write_req_apply_with_time_req_apply_addr_phi_fu_302_p4 = ap_phi_reg_pp0_iter1_write_req_apply_with_time_req_apply_addr_reg_299;
    end
end

always @ (*) begin
    if (((valid_write_req_apply_reg_219 == 1'd1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0))) begin
        ap_phi_mux_write_req_apply_with_time_req_apply_num_phi_fu_313_p4 = p_0_0_s_reg_276;
    end else begin
        ap_phi_mux_write_req_apply_with_time_req_apply_num_phi_fu_313_p4 = ap_phi_reg_pp0_iter1_write_req_apply_with_time_req_apply_num_reg_310;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0) & ((valid_write_req_data_reg_231 == 1'd1) | (empty_n_5_reg_490 == 1'd1)))) begin
        ap_phi_mux_write_req_data_0_2_phi_fu_404_p4 = ap_phi_mux_tmp_last_phi_fu_370_p4;
    end else begin
        ap_phi_mux_write_req_data_0_2_phi_fu_404_p4 = ap_phi_reg_pp0_iter1_write_req_data_0_2_reg_400;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0) & ((valid_write_req_data_reg_231 == 1'd1) | (empty_n_5_reg_490 == 1'd1)))) begin
        ap_phi_mux_write_req_data_1_2_phi_fu_392_p4 = ap_phi_mux_tmp_data_V_phi_fu_359_p4;
    end else begin
        ap_phi_mux_write_req_data_1_2_phi_fu_392_p4 = ap_phi_reg_pp0_iter1_write_req_data_1_2_reg_388;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & ((before_delay_dram_write_req_apply_with_time_V_time_full_n & before_delay_dram_write_req_apply_with_time_V_req_apply_num_full_n & before_delay_dram_write_req_apply_with_time_V_req_apply_addr_full_n) == 1'b1) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001) & ((empty_n_4_reg_473 == 1'd1) | (valid_write_req_apply_reg_219 == 1'd1)))) begin
        before_delay_dram_write_req_apply_with_time_V_time1_update = 1'b1;
    end else begin
        before_delay_dram_write_req_apply_with_time_V_time1_update = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & ((before_delay_dram_write_req_data_V_last_full_n & before_delay_dram_write_req_data_V_data_V_full_n) == 1'b1) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001) & ((valid_write_req_data_reg_231 == 1'd1) | (empty_n_5_reg_490 == 1'd1)))) begin
        before_delay_dram_write_req_data_V_last1_update = 1'b1;
    end else begin
        before_delay_dram_write_req_data_V_last1_update = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & ((host_dram_write_req_apply_V_num_empty_n & host_dram_write_req_apply_V_addr_empty_n) == 1'b1) & (ap_enable_reg_pp0_iter0 == 1'b1) & (ap_phi_mux_valid_write_req_apply_phi_fu_223_p4 == 1'd0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        host_dram_write_req_apply_V_num0_update = 1'b1;
    end else begin
        host_dram_write_req_apply_V_num0_update = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & ((host_dram_write_req_data_V_last_empty_n & host_dram_write_req_data_V_data_V_empty_n) == 1'b1) & (ap_enable_reg_pp0_iter0 == 1'b1) & (ap_phi_mux_valid_write_req_data_phi_fu_235_p4 == 1'd0) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        host_dram_write_req_data_V_last0_update = 1'b1;
    end else begin
        host_dram_write_req_data_V_last0_update = 1'b0;
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
    ap_condition_162 = ((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter0 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001));
end

assign ap_done = 1'b0;

assign ap_enable_pp0 = (ap_idle_pp0 ^ 1'b1);

assign ap_phi_reg_pp0_iter0_p_0_0_2_reg_333 = 'bx;

assign ap_phi_reg_pp0_iter0_p_0_1_2_reg_321 = 'bx;

assign ap_phi_reg_pp0_iter0_tmp_data_V_reg_356 = 'bx;

assign ap_phi_reg_pp0_iter0_tmp_last_reg_367 = 'bx;

assign ap_phi_reg_pp0_iter0_valid_write_req_apply_2_reg_345 = 'bx;

assign ap_phi_reg_pp0_iter0_valid_write_req_data_1_reg_378 = 'bx;

assign ap_phi_reg_pp0_iter0_valid_write_req_data_3_reg_412 = 'bx;

assign ap_phi_reg_pp0_iter0_write_req_apply_with_time_req_apply_addr_reg_299 = 'bx;

assign ap_phi_reg_pp0_iter0_write_req_apply_with_time_req_apply_num_reg_310 = 'bx;

assign ap_phi_reg_pp0_iter0_write_req_data_0_2_reg_400 = 'bx;

assign ap_phi_reg_pp0_iter0_write_req_data_1_2_reg_388 = 'bx;

assign ap_ready = 1'b0;

assign before_delay_dram_write_req_apply_with_time_V_req_apply_addr_din = ap_phi_mux_write_req_apply_with_time_req_apply_addr_phi_fu_302_p4;

assign before_delay_dram_write_req_apply_with_time_V_req_apply_addr_write = before_delay_dram_write_req_apply_with_time_V_time1_update;

assign before_delay_dram_write_req_apply_with_time_V_req_apply_num_din = ap_phi_mux_write_req_apply_with_time_req_apply_num_phi_fu_313_p4;

assign before_delay_dram_write_req_apply_with_time_V_req_apply_num_write = before_delay_dram_write_req_apply_with_time_V_time1_update;

assign before_delay_dram_write_req_apply_with_time_V_time_din = write_req_apply_with_time_time_reg_287;

assign before_delay_dram_write_req_apply_with_time_V_time_write = before_delay_dram_write_req_apply_with_time_V_time1_update;

assign before_delay_dram_write_req_data_V_data_V_din = ap_phi_mux_tmp_data_V_phi_fu_359_p4;

assign before_delay_dram_write_req_data_V_data_V_write = before_delay_dram_write_req_data_V_last1_update;

assign before_delay_dram_write_req_data_V_last_din = ap_phi_mux_tmp_last_phi_fu_370_p4;

assign before_delay_dram_write_req_data_V_last_write = before_delay_dram_write_req_data_V_last1_update;

assign empty_n_1_nbread_fu_188_p3_0 = (host_dram_write_req_data_V_last_empty_n & host_dram_write_req_data_V_data_V_empty_n);

assign empty_n_4_fu_423_p1 = empty_n_nbread_fu_180_p3_0;

assign empty_n_5_fu_435_p1 = empty_n_1_nbread_fu_188_p3_0;

assign empty_n_nbread_fu_180_p3_0 = (host_dram_write_req_apply_V_num_empty_n & host_dram_write_req_apply_V_addr_empty_n);

assign full_n_1_nbwrite_fu_209_p5 = (before_delay_dram_write_req_data_V_last_full_n & before_delay_dram_write_req_data_V_data_V_full_n);

assign full_n_nbwrite_fu_196_p7 = (before_delay_dram_write_req_apply_with_time_V_time_full_n & before_delay_dram_write_req_apply_with_time_V_req_apply_num_full_n & before_delay_dram_write_req_apply_with_time_V_req_apply_addr_full_n);

assign host_dram_write_req_apply_V_addr_read = host_dram_write_req_apply_V_num0_update;

assign host_dram_write_req_apply_V_num_read = host_dram_write_req_apply_V_num0_update;

assign host_dram_write_req_data_V_data_V_read = host_dram_write_req_data_V_last0_update;

assign host_dram_write_req_data_V_last_read = host_dram_write_req_data_V_last0_update;

assign not_full_n_i5_fu_460_p2 = (full_n_1_nbwrite_fu_209_p5 ^ 1'd1);

assign not_full_n_i_fu_453_p2 = (full_n_nbwrite_fu_196_p7 ^ 1'd1);

assign p_valid_write_req_data_1_fu_466_p2 = (not_full_n_i5_fu_460_p2 & ap_phi_mux_valid_write_req_data_1_phi_fu_381_p4);

assign time_fu_447_p2 = (write_req_apply_with_time_time_reg_287 + 64'd1);

endmodule //dram_write_req_time_marker
