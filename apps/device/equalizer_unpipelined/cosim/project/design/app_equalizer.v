// ==============================================================
// RTL generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2017.4.op
// Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="app_equalizer,hls_ip_2017_4_op,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xcvu9p-flgb2104-2-i,HLS_INPUT_CLOCK=4.000000,HLS_INPUT_ARCH=others,HLS_SYN_CLOCK=3.227875,HLS_SYN_LAT=-1,HLS_SYN_TPT=none,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=63165,HLS_SYN_LUT=46863}" *)

module app_equalizer (
        ap_clk,
        ap_rst,
        ap_start,
        ap_done,
ap_continue,
        ap_idle,
        ap_ready,
        reset_app_equalizer_V_dout,
        reset_app_equalizer_V_empty_n,
        reset_app_equalizer_V_read,
        app_input_params_V_dout,
        app_input_params_V_empty_n,
        app_input_params_V_read,
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
        app_output_data_V_eop_write
);

parameter    ap_ST_fsm_state1 = 3'd1;
parameter    ap_ST_fsm_state2 = 3'd2;
parameter    ap_ST_fsm_state3 = 3'd4;

input   ap_clk;
input   ap_rst;
input   ap_start;
output   ap_done;
output   ap_idle;
output ap_ready; output ap_continue;
input   reset_app_equalizer_V_dout;
input   reset_app_equalizer_V_empty_n;
output   reset_app_equalizer_V_read;
input  [31:0] app_input_params_V_dout;
input   app_input_params_V_empty_n;
output   app_input_params_V_read;
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

reg ap_idle;
reg reset_app_equalizer_V_read;
reg app_input_params_V_read;
reg app_input_data_V_data_V_read;
reg app_input_data_V_len_read;
reg app_input_data_V_eop_read;
reg app_output_data_V_data_V_write;
reg app_output_data_V_len_write;
reg app_output_data_V_eop_write;

(* fsm_encoding = "none" *) reg   [2:0] ap_CS_fsm;
wire    ap_CS_fsm_state1;
wire   [0:0] empty_n_2_fu_120_p1;
reg   [0:0] empty_n_2_reg_181;
wire    ap_CS_fsm_state2;
wire   [0:0] ap_phi_mux_reset_phi_fu_68_p4;
wire   [0:0] empty_n_fu_124_p1;
reg   [0:0] empty_n_reg_186;
reg   [31:0] tmp_reg_190;
wire    grp_equalizer_fu_98_ap_start;
wire    grp_equalizer_fu_98_ap_done;
wire    grp_equalizer_fu_98_ap_idle;
wire    grp_equalizer_fu_98_ap_ready;
wire    grp_equalizer_fu_98_app_input_data_V_data_V_read;
wire    grp_equalizer_fu_98_app_input_data_V_len_read;
wire    grp_equalizer_fu_98_app_input_data_V_eop_read;
wire   [511:0] grp_equalizer_fu_98_app_output_data_V_data_V_din;
wire    grp_equalizer_fu_98_app_output_data_V_data_V_write;
wire   [15:0] grp_equalizer_fu_98_app_output_data_V_len_din;
wire    grp_equalizer_fu_98_app_output_data_V_len_write;
wire    grp_equalizer_fu_98_app_output_data_V_eop_din;
wire    grp_equalizer_fu_98_app_output_data_V_eop_write;
reg   [0:0] reset_reg_64;
reg   [0:0] ap_phi_mux_reset_be_phi_fu_90_p4;
wire    ap_CS_fsm_state3;
reg    ap_predicate_op34_call_state3;
reg    ap_block_state3_on_subcall_done;
reg   [0:0] reset_1_reg_76;
wire   [0:0] p_reset_1_fu_154_p2;
reg    ap_reg_grp_equalizer_fu_98_ap_start;
reg   [31:0] reset_cnt_fu_48;
wire   [31:0] p_s_fu_161_p3;
wire   [31:0] reset_cnt_1_fu_136_p2;
wire   [0:0] tmp_s_fu_142_p2;
wire   [0:0] not_s_fu_148_p2;
reg   [2:0] ap_NS_fsm;

// power-on initialization
initial begin
#0 ap_CS_fsm = 3'd1;
#0 ap_reg_grp_equalizer_fu_98_ap_start = 1'b0;
end

