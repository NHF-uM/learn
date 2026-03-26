// 单个设备结构体，包含一个标志位
typedef struct {
    int id;
    unsigned char status_flag; // 例如：0x01 表示开，0x02 表示故障
} Device;

// 聚合结构体，统一管理多个设备的标志位
typedef struct {
    unsigned char device1_flag;
    unsigned char device2_flag;
} DeviceFlags;

// get/set 函数
unsigned char get_device_flag(Device *dev) {
    return dev->status_flag;
}

void set_device_flag(Device *dev, unsigned char flag) {
    dev->status_flag = flag;
}

// 将所有设备的标志位聚合到 DeviceFlags
void aggregate_flags(Device *dev1, Device *dev2, DeviceFlags *flags) {
    flags->device1_flag = get_device_flag(dev1);
    flags->device2_flag = get_device_flag(dev2);
}

// 批量设置设备标志位
void set_all_flags(Device *dev1, Device *dev2, DeviceFlags *flags) {
    set_device_flag(dev1, flags->device1_flag);
    set_device_flag(dev2, flags->device2_flag);
}

// 示例用法
int main() {
    Device dev1 = {1, 0x01};
    Device dev2 = {2, 0x02};
    DeviceFlags flags;

    // 聚合所有设备的标志位
    aggregate_flags(&dev1, &dev2, &flags);

    // 批量修改标志位
    flags.device1_flag = 0x03;
    flags.device2_flag = 0x00;
    set_all_flags(&dev1, &dev2, &flags);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 定义设备状态标志位结构体
struct DeviceStatus {
    unsigned int powerOn  : 1;  // 电源状态：1开启，0关闭
    unsigned int errorDetected  : 1;  // 错误标志：1有错误，0无错误
    unsigned int dataReady  : 1;  // 数据就绪：1就绪，0未就绪
    unsigned int mode  : 2;  // 操作模式：00普通/01节能/10高速/11预留
    unsigned int reserved  : 27;  // 保留位（共32位）
};
// 定义系统标志位聚合结构体
typedef struct {
    struct DeviceStatus sensor1;  // 传感器1状态
    struct DeviceStatus sensor2;  // 传感器2状态
    struct DeviceStatus actuator;  // 执行器状态
} SystemFlags;
// 初始化系统标志位
void initSystemFlags(SystemFlags *sf) {
    sf->sensor1.powerOn = 0;
    sf->sensor1.errorDetected = 0;
    sf->sensor1.dataReady = 0;
    sf->sensor1.mode = 0;
    
    sf->sensor2.powerOn = 0;
    sf->sensor2.errorDetected = 0;
    sf->sensor2.dataReady = 0;
    sf->sensor2.mode = 0;
    
    sf->actuator.powerOn = 0;
    sf->actuator.errorDetected = 0;
    sf->actuator.dataReady = 0;
    sf->actuator.mode = 0;
}

// 设置指定设备的标志位
void setDeviceFlag(SystemFlags *sf, int deviceIndex, int flagIndex, int value) {
    struct DeviceStatus *device;
    
    if (deviceIndex == 0) device = &sf->sensor1;
    else if (deviceIndex == 1) device = &sf->sensor2;
    else if (deviceIndex == 2) device = &sf->actuator;
    else return;
    
    switch (flagIndex) {
        case 0: device->powerOn = value; break;
        case 1: device->errorDetected = value; break;
        case 2: device->dataReady = value; break;
        case 3: device->mode = value; break;
        default: break;
    }
}

// 获取指定设备的标志位
int getDeviceFlag(const SystemFlags *sf, int deviceIndex, int flagIndex) {
    const struct DeviceStatus *device;
    
    if (deviceIndex == 0) device = &sf->sensor1;
    else if (deviceIndex == 1) device = &sf->sensor2;
    else if (deviceIndex == 2) device = &sf->actuator;
    else return 0;
    
    switch (flagIndex) {
        case 0: return device->powerOn;
        case 1: return device->errorDetected;
        case 2: return device->dataReady;
        case 3: return device->mode;
        default: return 0;
    }
}

// 更直观的接口函数
void setPowerOn(SystemFlags *sf, int deviceIndex, int value) {
    setDeviceFlag(sf, deviceIndex, 0, value);
}

int isPowerOn(const SystemFlags *sf, int deviceIndex) {
    return getDeviceFlag(sf, deviceIndex, 0);
}
int main() {
    SystemFlags systemFlags;
    
    // 初始化系统标志位
    initSystemFlags(&systemFlags);
    
    // 设置传感器1的电源状态为开启
    setPowerOn(&systemFlags, 0, 1);
    
    // 设置传感器2的错误标志
    setDeviceFlag(&systemFlags, 1, 1, 1);
    
    // 设置执行器的模式为节能模式（二进制01）
    setDeviceFlag(&systemFlags, 2, 3, 1);
    
    // 检查传感器1的电源状态
    if (isPowerOn(&systemFlags, 0)) {
        printf("传感器1已开启\n");
    }
    
    // 检查传感器2的错误标志
    if (getDeviceFlag(&systemFlags, 1, 1)) {
        printf("传感器2检测到错误\n");
    }
    
    // 使用聚合结构体直接访问
    systemFlags.sensor1.powerOn = 0;  // 关闭传感器1电源
    printf("传感器1电源状态: %s\n", systemFlags.sensor1.powerOn ? "开启" : "关闭");
    
    return 0;
}