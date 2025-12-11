# 智能通风与安全监控系统

基于STM32F103C8T6 + ESP8266 + ESP32的物联网智能家居项目

## 📋 项目简介

这是一个智能通风与气体安全监控系统，能够实时监测室内多种有害气体浓度、温湿度，并根据环境状况自动控制通风设备。系统支持本地控制和远程MQTT物联网控制。

### 核心功能

- ✅ 多气体实时监测（甲烷、烟雾、一氧化碳）
- ✅ 温湿度环境监控
- ✅ 智能安全联动（气体超标自动报警+通风）
- ✅ 自动/手动双模式控制
- ✅ MQTT远程控制与数据上报
- ✅ 本地Web控制面板

------

## 🎯 系统架构

```
┌─────────────────────┐
│  STM32F103C8T6      │ ← 主控制器
│  ├─ MQ-4 (甲烷)     │
│  ├─ MQ-2 (烟雾)     │
│  ├─ MQ-7 (一氧化碳) │
│  ├─ DHT11 (温湿度)  │
│  ├─ 继电器 (窗户)   │
│  ├─ TB6612 (风扇)   │
│  ├─ 蜂鸣器 (报警)   │
│  └─ LED + 按键      │
└──────────┬──────────┘
           │ UART
           ↓
┌─────────────────────┐
│  ESP8266            │ ← WiFi网关
│  ├─ WiFi连接        │
│  ├─ MQTT客户端      │
│  └─ 数据转发        │
└──────────┬──────────┘
           │ MQTT
           ↓
┌─────────────────────┐
│  云平台 / 手机APP   │
└─────────────────────┘

┌─────────────────────┐
│  ESP32-WROOM        │ ← 高级功能节点
│  ├─ 本地Web服务器   │
│  ├─ 蓝牙控制        │
│  └─ 独立MQTT节点    │
└─────────────────────┘
```

------

## 🔧 硬件清单

### 主控单元

| 组件     | 型号                 | 数量 | 说明                 |
| -------- | -------------------- | ---- | -------------------- |
| 主控MCU  | STM32F103C8T6 (蓝丸) | 1    | ARM Cortex-M3, 72MHz |
| WiFi模块 | ESP8266              | 1    | 2.4G WiFi, UART通信  |
| 高级节点 | ESP32-WROOM          | 1    | 双核, WiFi+蓝牙      |

### 传感器模块

| 组件         | 型号  | 数量 | 功能              |
| ------------ | ----- | ---- | ----------------- |
| 甲烷传感器   | MQ-4  | 1    | 检测天然气泄漏    |
| 烟雾传感器   | MQ-2  | 1    | 检测烟雾/可燃气体 |
| CO传感器     | MQ-7  | 1    | 检测一氧化碳      |
| 温湿度传感器 | DHT11 | 1    | 温度+湿度监测     |

### 执行器模块

| 组件       | 型号         | 数量 | 功能         |
| ---------- | ------------ | ---- | ------------ |
| 继电器模块 | 5V单路继电器 | 1    | 控制窗户开关 |
| 电机驱动   | TB6612FNG    | 1    | 驱动直流风扇 |
| 直流电机   | 3-6V直流电机 | 1    | 模拟排气风扇 |
| 蜂鸣器     | 有源蜂鸣器   | 1    | 危险报警     |

### 其他组件

- LED指示灯 × 1 (板载PC13)
- 按键 × 1 (模式切换)
- 面包板、杜邦线若干

------

## 📌 引脚定义 (STM32F103C8T6)

