#ifndef CSYSDEV_HPP
#define CSYSDEV_HPP

enum PCIE_DEVICE
{
    PCIE_TPG0  = 0x0,
    PCIE_TPG1       ,
    PCIE_MAX_DEV_NUM,
};

enum PCIE_XDEV_INDEX
{
    PCIE_XDMA_USR = 0x0,
    PCIE_XDMA_H2C      ,
    PCIE_XDMA_C2H      ,
    PCIE_XDMA_MAX_NUM  ,
};

//TPG0
#define XDMA_TPG0_USR_CDEV         "/dev/xdma0_user"
#define XDMA_TPG0_H2C_CDEV         "/dev/xdma0_h2c_0"
#define XDMA_TPG0_C2H_CDEV         "/dev/xdma0_c2h_0"

//TPG1
#define XDMA_TPG1_USR_CDEV         "/dev/xdma1_user"
#define XDMA_TPG1_H2C_CDEV         "/dev/xdma1_h2c_0"
#define XDMA_TPG1_C2H_CDEV         "/dev/xdma1_c2h_0"

//TPG0/1
#define XDMA_TPG_MEM_SIZE         0x800FFFF	//0x602FFFF
#define XDMA_TPG_MEM_BASE         0x0000000

#endif // PCIEDEVLIST_HPP