equalizer grp_equalizer_fu_98(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst),
    .ap_start(grp_equalizer_fu_98_ap_start),
    .ap_done(grp_equalizer_fu_98_ap_done),
    .ap_idle(grp_equalizer_fu_98_ap_idle),
    .ap_ready(grp_equalizer_fu_98_ap_ready),
    .image_height(tmp_reg_190),
    .app_input_data_V_data_V_dout(app_input_data_V_data_V_dout),
    .app_input_data_V_data_V_empty_n(app_input_data_V_data_V_empty_n),
    .app_input_data_V_data_V_read(grp_equalizer_fu_98_app_input_data_V_data_V_read),
    .app_input_data_V_len_dout(app_input_data_V_len_dout),
    .app_input_data_V_len_empty_n(app_input_data_V_len_empty_n),
    .app_input_data_V_len_read(grp_equalizer_fu_98_app_input_data_V_len_read),
    .app_input_data_V_eop_dout(app_input_data_V_eop_dout),
    .app_input_data_V_eop_empty_n(app_input_data_V_eop_empty_n),
    .app_input_data_V_eop_read(grp_equalizer_fu_98_app_input_data_V_eop_read),
    .app_output_data_V_data_V_din(grp_equalizer_fu_98_app_output_data_V_data_V_din),
    .app_output_data_V_data_V_full_n(app_output_data_V_data_V_full_n),
    .app_output_data_V_data_V_write(grp_equalizer_fu_98_app_output_data_V_data_V_write),
    .app_output_data_V_len_din(grp_equalizer_fu_98_app_output_data_V_len_din),
    .app_output_data_V_len_full_n(app_output_data_V_len_full_n),
    .app_output_data_V_len_write(grp_equalizer_fu_98_app_output_data_V_len_write),
    .app_output_data_V_eop_din(grp_equalizer_fu_98_app_output_data_V_eop_din),
    .app_output_data_V_eop_full_n(app_output_data_V_eop_full_n),
    .app_output_data_V_eop_write(grp_equalizer_fu_98_app_output_data_V_eop_write)
);

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_CS_fsm <= ap_ST_fsm_state1;
    end else begin
        ap_CS_fsm <= ap_NS_fsm;
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_reg_grp_equalizer_fu_98_ap_start <= 1'b0;
    end else begin
        if (((empty_n_fu_124_p1 == 1'd1) & (ap_phi_mux_reset_phi_fu_68_p4 == 1'd0) & (empty_n_2_fu_120_p1 == 1'd0) & (1'b1 == ap_CS_fsm_state2))) begin
            ap_reg_grp_equalizer_fu_98_ap_start <= 1'b1;
        end else if ((grp_equalizer_fu_98_ap_ready == 1'b1)) begin
            ap_reg_grp_equalizer_fu_98_ap_start <= 1'b0;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_CS_fsm_state2)) begin
        if (((ap_phi_mux_reset_phi_fu_68_p4 == 1'd0) & (empty_n_2_fu_120_p1 == 1'd1))) begin
            reset_1_reg_76 <= reset_app_equalizer_V_empty_n;
        end else if ((ap_phi_mux_reset_phi_fu_68_p4 == 1'd1)) begin
            reset_1_reg_76 <= reset_reg_64;
        end
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_state3) & (1'b0 == ap_block_state3_on_subcall_done) & ((reset_reg_64 == 1'd1) | (empty_n_2_reg_181 == 1'd1)))) begin
        reset_cnt_fu_48 <= p_s_fu_161_p3;
    end else if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        reset_cnt_fu_48 <= 32'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_state3) & (1'b0 == ap_block_state3_on_subcall_done))) begin
        reset_reg_64 <= ap_phi_mux_reset_be_phi_fu_90_p4;
    end else if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        reset_reg_64 <= 1'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((ap_phi_mux_reset_phi_fu_68_p4 == 1'd0) & (1'b1 == ap_CS_fsm_state2))) begin
        empty_n_2_reg_181 <= reset_app_equalizer_V_empty_n;
    end
end

always @ (posedge ap_clk) begin
    if (((ap_phi_mux_reset_phi_fu_68_p4 == 1'd0) & (empty_n_2_fu_120_p1 == 1'd0) & (1'b1 == ap_CS_fsm_state2))) begin
        empty_n_reg_186 <= app_input_params_V_empty_n;
        tmp_reg_190 <= app_input_params_V_dout;
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
    if ((1'b1 == ap_CS_fsm_state3)) begin
        if (((reset_reg_64 == 1'd0) & (empty_n_2_reg_181 == 1'd0))) begin
            ap_phi_mux_reset_be_phi_fu_90_p4 = 1'd0;
        end else if (((reset_reg_64 == 1'd1) | (empty_n_2_reg_181 == 1'd1))) begin
            ap_phi_mux_reset_be_phi_fu_90_p4 = p_reset_1_fu_154_p2;
        end else begin
            ap_phi_mux_reset_be_phi_fu_90_p4 = 'bx;
        end
    end else begin
        ap_phi_mux_reset_be_phi_fu_90_p4 = 'bx;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_state3) & (ap_predicate_op34_call_state3 == 1'b1))) begin
        app_input_data_V_data_V_read = grp_equalizer_fu_98_app_input_data_V_data_V_read;
    end else begin
        app_input_data_V_data_V_read = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_state3) & (ap_predicate_op34_call_state3 == 1'b1))) begin
        app_input_data_V_eop_read = grp_equalizer_fu_98_app_input_data_V_eop_read;
    end else begin
        app_input_data_V_eop_read = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_state3) & (ap_predicate_op34_call_state3 == 1'b1))) begin
        app_input_data_V_len_read = grp_equalizer_fu_98_app_input_data_V_len_read;
    end else begin
        app_input_data_V_len_read = 1'b0;
    end
end

always @ (*) begin
    if (((app_input_params_V_empty_n == 1'b1) & (((ap_phi_mux_reset_phi_fu_68_p4 == 1'd0) & (empty_n_2_fu_120_p1 == 1'd0) & (1'b1 == ap_CS_fsm_state2) & (app_input_params_V_empty_n == 1'b1)) | ((1'b1 == ap_CS_fsm_state3) & (1'b0 == ap_block_state3_on_subcall_done) & ((reset_reg_64 == 1'd1) | (empty_n_2_reg_181 == 1'd1)))))) begin
        app_input_params_V_read = 1'b1;
    end else begin
        app_input_params_V_read = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_state3) & (ap_predicate_op34_call_state3 == 1'b1))) begin
        app_output_data_V_data_V_write = grp_equalizer_fu_98_app_output_data_V_data_V_write;
    end else begin
        app_output_data_V_data_V_write = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_state3) & (ap_predicate_op34_call_state3 == 1'b1))) begin
        app_output_data_V_eop_write = grp_equalizer_fu_98_app_output_data_V_eop_write;
    end else begin
        app_output_data_V_eop_write = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_state3) & (ap_predicate_op34_call_state3 == 1'b1))) begin
        app_output_data_V_len_write = grp_equalizer_fu_98_app_output_data_V_len_write;
    end else begin
        app_output_data_V_len_write = 1'b0;
    end
end

always @ (*) begin
    if (((ap_phi_mux_reset_phi_fu_68_p4 == 1'd0) & (1'b1 == ap_CS_fsm_state2) & (reset_app_equalizer_V_empty_n == 1'b1))) begin
        reset_app_equalizer_V_read = 1'b1;
    end else begin
        reset_app_equalizer_V_read = 1'b0;
    end
end

always @ (*) begin
    case (ap_CS_fsm)
        ap_ST_fsm_state1 : begin
            if (((ap_start == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
                ap_NS_fsm = ap_ST_fsm_state2;
            end else begin
                ap_NS_fsm = ap_ST_fsm_state1;
            end
        end
        ap_ST_fsm_state2 : begin
            ap_NS_fsm = ap_ST_fsm_state3;
        end
        ap_ST_fsm_state3 : begin
            if (((1'b1 == ap_CS_fsm_state3) & (1'b0 == ap_block_state3_on_subcall_done))) begin
                ap_NS_fsm = ap_ST_fsm_state2;
            end else begin
                ap_NS_fsm = ap_ST_fsm_state3;
            end
        end
        default : begin
            ap_NS_fsm = 'bx;
        end
    endcase
end

assign ap_CS_fsm_state1 = ap_CS_fsm[32'd0];

assign ap_CS_fsm_state2 = ap_CS_fsm[32'd1];

assign ap_CS_fsm_state3 = ap_CS_fsm[32'd2];

always @ (*) begin
    ap_block_state3_on_subcall_done = ((grp_equalizer_fu_98_ap_done == 1'b0) & (ap_predicate_op34_call_state3 == 1'b1));
end

assign ap_done = 1'b0;

assign ap_phi_mux_reset_phi_fu_68_p4 = reset_reg_64;

always @ (*) begin
    ap_predicate_op34_call_state3 = ((empty_n_reg_186 == 1'd1) & (reset_reg_64 == 1'd0) & (empty_n_2_reg_181 == 1'd0));
end

assign ap_ready = 1'b0;

assign app_output_data_V_data_V_din = grp_equalizer_fu_98_app_output_data_V_data_V_din;

assign app_output_data_V_eop_din = grp_equalizer_fu_98_app_output_data_V_eop_din;

assign app_output_data_V_len_din = grp_equalizer_fu_98_app_output_data_V_len_din;

assign empty_n_2_fu_120_p1 = reset_app_equalizer_V_empty_n;

assign empty_n_fu_124_p1 = app_input_params_V_empty_n;

assign grp_equalizer_fu_98_ap_start = ap_reg_grp_equalizer_fu_98_ap_start;

assign not_s_fu_148_p2 = (tmp_s_fu_142_p2 ^ 1'd1);

assign p_reset_1_fu_154_p2 = (reset_1_reg_76 & not_s_fu_148_p2);

assign p_s_fu_161_p3 = ((tmp_s_fu_142_p2[0:0] === 1'b1) ? 32'd0 : reset_cnt_1_fu_136_p2);

assign reset_cnt_1_fu_136_p2 = (reset_cnt_fu_48 + 32'd1);

assign tmp_s_fu_142_p2 = ((reset_cnt_1_fu_136_p2 == 32'd2048) ? 1'b1 : 1'b0);

endmodule //app_equalizer
