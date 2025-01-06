# HMI项目大纲

## 一、硬件平台介绍（-课时）

### **1.1 原理图**

![1729847963592](image/1729847963592.png)

### 1.2 实物展示

![1729848023144](image/LVGL项目（一）/1729848023144.png)

![1729848040747](image/LVGL项目（一）/1729848040747.png)![1729848102150](image/LVGL项目（一）/1729848102150.png)

### 1.3 功能讲解

## 二、软件环境介绍（-课时）

### 2.1 VSCODE

#### 2.1.1 下载

[Visual Studio Code - Code Editing. Redefined](https://code.visualstudio.com/)

#### 2.1.2 安装

#### 2.1.3 插件

名称: C/C++
ID: ms-vscode.cpptools
说明: C/C++ IntelliSense, debugging, and code browsing.
版本: 1.22.10
发布者: Microsoft
VS Marketplace 链接: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools

名称: Office Viewer(Markdown Editor)
ID: cweijan.vscode-office
说明: View word,excel files and using WYSIWYG editor for markdown.
版本: 3.4.3
发布者: Weijan Chen
VS Marketplace 链接: https://marketplace.visualstudio.com/items?itemName=cweijan.vscode-office

名称: Chinese (Simplified) (简体中文) Language Pack for Visual Studio Code
ID: MS-CEINTL.vscode-language-pack-zh-hans
说明: Language pack extension for Chinese (Simplified)
版本: 1.94.2024101609
发布者: Microsoft
VS Marketplace 链接: https://marketplace.visualstudio.com/items?itemName=MS-CEINTL.vscode-language-pack-zh-hans

名称: Bookmarks
ID: alefragnani.Bookmarks
说明: Mark lines and jump to them
版本: 13.5.0
发布者: Alessandro Fragnani
VS Marketplace 链接: https://marketplace.visualstudio.com/items?itemName=alefragnani.Bookmarks

### 2.2 GUI GUIDER

#### 2.2.1 下载

[GUI Guider | NXP 半导体](https://www.nxp.com.cn/design/design-center/software/development-software/gui-guider:GUI-GUIDER)

需要用户注册

#### 2.2.2 安装

### 2.3 CodeBlocks

Code::Blocks 是一款免费的、开源的、跨平台 C，C++ 和 Fortran IDE，旨在满足用户最高要求。它具有很高的扩展性和完全可配置的插件功能。

#### 2.3.1 下载

https://www.codeblocks.org/downloads/

如果自己电脑没有配置MINGW选择codeblocks-20.03mingw-setup.exe

## 三、ESP32开发环境搭建（-课时）

### 3.1 [Windows](https://gitforwindows.org/)开发环境

[Windows 平台工具链的标准设置 - ESP32-S3 - — ESP-IDF 编程指南 latest 文档](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32s3/get-started/windows-setup.html)

#### 3.1.1 VSCODE中安装IDF插件

创建IDF的存储目录，注意不要出现中文和空格等特殊字符

## 四、LVGL虚拟环境搭建（-课时）

### 4.1 codeblocks模拟器搭建

#### 4.1.1 拉取codeblocks模拟器工程代码

[Git for Windows](https://gitforwindows.org/)

[lv_port_win_codeblocks](https://github.com/lvgl/lv_port_win_codeblocks)

`git clone --recurse-submodules https://github.com/lvgl/lv_port_win_codeblocks.git`

#### 4.1.2 环境验证

需要注意，更新mingw版本，否则使用codeblocks自带的mingw编译会报错

![1730185763159](image/LVGL项目（一）/1730185763159.png)

链接异常：

![1730188688941](image/LVGL项目（一）/1730188688941.png)

[Code::Blocks set use flat objects - How-to - LVGL Forum](https://forum.lvgl.io/t/code-blocks-set-use-flat-objects/16636)

### 4.2 vscode模拟器搭建

#### 4.2.1 拉取vscode 模拟器工程

`git clone --recursive https://github.com/lvgl/lv_port_pc_vscode`

#### 4.2.2 安装插件

![1730883608094](image/LVGL项目（一）/1730883608094.png)

#### 4.2.3安装mingw

[Releases · mstorsjo/llvm-mingw (github.com)](https://github.com/mstorsjo/llvm-mingw/releases)

![1730876912120](image/LVGL项目（一）/1730876912120.png)

下载后解压，并添加到环境变量，例如：

![1730881446824](image/LVGL项目（一）/1730881446824.png)

#### 4.2.4 安装sdl

[Releases · libsdl-org/SDL](https://github.com/libsdl-org/SDL/releases)

![1730881069629](image/LVGL项目（一）/1730881069629.png)

#### 4.2.5 安装cmake

[Download CMake](https://cmake.org/download/)

![1730876992056](image/LVGL项目（一）/1730876992056.png)

#### 4.2.6 错误处理

![1730903273138](image/LVGL项目（一）/1730903273138.png)

![1730903430660](image/LVGL项目（一）/1730903430660.png)

![1730903767111](image/LVGL项目（一）/1730903767111.png)

![1730903823348](image/LVGL项目（一）/1730903823348.png)

![1730903876249](image/LVGL项目（一）/1730903876249.png)

**参考**

[基于vscode搭建LVGL环境 - USTHzhanglu - 博客园](https://www.cnblogs.com/USTHzhanglu/p/18348535)

## 五、LVGL移植到ESP32

### 5.1 基于rgb_panel建立工程

工作目录下打开VSCODE

![1730428545837](image/LVGL项目（一）/1730428545837.png)

### 5.2 添加lvgl

![1730428632902](image/LVGL项目（一）/1730428632902.png)

### 5.3 编译错误解决

![1730430256775](image/LVGL项目（一）/1730430256775.png)

### 5.4 功能确认

#### 5.4.1 初始化

参考LVGL模版或者ESP文档

managed_components\lvgl__lvgl\examples\porting\lv_port_disp_template.c

managed_components\lvgl__lvgl\docs\integration\chip\espressif.rst

#### 5.4.1 hello world

注意：使用CONFIG_LV_USE_BUILTIN_MALLOC，需要将lv_init()放在lv_display_create之前，先完成lv_mem的初始化

![1730444750600](image/LVGL项目（一）/1730444750600.png)

## 六、驱动完善

### 6.1 触摸

原理图

![1730554230954](image/LVGL项目（一）/1730554230954.png)

![1730618287449](image/LVGL项目（一）/1730618287449.png)

#### 6.1.1 添加GT911组件

![1730617861463](image/LVGL项目（一）/1730617861463.png)

![1730618106142](image/LVGL项目（一）/1730618106142.png)

#### 6.1.2 添加GT911初始化

参考examples\peripherals\lcd\i80_controller\main\i80_controller_example_main.c

`example_init_lcd_touch`

#### 6.1.3 测试触摸读取

```c
static void example_touch_read_task(void *arg)
{
    ESP_LOGI(TAG, "Starting example_touch_read_task task");
    uint16_t touch_x[1];
    uint16_t touch_y[1];
    uint16_t touch_last_x = 0;
    uint16_t touch_last_y = 0;
    uint16_t touch_strength[1];
    uint8_t touch_cnt = 0;
    uint32_t task_delay_ms = 100;
    esp_lcd_touch_handle_t tp = (esp_lcd_touch_handle_t)arg;
    while (1) {
        esp_lcd_touch_read_data(tp);
        bool touchpad_pressed = esp_lcd_touch_get_coordinates(tp, touch_x, touch_y, touch_strength, &touch_cnt, 1);
        if ((touch_x[0] != touch_last_x) || (touch_y[0] != touch_last_y))
        {
            ESP_LOGI(TAG, "Touch coordinate:[%d],[%d]", touch_x[0], touch_y[0]);
        }

        touch_last_x = touch_x[0];
        touch_last_y = touch_y[0];
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}
```

### 6.2 SD卡

使用spi模式

原理图

![1730689501867](image/LVGL项目（一）/1730689501867.png)

#### 6.2.1 sdspi组件

参考examples\storage\sd_card\sdspi\main\sd_card_example_main.c

#### 6.2.2 测试

### 6.3 debug

#### 6.3.1 触摸初始化失败，多次重启OK

```shell
I (1227) gpio: GPIO[38]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
W (1237) GT911: Unable to initialize the I2C address
E (1267) lcd_panel.io.i2c: panel_io_i2c_rx_buffer(136): i2c transaction failed
E (1267) GT911: touch_gt911_read_cfg(390): GT911 read error!
E (1267) GT911: esp_lcd_touch_new_i2c_gt911(159): GT911 init failed
E (1277) GT911: Error (0xffffffff)! Touch controller GT911 initialization failed!
I (1277) gpio: GPIO[38]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
ESP_ERROR_CHECK failed: esp_err_t 0xffffffff (ESP_FAIL) at 0x4200a0d1
--- 0x4200a0d1: example_init_lcd_touch at D:/esp-idf/test/rgb_test/rgb_panel/main/rgb_lcd_example_main.c:384 (discriminator 1)

```

* 更新i2c driver

  ```c
      i2c_master_bus_config_t i2c_bus_config = {
          .clk_source = I2C_CLK_SRC_DEFAULT,
          .i2c_port = EXAMPLE_I2C_NUM,
          .scl_io_num = EXAMPLE_I2C_SCL,
          .sda_io_num = EXAMPLE_I2C_SDA,
          .glitch_ignore_cnt = 7,
      };
      i2c_master_bus_handle_t bus_handle;

      ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));
  ```
* 增加初始化错误处理

  ```c
      for (int i = 0; i < 10; i++) {
          ret = esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, &tp);
          if (ret == ESP_OK) {
              ESP_LOGI(TAG, "Initialize touch controller GT911 OK!");
              break;
          }
          vTaskDelay(pdMS_TO_TICKS(200));
      }
  ```

## 七、软件功能

### 7.1 ui布局

#### 7.1.1 模拟器上创建界面

**绘制草图**

![1730962501055](image/LVGL项目（一）/1730962501055.png)

**添加输入框**

参考 `lvgl\examples\widgets\textarea\lv_example_textarea_2.c`

**添加预览按钮**

参考 `lvgl\examples\widgets\button\lv_example_button_1.c`

**添加渲染区**

参考 `lvgl\examples\widgets\canvas\lv_example_canvas_7.c`

**添加中文输入**

参考 `lvgl\examples\others\ime\lv_example_ime_pinyin_1.c`

![1732718634682](image/LVGL项目（一）/1732718634682.png)

### 7.2 CAD字体文件解析

#### 7.2.1 文件系统添加

模拟器端lv_confi.h需要修改的地方

![1732719419152](image/LVGL项目（一）/1732719419152.png)

### 7.3 CAD字体渲染

![1733064308371](image/LVGL项目（一）/1733064308371.png)

31.shx文件是由.shp文件编译生成，这里我们用到的是大字体（适用中文），文件中描述字体的坐标数据前面会有一个gbk码（2byte）

在ui界面输入的中文字符，比如“芯”存储的编码值是utf8，utf8是对unicode码的一种编码格式，每一个字符都对应唯一的unicode码

需要先对utf8解码出对应的unicode 码，再使用unicode码从文件中找到对应的gbk码，这里使用的文件为utf16.bin

关于 shx（shp）代码解析

[AutoCAD 2024 Developer and ObjectARX 帮助 | 特殊代码参考 | Autodesk](https://help.autodesk.com/view/OARX/2024/CHS/?guid=GUID-06832147-16BE-4A66-A6D0-3ADF98DC8228)

关于shx文件格式

[AutoCAD逆向工程中Shx字体文件解析 - 豆丁网](https://www.docin.com/p-1491617102.html)

关于utf8转换gbk

[C语言实现中文BGK、UNICODE、UTF8之间的编码转换_c语言汉字编码转换-CSDN博客](https://blog.csdn.net/Ranchaun/article/details/109262435)

[ASCII码、ISO8859-1、Unicode、GBK和UTF-8 的区别 - panchanggui - 博客园](https://www.cnblogs.com/panchanggui/p/9370629.html)

shx解析器

[github.com](https://github.com/tatarize/shxparser)

## 八、综合示例

### 8.1 移植ui布局到esp32

遗留问题：键盘输入无效

### 8.2 移植字体解析到esp32

解决键盘输入的问题，注意lvgl v9 输入驱动的注册和v8版本不一样

参考：managed_components\lvgl__lvgl\examples\porting\lv_port_indev_template.c

还要添加userdata

```c
    indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchpad, example_lvgl_touch_cb);
    lv_indev_set_user_data(indev_touchpad,tp);
```

遗留问题：绘制的时候出现异常，看门狗复位

```shell
E (46842) task_wdt:  - IDLE0 (CPU 0)
E (46842) task_wdt: Tasks currently running:
E (46842) task_wdt: CPU 0: main
E (46842) task_wdt: CPU 1: IDLE1
E (46842) task_wdt: Print CPU 0 (current core) backtrace


Backtrace: 0x4204714F:0x3FC99230 0x4204756C:0x3FC99250 0x40377C61:0x3FC99280 0x42015B76:0x3FCD8AB0 0x420113AA:0x3FCD8AF0 0x420113E1:0x3FCD8B20 0x420262B5:0x3FCD8B50 0x42009ACE:0x3FCD8B70 0x4200A333:0x3FCD8BB0 0x4200A53B:0x3FCD8C30 0x4200A5FD:0x3FCD8C80 0x4200A69F:0x3FCD8CA0 0x4200AD71:0x3FCD8CD0 0x42066B5F:0x3FCD8DE0 0x4037EF51:0x3FCD8E10
--- 0x4204714f: task_wdt_timeout_handling at D:/esp-idf/v5.3.1/esp-idf/components/esp_system/task_wdt/task_wdt.c:434
0x4204756c: task_wdt_isr at D:/esp-idf/v5.3.1/esp-idf/components/esp_system/task_wdt/task_wdt.c:507
0x40377c61: _xt_lowint1 at D:/esp-idf/v5.3.1/esp-idf/components/xtensa/xtensa_vectors.S:1240
0x42015b76: lv_inv_area at D:/esp-idf/test/rgb_test2/rgb_panel/managed_components/lvgl__lvgl/src/core/lv_refr.c:274
0x420113aa: lv_obj_invalidate_area at D:/esp-idf/test/rgb_test2/rgb_panel/managed_components/lvgl__lvgl/src/core/lv_obj_pos.c:838 (discriminator 1)
0x420113e1: lv_obj_invalidate at D:/esp-idf/test/rgb_test2/rgb_panel/managed_components/lvgl__lvgl/src/core/lv_obj_pos.c:854
0x420262b5: lv_canvas_finish_layer at D:/esp-idf/test/rgb_test2/rgb_panel/managed_components/lvgl__lvgl/src/widgets/canvas/lv_canvas.c:406
0x42009ace: pen_draw_line at D:/esp-idf/test/rgb_test2/rgb_panel/co
```

### 8.3 移植字体渲染到esp32

解决遗留问题，画布绘制异常
