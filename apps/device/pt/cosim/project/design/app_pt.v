// ==============================================================
// RTL generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2017.4.op
// Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="app_pt,hls_ip_2017_4_op,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xcvu9p-flgb2104-2-i,HLS_INPUT_CLOCK=4.000000,HLS_INPUT_ARCH=others,HLS_SYN_CLOCK=2.997000,HLS_SYN_LAT=-1,HLS_SYN_TPT=none,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=569,HLS_SYN_LUT=289}" *)

module app_pt (
        ap_clk,
        ap_rst,
        ap_start,
        ap_done,
ap_continue,
        ap_idle,
        ap_ready,
        reset_app_pt_V_dout,
        reset_app_pt_V_empty_n,
        reset_app_pt_V_read,
        app_input_data_V_data_V_dout,
        app_input_data_V_data_V_empty_n,
        app_input_data_V_data_V_read,
        app_input_data_V_len_dout,
        app_input_data_V_len_empty_n,
        app_input_data_V_len_read,
        app_input_data_V_eop_dout,
        app_input_data_V_eop_empty_n,
        app_input_data_V_eop_read,
        app_output_data_V_data_V_din,
        app_output_data_V_data_V_full_n,
        app_output_data_V_data_V_write,
        app_output_data_V_len_din,
        app_output_data_V_len_full_n,
        app_output_data_V_len_write,
        app_output_data_V_eop_din,
        app_output_data_V_eop_full_n,
        app_output_data_V_eop_write,
        app_input_params_V_dout,
        app_input_params_V_empty_n,
        app_input_params_V_read
);

parameter    ap_ST_fsm_state1 = 2'd1;
parameter    ap_ST_fsm_pp0_stage0 = 2'd2;

input   ap_clk;
input   ap_rst;
input   ap_start;
output   ap_done;
output   ap_idle;
output ap_ready; output ap_continue;
input   reset_app_pt_V_dout;
input   reset_app_pt_V_empty_n;
output   reset_app_pt_V_read;
input  [511:0] app_input_data_V_data_V_dout;
input   app_input_data_V_data_V_empty_n;
output   app_input_data_V_data_V_read;
input  [15:0] app_input_data_V_len_dout;
input   app_input_data_V_len_empty_n;
output   app_input_data_V_len_read;
input   app_input_data_V_eop_dout;
input   app_input_data_V_eop_empty_n;
output   app_input_data_V_eop_read;
output  [511:0] app_output_data_V_data_V_din;
input   app_output_data_V_data_V_full_n;
output   app_output_data_V_data_V_write;
output  [15:0] app_output_data_V_len_din;
input   app_output_data_V_len_full_n;
output   app_output_data_V_len_write;
output   app_output_data_V_eop_din;
input   app_output_data_V_eop_full_n;
output   app_output_data_V_eop_write;
input  [31:0] app_input_params_V_dout;
input   app_input_params_V_empty_n;
output   app_input_params_V_read;

reg ap_idle;
reg reset_app_pt_V_read;
reg app_input_params_V_read;