| 功能         | 引脚 | 类型     | 说明          |
| ------------ | ---- | -------- | ------------- |
| **传感器**   |      |          |               |
| MQ-4模拟输出 | PA0  | ADC1_IN0 | 甲烷传感器    |
| MQ-2模拟输出 | PA1  | ADC1_IN1 | 烟雾传感器    |
| MQ-7模拟输出 | PA2  | ADC1_IN2 | CO传感器      |
| DHT11数据线  | PA3  | GPIO     | 单总线通信    |
| **执行器**   |      |          |               |
| 继电器控制   | PA4  | GPIO_OUT | 高电平开窗    |
| 蜂鸣器       | PA5  | GPIO_OUT | 高电平报警    |
| **人机交互** |      |          |               |
| 状态LED      | PC13 | GPIO_OUT | 板载LED       |
| 模式按键     | PA7  | GPIO_IN  | 内部上拉      |
| **电机驱动** |      |          |               |
| TB6612_PWMA  | PB6  | TIM4_CH1 | PWM调速       |
| TB6612_AIN1  | PB7  | GPIO_OUT | 方向控制1     |
| TB6612_AIN2  | PB8  | GPIO_OUT | 方向控制2     |
| TB6612_STBY  | PB9  | GPIO_OUT | 使能信号      |
| **通信接口** |      |          |               |
| USART1_TX    | PA9  | UART     | 与ESP8266通信 |
| USART1_RX    | PA10 | UART     | 与ESP8266通信 |
| **调试接口** |      |          |               |
| SWDIO        | PA13 | SWD      | 调试/下载     |
| SWCLK        | PA14 | SWD      | 调试/下载     |

------

## 📂 项目结构

```
SmartVentilation/
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   ├── config.h              # 全局配置参数
│   │   └── stm32f1xx_hal_conf.h
│   └── Src/
│       ├── main.c                # 主程序
│       ├── stm32f1xx_it.c        # 中断处理
│       └── system_stm32f1xx.c
├── Drivers/                      # 底层驱动
│   ├── adc_driver.c/h           # ✅ ADC + MQ传感器驱动
│   ├── dht11.c/h                # DHT11温湿度传感器
│   ├── filter.c/h               # 数据滤波算法
│   ├── relay.c/h                # 继电器控制
│   ├── buzzer.c/h               # 蜂鸣器控制
│   ├── motor.c/h                # TB6612电机驱动
│   ├── button.c/h               # 按键检测
│   └── led.c/h                  # LED指示灯
├── App/                          # 应用层
│   ├── safety_logic.c/h         # 安全联动逻辑
│   ├── mode_control.c/h         # 模式切换
│   └── data_process.c/h         # 数据处理
├── Protocol/                     # 通信协议
│   ├── uart_protocol.c/h        # 串口协议
│   └── json_pack.c/h            # JSON数据封装
├── ESP8266/                      # ESP8266代码
│   └── mqtt_gateway/            # MQTT网关程序
├── ESP32/                        # ESP32代码
│   └── web_server/              # Web控制面板
├── Docs/                         # 文档
│   ├── hardware_design.md       # 硬件设计文档
│   ├── software_architecture.md # 软件架构说明
│   └── api_reference.md         # API接口文档
└── README.md
```

------

## 🚀 开发进度

### ✅ 已完成

- [x] 

  Step 1

  : 基础工程搭建

  - [x] STM32CubeMX配置
  - [x] 时钟配置 (72MHz)
  - [x] LED闪烁测试

- [x] 

  Step 2.1

  : ADC配置

  - [x] ADC1三通道配置 (MQ4/MQ2/MQ7)
  - [x] ADC驱动程序
  - [x] 原始数据采集
  - [x] PPM浓度转换

### 🔄 进行中

- [ ] **Step 2.2**: DHT11温湿度传感器
- [ ] **Step 2.3**: 数据滤波算法

### 📅 待开发

- [ ] 

  Step 3

  : 执行器驱动

  - [ ] 继电器控制
  - [ ] 蜂鸣器控制
  - [ ] TB6612电机驱动

- [ ] 

  Step 4

  : 人机交互

  - [ ] 按键检测
  - [ ] LED状态指示

- [ ] 

  Step 5

  : 应用逻辑

  - [ ] 安全联动算法
  - [ ] 模式切换

- [ ] 

  Step 6

  : 通信协议

  - [ ] 串口配置
  - [ ] JSON封装

- [ ] **Step 7**: ESP8266 MQTT网关

- [ ] **Step 8**: ESP32 Web服务器

- [ ] **Step 9**: 系统集成测试

------

## 🛠️ 开发环境

### STM32开发

- **IDE**: Keil MDK-ARM V5
- **配置工具**: STM32CubeMX 6.x
- **编程语言**: C (HAL库)
- **下载工具**: ST-Link V2
- **调试接口**: SWD

### ESP8266开发

- **IDE**: Arduino IDE / PlatformIO
- **开发框架**: Arduino Core for ESP8266
- **主要库**: PubSubClient (MQTT), ArduinoJson

