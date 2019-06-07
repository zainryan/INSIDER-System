if [ -z "$LLVM_SRC_PATH" ]; then
    echo "Need to set LLVM_SRC_PATH"
    exit 1
fi

if [ -z "$LLVM_BUILD_PATH" ]; then
    echo "Need to set LLVM_BUILD_PATH"
    exit 1
fi

if [ -z "$AWS_FPGA_PATH" ]; then
    echo "Need to set AWS_FPGA_PATH"
    exit 1
fi

orig_path=`pwd`

sudo yum -y install elfutils-libelf-devel

sudo mkdir -p /opt/Xilinx/license
sudo cp lic/XilinxAWS.lic /opt/Xilinx/license/XilinxAWS.lic

sudo cp patch/boost/queue.hpp /usr/include/boost/lockfree
sudo cp patch/aws/cosim/sh_bfm.sv $AWS_FPGA_PATH/hdk/common/verif/models/sh_bfm
sudo cp patch/aws/synthesis/strategy_TIMING.tcl $AWS_FPGA_PATH/hdk/common/shell_v04261818/build/scripts/strategy_TIMING.tcl
sudo cp patch/aws/runtime/fpga_pci.h $AWS_FPGA_PATH/sdk/userspace/include/fpga_pci.h
sudo cp patch/aws/runtime/fpga_pci.c $AWS_FPGA_PATH/sdk/userspace/fpga_libs/fpga_pci/fpga_pci.c

sudo rm -rf /usr/include/staccel
sudo mkdir /usr/include/staccel
sudo cp STAccel/inc/staccel* /usr/include/staccel
sudo cp -r STAccel/inc/hls_csim /usr/include
sudo rm -rf /usr/staccel/
sudo mkdir /usr/staccel/
sudo cp -r STAccel/template /usr/staccel
g++ STAccel/src/iopinChecker.cpp -std=c++11 -O3 \
    -o staccel_pinckr
sudo mv staccel_pinckr /usr/bin
sudo cp STAccel/shell/* /usr/bin

sudo rm -rf /usr/include/insider
sudo mkdir /usr/include/insider
sudo cp Insider/inc/* /usr/include/insider
sudo rm -rf /usr/insider
sudo mkdir /usr/insider
sudo cp -r Insider/cosim/ /usr/insider/
sudo cp -r Insider/synthesis/ /usr/insider/
g++ Insider/src/insider_reset_syn.cpp -std=c++11 -O3 \
    -o insider_reset_syn
sudo mv insider_reset_syn /usr/bin
sudo cp Insider/shell/* /usr/bin
cd Insider/lib; ./compile.sh; 
sudo mv libinsider_runtime.so /usr/lib64;
cd ../..;

echo -e "\nadd_clang_subdirectory(s2s-kernel)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
echo "add_clang_subdirectory(s2s-interconnect)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
echo "add_clang_subdirectory(csim-interconnect)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
echo "add_clang_subdirectory(insider-app)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
echo "add_clang_subdirectory(insider-interconnect)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
echo "add_clang_subdirectory(insider-cosim-intc)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
cp -r Insider/llvm/* $LLVM_SRC_PATH/tools/clang/tools
cp -r STAccel/llvm/* $LLVM_SRC_PATH/tools/clang/tools
cd $LLVM_BUILD_PATH
cmake3 $LLVM_SRC_PATH
make -j2
sudo make install

cd $orig_path