(* fsm_encoding = "none" *) reg   [1:0] ap_CS_fsm;
wire    ap_CS_fsm_state1;
reg    app_output_data_V_data_V_blk_n;
wire    ap_CS_fsm_pp0_stage0;
reg    ap_enable_reg_pp0_iter1;
wire    ap_block_pp0_stage0;
reg   [0:0] empty_n_3_reg_303;
reg   [0:0] empty_n_2_reg_307;
reg    app_output_data_V_len_blk_n;
reg    app_output_data_V_eop_blk_n;
reg   [0:0] reset_reg_195;
reg   [0:0] reset_be_reg_217;
wire   [0:0] empty_n_3_fu_234_p1;
wire    ap_block_state2_pp0_stage0_iter0;
wire    app_output_data_V_len1_status;
reg    ap_predicate_op50_write_state3;
reg    ap_block_state3_pp0_stage0_iter1;
reg    ap_block_pp0_stage0_11001;
reg   [0:0] ap_phi_mux_reset_phi_fu_199_p4;
reg   [511:0] tmp_data_V_reg_311;
reg   [15:0] tmp_len_reg_316;
reg   [0:0] tmp_eop_reg_321;
reg    ap_enable_reg_pp0_iter0;
reg    ap_block_pp0_stage0_subdone;
reg   [0:0] ap_phi_mux_reset_1_phi_fu_210_p4;
wire   [0:0] ap_phi_reg_pp0_iter0_reset_1_reg_207;
wire   [0:0] p_reset_1_fu_276_p2;
wire   [0:0] ap_phi_reg_pp0_iter0_reset_be_reg_217;
reg    app_input_data_V_len0_update;
wire   [0:0] grp_nbread_fu_166_p4_0;
reg    app_output_data_V_len1_update;
reg    ap_block_pp0_stage0_01001;
reg   [31:0] reset_cnt_fu_156;
wire   [31:0] p_s_fu_283_p3;
wire   [31:0] reset_cnt_1_fu_258_p2;
wire   [0:0] tmp_4_fu_264_p2;
wire   [0:0] not_s_fu_270_p2;
reg   [1:0] ap_NS_fsm;
reg    ap_idle_pp0;
wire    ap_enable_pp0;
reg    ap_condition_124;
reg    ap_condition_121;

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
    if ((1'b1 == ap_condition_121)) begin
        if (((ap_phi_mux_reset_phi_fu_199_p4 == 1'd0) & (empty_n_3_fu_234_p1 == 1'd0))) begin
            reset_be_reg_217 <= 1'd0;
        end else if (((ap_phi_mux_reset_phi_fu_199_p4 == 1'd1) | (empty_n_3_fu_234_p1 == 1'd1))) begin
            reset_be_reg_217 <= p_reset_1_fu_276_p2;
        end else if ((1'b1 == 1'b1)) begin
            reset_be_reg_217 <= ap_phi_reg_pp0_iter0_reset_be_reg_217;
        end
    end
end

always @ (posedge ap_clk) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter0 == 1'b1) & ((ap_phi_mux_reset_phi_fu_199_p4 == 1'd1) | (empty_n_3_fu_234_p1 == 1'd1)))) begin
        reset_cnt_fu_156 <= p_s_fu_283_p3;
    end else if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        reset_cnt_fu_156 <= 32'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        reset_reg_195 <= reset_be_reg_217;
    end else if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        reset_reg_195 <= 1'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_phi_mux_reset_phi_fu_199_p4 == 1'd0) & (empty_n_3_fu_234_p1 == 1'd0) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        empty_n_2_reg_307 <= grp_nbread_fu_166_p4_0;
        tmp_data_V_reg_311 <= app_input_data_V_data_V_dout;
        tmp_eop_reg_321 <= app_input_data_V_eop_dout;
        tmp_len_reg_316 <= app_input_data_V_len_dout;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_phi_mux_reset_phi_fu_199_p4 == 1'd0) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        empty_n_3_reg_303 <= reset_app_pt_V_empty_n;
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
    if (((ap_enable_reg_pp0_iter1 == 1'b0) & (ap_enable_reg_pp0_iter0 == 1'b0))) begin
        ap_idle_pp0 = 1'b1;
    end else begin
        ap_idle_pp0 = 1'b0;
    end
end

always @ (*) begin
    if ((1'b1 == ap_condition_124)) begin
        if (((empty_n_3_fu_234_p1 == 1'd1) & (ap_phi_mux_reset_phi_fu_199_p4 == 1'd0))) begin
            ap_phi_mux_reset_1_phi_fu_210_p4 = reset_app_pt_V_empty_n;
        end else if ((ap_phi_mux_reset_phi_fu_199_p4 == 1'd1)) begin
            ap_phi_mux_reset_1_phi_fu_210_p4 = ap_phi_mux_reset_phi_fu_199_p4;
        end else begin
            ap_phi_mux_reset_1_phi_fu_210_p4 = ap_phi_reg_pp0_iter0_reset_1_reg_207;
        end
    end else begin
        ap_phi_mux_reset_1_phi_fu_210_p4 = ap_phi_reg_pp0_iter0_reset_1_reg_207;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        ap_phi_mux_reset_phi_fu_199_p4 = reset_be_reg_217;
    end else begin
        ap_phi_mux_reset_phi_fu_199_p4 = reset_reg_195;
    end
end

always @ (*) begin
    if ((((app_input_data_V_len_empty_n & app_input_data_V_eop_empty_n & app_input_data_V_data_V_empty_n) == 1'b1) & (((1'b0 == ap_block_pp0_stage0_11001) & (ap_phi_mux_reset_phi_fu_199_p4 == 1'd0) & (empty_n_3_fu_234_p1 == 1'd0) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter0 == 1'b1)) | ((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter0 == 1'b1) & ((ap_phi_mux_reset_phi_fu_199_p4 == 1'd1) | (empty_n_3_fu_234_p1 == 1'd1)))))) begin
        app_input_data_V_len0_update = 1'b1;
    end else begin
        app_input_data_V_len0_update = 1'b0;
    end
end

always @ (*) begin
    if (((app_input_params_V_empty_n == 1'b1) & (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & ((reset_reg_195 == 1'd1) | (empty_n_3_reg_303 == 1'd1))) | ((1'b0 == ap_block_pp0_stage0_11001) & (empty_n_3_reg_303 == 1'd0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (app_input_params_V_empty_n == 1'b1))))) begin
        app_input_params_V_read = 1'b1;
    end else begin
        app_input_params_V_read = 1'b0;
    end
end

always @ (*) begin
    if (((empty_n_2_reg_307 == 1'd1) & (empty_n_3_reg_303 == 1'd0) & (1'b0 == ap_block_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        app_output_data_V_data_V_blk_n = app_output_data_V_data_V_full_n;
    end else begin
        app_output_data_V_data_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((empty_n_2_reg_307 == 1'd1) & (empty_n_3_reg_303 == 1'd0) & (1'b0 == ap_block_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        app_output_data_V_eop_blk_n = app_output_data_V_eop_full_n;
    end else begin
        app_output_data_V_eop_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_predicate_op50_write_state3 == 1'b1))) begin
        app_output_data_V_len1_update = 1'b1;
    end else begin
        app_output_data_V_len1_update = 1'b0;
    end
end

always @ (*) begin
    if (((empty_n_2_reg_307 == 1'd1) & (empty_n_3_reg_303 == 1'd0) & (1'b0 == ap_block_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        app_output_data_V_len_blk_n = app_output_data_V_len_full_n;
    end else begin
        app_output_data_V_len_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_phi_mux_reset_phi_fu_199_p4 == 1'd0) & (1'b1 == ap_CS_fsm_pp0_stage0) & (reset_app_pt_V_empty_n == 1'b1) & (ap_enable_reg_pp0_iter0 == 1'b1))) begin
        reset_app_pt_V_read = 1'b1;
    end else begin
        reset_app_pt_V_read = 1'b0;
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
    ap_block_pp0_stage0_01001 = ((app_output_data_V_len1_status == 1'b0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (ap_predicate_op50_write_state3 == 1'b1));
end

always @ (*) begin
    ap_block_pp0_stage0_11001 = ((app_output_data_V_len1_status == 1'b0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (ap_predicate_op50_write_state3 == 1'b1));
end

always @ (*) begin
    ap_block_pp0_stage0_subdone = ((app_output_data_V_len1_status == 1'b0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (ap_predicate_op50_write_state3 == 1'b1));
end

assign ap_block_state2_pp0_stage0_iter0 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_state3_pp0_stage0_iter1 = ((app_output_data_V_len1_status == 1'b0) & (ap_predicate_op50_write_state3 == 1'b1));
end

always @ (*) begin
    ap_condition_121 = ((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter0 == 1'b1));
end

always @ (*) begin
    ap_condition_124 = ((1'b0 == ap_block_pp0_stage0) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter0 == 1'b1));
end

assign ap_done = 1'b0;

assign ap_enable_pp0 = (ap_idle_pp0 ^ 1'b1);

assign ap_phi_reg_pp0_iter0_reset_1_reg_207 = 'bx;

assign ap_phi_reg_pp0_iter0_reset_be_reg_217 = 'bx;

always @ (*) begin
    ap_predicate_op50_write_state3 = ((empty_n_2_reg_307 == 1'd1) & (empty_n_3_reg_303 == 1'd0));
end

assign ap_ready = 1'b0;

assign app_input_data_V_data_V_read = app_input_data_V_len0_update;

assign app_input_data_V_eop_read = app_input_data_V_len0_update;

assign app_input_data_V_len_read = app_input_data_V_len0_update;

assign app_output_data_V_data_V_din = tmp_data_V_reg_311;

assign app_output_data_V_data_V_write = app_output_data_V_len1_update;

assign app_output_data_V_eop_din = tmp_eop_reg_321;

assign app_output_data_V_eop_write = app_output_data_V_len1_update;

assign app_output_data_V_len1_status = (app_output_data_V_len_full_n & app_output_data_V_eop_full_n & app_output_data_V_data_V_full_n);

assign app_output_data_V_len_din = tmp_len_reg_316;

assign app_output_data_V_len_write = app_output_data_V_len1_update;

assign empty_n_3_fu_234_p1 = reset_app_pt_V_empty_n;

assign grp_nbread_fu_166_p4_0 = (app_input_data_V_len_empty_n & app_input_data_V_eop_empty_n & app_input_data_V_data_V_empty_n);

assign not_s_fu_270_p2 = (tmp_4_fu_264_p2 ^ 1'd1);

assign p_reset_1_fu_276_p2 = (not_s_fu_270_p2 & ap_phi_mux_reset_1_phi_fu_210_p4);

assign p_s_fu_283_p3 = ((tmp_4_fu_264_p2[0:0] === 1'b1) ? 32'd0 : reset_cnt_1_fu_258_p2);

assign reset_cnt_1_fu_258_p2 = (reset_cnt_fu_156 + 32'd1);

assign tmp_4_fu_264_p2 = ((reset_cnt_1_fu_258_p2 == 32'd2048) ? 1'b1 : 1'b0);

endmodule //app_pt