### ESP32开发

- **IDE**: Arduino IDE / PlatformIO
- **开发框架**: ESP-IDF / Arduino
- **主要库**: WebServer, WiFi, BluetoothSerial

------

## 📖 快速开始

### 1. 克隆项目

```bash
git clone https://github.com/xylx521/SmartEnvironmentalSystem.git
cd SmartEnvironmentalSystem
```

### 2. STM32编译与下载

```bash
# 使用STM32CubeMX打开 .ioc 文件
# 或直接在Keil中打开 MDK-ARM/SmartVentilation.uvprojx
# 编译后使用ST-Link下载
```

### 3. ESP8266配置

```bash
# 修改WiFi配置
# 修改MQTT服务器地址
# 使用Arduino IDE上传
```

### 4. ESP32配置

```bash
# 配置WiFi和Web服务器
# 上传程序
```

------

## 🧪 测试说明

### MQ传感器预热

- **首次使用**: 需要连续通电48小时预热

- **日常使用**: 每次上电预热3-5分钟

- 测试方法

  :

  - MQ-4: 打火机靠近（不点燃）
  - MQ-2: 烟雾/香烟烟雾
  - MQ-7: 香烟烟雾

### DHT11测试

- 对着传感器哈气，观察湿度上升
- 用手指捂住传感器，观察温度上升

### 执行器测试

- 继电器: 听到"咔哒"声
- 电机: 观察风扇转速变化
- 蜂鸣器: 发出"滴滴"声

------

## 📡 通信协议

### STM32 → ESP8266 (UART, 115200, 8N1)

**数据上报 (JSON格式):**

```json
{
  "mq4": 350,
  "mq2": 280,
  "mq7": 120,
  "temp": 25.6,
  "humi": 65,
  "window": 1,
  "fan_speed": 80,
  "alarm": 0
}
```

### ESP8266 → MQTT服务器

**Topic**: `smart_home/sensors/data` **Payload**: 同上JSON数据

### MQTT → ESP8266 → STM32

**Topic**: `smart_home/control/command` **控制指令:**

```json
{
  "cmd": "window",
  "value": 1
}
```

------

## ⚠️ 安全阈值配置

| 气体       | 正常值    | 警告值      | 危险值    | 处理动作           |
| ---------- | --------- | ----------- | --------- | ------------------ |
| 甲烷(MQ-4) | < 300 ppm | 300-500 ppm | > 500 ppm | 开窗+风扇高速      |
| 烟雾(MQ-2) | < 200 ppm | 200-400 ppm | > 400 ppm | 关窗+风扇停止+报警 |
| CO(MQ-7)   | < 50 ppm  | 50-100 ppm  | > 100 ppm | 开窗+风扇高速+报警 |
| 温度       | 15-28°C   | 28-35°C     | > 35°C    | 开窗+风扇中速      |
| 湿度       | 40-70%    | 70-85%      | > 85%     | 风扇中速除湿       |

------

## 🤝 贡献指南

欢迎提交Issue和Pull Request！

### 开发规范

- 代码风格: 遵循Linux内核编码风格
- 提交信息: 使用中文，格式 `[模块] 简短描述`
- 文档更新: 重要功能需同步更新README

------

## 📄 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](https://claude.ai/chat/LICENSE) 文件

------

## 📮 联系方式

- **项目主页**: https://github.com/xylx521/SmartEnvironmentalSystem
- **问题反馈**: https://github.com/xylx521/SmartEnvironmentalSystem/issues

------

## 🙏 致谢

- STMicroelectronics - STM32CubeMX
- Espressif - ESP8266/ESP32开发框架
- Arduino社区 - 丰富的开源库

------

## 📊 项目统计

![GitHub stars](https://img.shields.io/github/stars/xylx521/SmartEnvironmentalSystem?style=social) ![GitHub forks](https://img.shields.io/github/forks/xylx521/SmartEnvironmentalSystem?style=social) ![GitHub issues](https://img.shields.io/github/issues/xylx521/SmartEnvironmentalSystem) ![GitHub license](https://img.shields.io/github/license/xylx521/SmartEnvironmentalSystem)

------

**⭐ 如果这个项目对你有帮助，请给个Star支持一下！**