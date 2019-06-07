#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

map<string, string> neccessaryPinsMap;
vector<string> stringOccurs;
vector<string> unmatchedStrings;

void setup() {
  neccessaryPinsMap["peek_req_V_dout"] = "input [31:0] peek_req_V_dout;";
  neccessaryPinsMap["peek_req_V_empty_n"] = "input peek_req_V_empty_n;";
  neccessaryPinsMap["peek_req_V_read"] = "output peek_req_V_read;";
  neccessaryPinsMap["peek_resp_V_din"] = "output [31:0] peek_resp_V_din;";
  neccessaryPinsMap["peek_resp_V_full_n"] = "input peek_resp_V_full_n;";
  neccessaryPinsMap["peek_resp_V_write"] = "output peek_resp_V_write;";

  neccessaryPinsMap["poke_V_data_dout"] = "input  [31:0] poke_V_data_dout;";
  neccessaryPinsMap["poke_V_data_empty_n"] = "input   poke_V_data_empty_n;";
  neccessaryPinsMap["poke_V_data_read"] = "output   poke_V_data_read;";
  neccessaryPinsMap["poke_V_tag_dout"] = "input  [31:0] poke_V_tag_dout;";
  neccessaryPinsMap["poke_V_tag_empty_n"] = "input   poke_V_tag_empty_n;";
  neccessaryPinsMap["poke_V_tag_read"] = "output   poke_V_tag_read;";

  neccessaryPinsMap["pcie_write_req_data_V_last_din"] =
      "output   pcie_write_req_data_V_last_din;";
  neccessaryPinsMap["pcie_write_req_data_V_last_full_n"] =
      "input   pcie_write_req_data_V_last_full_n;";
  neccessaryPinsMap["pcie_write_req_data_V_last_write"] =
      "output   pcie_write_req_data_V_last_write;";
  neccessaryPinsMap["pcie_write_req_data_V_data_V_din"] =
      "output  [511:0] pcie_write_req_data_V_data_V_din;";
  neccessaryPinsMap["pcie_write_req_data_V_data_V_full_n"] =
      "input   pcie_write_req_data_V_data_V_full_n;";
  neccessaryPinsMap["pcie_write_req_data_V_data_V_write"] =
      "output   pcie_write_req_data_V_data_V_write;";
  neccessaryPinsMap["pcie_write_req_apply_V_num_din"] =
      "output  [7:0] pcie_write_req_apply_V_num_din;";
  neccessaryPinsMap["pcie_write_req_apply_V_num_full_n"] =
      "input   pcie_write_req_apply_V_num_full_n;";
  neccessaryPinsMap["pcie_write_req_apply_V_num_write"] =
      "output   pcie_write_req_apply_V_num_write;";
  neccessaryPinsMap["pcie_write_req_apply_V_addr_din"] =
      "output  [63:0] pcie_write_req_apply_V_addr_din;";
  neccessaryPinsMap["pcie_write_req_apply_V_addr_full_n"] =
      "input   pcie_write_req_apply_V_addr_full_n;";
  neccessaryPinsMap["pcie_write_req_apply_V_addr_write"] =
      "output   pcie_write_req_apply_V_addr_write;";

  neccessaryPinsMap["pcie_read_req_V_num_din"] =
      "output  [7:0] pcie_read_req_V_num_din;";
  neccessaryPinsMap["pcie_read_req_V_num_full_n"] =
      "input   pcie_read_req_V_num_full_n;";
  neccessaryPinsMap["pcie_read_req_V_num_write"] =
      "output   pcie_read_req_V_num_write;";
  neccessaryPinsMap["pcie_read_req_V_addr_din"] =
      "output  [63:0] pcie_read_req_V_addr_din;";
  neccessaryPinsMap["pcie_read_req_V_addr_full_n"] =
      "input   pcie_read_req_V_addr_full_n;";
  neccessaryPinsMap["pcie_read_req_V_addr_write"] =
      "output   pcie_read_req_V_addr_write;";
  neccessaryPinsMap["pcie_read_resp_V_last_dout"] =
      "input   pcie_read_resp_V_last_dout;";
  neccessaryPinsMap["pcie_read_resp_V_last_empty_n"] =
      "input   pcie_read_resp_V_last_empty_n;";
  neccessaryPinsMap["pcie_read_resp_V_last_read"] =
      "output   pcie_read_resp_V_last_read;";
  neccessaryPinsMap["pcie_read_resp_V_data_V_dout"] =
      "input  [511:0] pcie_read_resp_V_data_V_dout;";
  neccessaryPinsMap["pcie_read_resp_V_data_V_empty_n"] =
      "input   pcie_read_resp_V_data_V_empty_n;";
  neccessaryPinsMap["pcie_read_resp_V_data_V_read"] =
      "output   pcie_read_resp_V_data_V_read;";

  neccessaryPinsMap["dramA_read_req_V_num_din"] =
      "output  [7:0] dramA_read_req_V_num_din;";
  neccessaryPinsMap["dramA_read_req_V_num_full_n"] =
      "input   dramA_read_req_V_num_full_n;";
  neccessaryPinsMap["dramA_read_req_V_num_write"] =
      "output   dramA_read_req_V_num_write;";
  neccessaryPinsMap["dramA_read_req_V_addr_din"] =
      "output  [63:0] dramA_read_req_V_addr_din;";
  neccessaryPinsMap["dramA_read_req_V_addr_full_n"] =
      "input   dramA_read_req_V_addr_full_n;";
  neccessaryPinsMap["dramA_read_req_V_addr_write"] =
      "output   dramA_read_req_V_addr_write;";
  neccessaryPinsMap["dramA_read_resp_V_last_dout"] =
      "input   dramA_read_resp_V_last_dout;";
  neccessaryPinsMap["dramA_read_resp_V_last_empty_n"] =
      "input   dramA_read_resp_V_last_empty_n;";
  neccessaryPinsMap["dramA_read_resp_V_last_read"] =
      "output   dramA_read_resp_V_last_read;";
  neccessaryPinsMap["dramA_read_resp_V_data_V_dout"] =
      "input  [511:0] dramA_read_resp_V_data_V_dout;";
  neccessaryPinsMap["dramA_read_resp_V_data_V_empty_n"] =
      "input   dramA_read_resp_V_data_V_empty_n;";
  neccessaryPinsMap["dramA_read_resp_V_data_V_read"] =
      "output   dramA_read_resp_V_data_V_read;";
  neccessaryPinsMap["dramA_write_req_data_V_last_din"] =
      "output   dramA_write_req_data_V_last_din;";
  neccessaryPinsMap["dramA_write_req_data_V_last_full_n"] =
      "input   dramA_write_req_data_V_last_full_n;";
  neccessaryPinsMap["dramA_write_req_data_V_last_write"] =
      "output   dramA_write_req_data_V_last_write;";
  neccessaryPinsMap["dramA_write_req_data_V_data_V_din"] =
      "output  [511:0] dramA_write_req_data_V_data_V_din;";
  neccessaryPinsMap["dramA_write_req_data_V_data_V_full_n"] =
      "input   dramA_write_req_data_V_data_V_full_n;";
  neccessaryPinsMap["dramA_write_req_data_V_data_V_write"] =
      "output   dramA_write_req_data_V_data_V_write;";
  neccessaryPinsMap["dramA_write_req_apply_V_num_din"] =
      "output  [7:0] dramA_write_req_apply_V_num_din;";
  neccessaryPinsMap["dramA_write_req_apply_V_num_full_n"] =
      "input   dramA_write_req_apply_V_num_full_n;";
  neccessaryPinsMap["dramA_write_req_apply_V_num_write"] =
      "output   dramA_write_req_apply_V_num_write;";
  neccessaryPinsMap["dramA_write_req_apply_V_addr_din"] =
      "output  [63:0] dramA_write_req_apply_V_addr_din;";
  neccessaryPinsMap["dramA_write_req_apply_V_addr_full_n"] =
      "input   dramA_write_req_apply_V_addr_full_n;";
  neccessaryPinsMap["dramA_write_req_apply_V_addr_write"] =
      "output   dramA_write_req_apply_V_addr_write;";

  neccessaryPinsMap["dramB_read_req_V_num_din"] =
      "output  [7:0] dramB_read_req_V_num_din;";
  neccessaryPinsMap["dramB_read_req_V_num_full_n"] =
      "input   dramB_read_req_V_num_full_n;";
  neccessaryPinsMap["dramB_read_req_V_num_write"] =
      "output   dramB_read_req_V_num_write;";
  neccessaryPinsMap["dramB_read_req_V_addr_din"] =
      "output  [63:0] dramB_read_req_V_addr_din;";
  neccessaryPinsMap["dramB_read_req_V_addr_full_n"] =
      "input   dramB_read_req_V_addr_full_n;";
  neccessaryPinsMap["dramB_read_req_V_addr_write"] =
      "output   dramB_read_req_V_addr_write;";
  neccessaryPinsMap["dramB_read_resp_V_last_dout"] =
      "input   dramB_read_resp_V_last_dout;";
  neccessaryPinsMap["dramB_read_resp_V_last_empty_n"] =
      "input   dramB_read_resp_V_last_empty_n;";
  neccessaryPinsMap["dramB_read_resp_V_last_read"] =
      "output   dramB_read_resp_V_last_read;";
  neccessaryPinsMap["dramB_read_resp_V_data_V_dout"] =
      "input  [511:0] dramB_read_resp_V_data_V_dout;";
  neccessaryPinsMap["dramB_read_resp_V_data_V_empty_n"] =
      "input   dramB_read_resp_V_data_V_empty_n;";
  neccessaryPinsMap["dramB_read_resp_V_data_V_read"] =
      "output   dramB_read_resp_V_data_V_read;";
  neccessaryPinsMap["dramB_write_req_data_V_last_din"] =
      "output   dramB_write_req_data_V_last_din;";
  neccessaryPinsMap["dramB_write_req_data_V_last_full_n"] =
      "input   dramB_write_req_data_V_last_full_n;";
  neccessaryPinsMap["dramB_write_req_data_V_last_write"] =
      "output   dramB_write_req_data_V_last_write;";
  neccessaryPinsMap["dramB_write_req_data_V_data_V_din"] =
      "output  [511:0] dramB_write_req_data_V_data_V_din;";
  neccessaryPinsMap["dramB_write_req_data_V_data_V_full_n"] =
      "input   dramB_write_req_data_V_data_V_full_n;";
  neccessaryPinsMap["dramB_write_req_data_V_data_V_write"] =
      "output   dramB_write_req_data_V_data_V_write;";
  neccessaryPinsMap["dramB_write_req_apply_V_num_din"] =
      "output  [7:0] dramB_write_req_apply_V_num_din;";
  neccessaryPinsMap["dramB_write_req_apply_V_num_full_n"] =
      "input   dramB_write_req_apply_V_num_full_n;";
  neccessaryPinsMap["dramB_write_req_apply_V_num_write"] =
      "output   dramB_write_req_apply_V_num_write;";
  neccessaryPinsMap["dramB_write_req_apply_V_addr_din"] =
      "output  [63:0] dramB_write_req_apply_V_addr_din;";
  neccessaryPinsMap["dramB_write_req_apply_V_addr_full_n"] =
      "input   dramB_write_req_apply_V_addr_full_n;";
  neccessaryPinsMap["dramB_write_req_apply_V_addr_write"] =
      "output   dramB_write_req_apply_V_addr_write;";

  neccessaryPinsMap["dramC_read_req_V_num_din"] =
      "output  [7:0] dramC_read_req_V_num_din;";
  neccessaryPinsMap["dramC_read_req_V_num_full_n"] =
      "input   dramC_read_req_V_num_full_n;";
  neccessaryPinsMap["dramC_read_req_V_num_write"] =
      "output   dramC_read_req_V_num_write;";
  neccessaryPinsMap["dramC_read_req_V_addr_din"] =
      "output  [63:0] dramC_read_req_V_addr_din;";
  neccessaryPinsMap["dramC_read_req_V_addr_full_n"] =
      "input   dramC_read_req_V_addr_full_n;";
  neccessaryPinsMap["dramC_read_req_V_addr_write"] =
      "output   dramC_read_req_V_addr_write;";
  neccessaryPinsMap["dramC_read_resp_V_last_dout"] =
      "input   dramC_read_resp_V_last_dout;";
  neccessaryPinsMap["dramC_read_resp_V_last_empty_n"] =
      "input   dramC_read_resp_V_last_empty_n;";
  neccessaryPinsMap["dramC_read_resp_V_last_read"] =
      "output   dramC_read_resp_V_last_read;";
  neccessaryPinsMap["dramC_read_resp_V_data_V_dout"] =
      "input  [511:0] dramC_read_resp_V_data_V_dout;";
  neccessaryPinsMap["dramC_read_resp_V_data_V_empty_n"] =
      "input   dramC_read_resp_V_data_V_empty_n;";
  neccessaryPinsMap["dramC_read_resp_V_data_V_read"] =
      "output   dramC_read_resp_V_data_V_read;";
  neccessaryPinsMap["dramC_write_req_data_V_last_din"] =
      "output   dramC_write_req_data_V_last_din;";
  neccessaryPinsMap["dramC_write_req_data_V_last_full_n"] =
      "input   dramC_write_req_data_V_last_full_n;";
  neccessaryPinsMap["dramC_write_req_data_V_last_write"] =
      "output   dramC_write_req_data_V_last_write;";
  neccessaryPinsMap["dramC_write_req_data_V_data_V_din"] =
      "output  [511:0] dramC_write_req_data_V_data_V_din;";
  neccessaryPinsMap["dramC_write_req_data_V_data_V_full_n"] =
      "input   dramC_write_req_data_V_data_V_full_n;";
  neccessaryPinsMap["dramC_write_req_data_V_data_V_write"] =
      "output   dramC_write_req_data_V_data_V_write;";
  neccessaryPinsMap["dramC_write_req_apply_V_num_din"] =
      "output  [7:0] dramC_write_req_apply_V_num_din;";
  neccessaryPinsMap["dramC_write_req_apply_V_num_full_n"] =
      "input   dramC_write_req_apply_V_num_full_n;";
  neccessaryPinsMap["dramC_write_req_apply_V_num_write"] =
      "output   dramC_write_req_apply_V_num_write;";
  neccessaryPinsMap["dramC_write_req_apply_V_addr_din"] =
      "output  [63:0] dramC_write_req_apply_V_addr_din;";
  neccessaryPinsMap["dramC_write_req_apply_V_addr_full_n"] =
      "input   dramC_write_req_apply_V_addr_full_n;";
  neccessaryPinsMap["dramC_write_req_apply_V_addr_write"] =
      "output   dramC_write_req_apply_V_addr_write;";

  neccessaryPinsMap["dramD_read_req_V_num_din"] =
      "output  [7:0] dramD_read_req_V_num_din;";
  neccessaryPinsMap["dramD_read_req_V_num_full_n"] =
      "input   dramD_read_req_V_num_full_n;";
  neccessaryPinsMap["dramD_read_req_V_num_write"] =
      "output   dramD_read_req_V_num_write;";
  neccessaryPinsMap["dramD_read_req_V_addr_din"] =
      "output  [63:0] dramD_read_req_V_addr_din;";
  neccessaryPinsMap["dramD_read_req_V_addr_full_n"] =
      "input   dramD_read_req_V_addr_full_n;";
  neccessaryPinsMap["dramD_read_req_V_addr_write"] =
      "output   dramD_read_req_V_addr_write;";
  neccessaryPinsMap["dramD_read_resp_V_last_dout"] =
      "input   dramD_read_resp_V_last_dout;";
  neccessaryPinsMap["dramD_read_resp_V_last_empty_n"] =
      "input   dramD_read_resp_V_last_empty_n;";
  neccessaryPinsMap["dramD_read_resp_V_last_read"] =
      "output   dramD_read_resp_V_last_read;";
  neccessaryPinsMap["dramD_read_resp_V_data_V_dout"] =
      "input  [511:0] dramD_read_resp_V_data_V_dout;";
  neccessaryPinsMap["dramD_read_resp_V_data_V_empty_n"] =
      "input   dramD_read_resp_V_data_V_empty_n;";
  neccessaryPinsMap["dramD_read_resp_V_data_V_read"] =
      "output   dramD_read_resp_V_data_V_read;";
  neccessaryPinsMap["dramD_write_req_data_V_last_din"] =
      "output   dramD_write_req_data_V_last_din;";
  neccessaryPinsMap["dramD_write_req_data_V_last_full_n"] =
      "input   dramD_write_req_data_V_last_full_n;";
  neccessaryPinsMap["dramD_write_req_data_V_last_write"] =
      "output   dramD_write_req_data_V_last_write;";
  neccessaryPinsMap["dramD_write_req_data_V_data_V_din"] =
      "output  [511:0] dramD_write_req_data_V_data_V_din;";
  neccessaryPinsMap["dramD_write_req_data_V_data_V_full_n"] =
      "input   dramD_write_req_data_V_data_V_full_n;";
  neccessaryPinsMap["dramD_write_req_data_V_data_V_write"] =
      "output   dramD_write_req_data_V_data_V_write;";
  neccessaryPinsMap["dramD_write_req_apply_V_num_din"] =
      "output  [7:0] dramD_write_req_apply_V_num_din;";
  neccessaryPinsMap["dramD_write_req_apply_V_num_full_n"] =
      "input   dramD_write_req_apply_V_num_full_n;";
  neccessaryPinsMap["dramD_write_req_apply_V_num_write"] =
      "output   dramD_write_req_apply_V_num_write;";
  neccessaryPinsMap["dramD_write_req_apply_V_addr_din"] =
      "output  [63:0] dramD_write_req_apply_V_addr_din;";
  neccessaryPinsMap["dramD_write_req_apply_V_addr_full_n"] =
      "input   dramD_write_req_apply_V_addr_full_n;";
  neccessaryPinsMap["dramD_write_req_apply_V_addr_write"] =
      "output   dramD_write_req_apply_V_addr_write;";
}

string eliminateSpc(string str) {
  int l = 0;
  int r = str.size() - 1;
  while (str[l] == ' ' || str[l] == '\t')
    l++;
  while (str[r] == ' ' || str[r] == '\t')
    r--;
  return str.substr(l, r - l + 1);
}

int main() {
  setup();
  freopen("project/design/interconnects.v", "r", stdin);
  freopen("project/design/interconnects.v.new", "w", stdout);
  string s;
  bool reachModuleStart = false;
  bool finished = false;
  while (getline(cin, s)) {
    bool noPrint = false;
    if (!finished) {
      if (reachModuleStart) {
        if (eliminateSpc(s) == ");") {
          cout << "," << endl;
          bool first = true;
          for (auto it = neccessaryPinsMap.begin();
               it != neccessaryPinsMap.end(); ++it) {
            string key = it->first;
            bool matched = false;
            for (auto s : stringOccurs) {
              if (s == key) {
                matched = true;
                break;
              }
            }
            if (!matched) {
              unmatchedStrings.push_back(key);
              if (first) {
                first = false;
                cout << key << endl;
              } else {
                cout << "," << key << endl;
              }
            }
          }
          noPrint = true;
          cout << ");" << endl;
          for (auto s : unmatchedStrings) {
            cout << neccessaryPinsMap[s] << endl;
          }
          finished = true;
        } else {
          stringOccurs.push_back(eliminateSpc(s.substr(0, s.size() - 1)));
        }
      } else if (s.substr(0, 6) == "module") {
        reachModuleStart = true;
      }
    }
    if (!noPrint) {
      cout << s << endl;
    }
  }

  return 0;
}
