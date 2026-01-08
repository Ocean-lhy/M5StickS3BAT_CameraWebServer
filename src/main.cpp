/*
  M5StackCoreS3_CameraWebServer
  Porting of espressif/arduino-esp32 example to M5Stack CoreS3 (GC0308)
  See also https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/Camera/CameraWebServer

  Author GOB https://twitter.com/gob_52_gob (*^○^*)
*/
#include <esp_camera.h>
#include <WiFi.h>
#include <gob_GC0308.hpp>

// --------------------------------
// Camera GC0308
// Pin settings
#define CAM_PIN_PWDN    39
#define CAM_PIN_RESET   41
#define CAM_PIN_XCLK    5
#define CAM_PIN_SIOD    48
#define CAM_PIN_SIOC    47
#define CAM_PIN_D7      40
#define CAM_PIN_D6      38
#define CAM_PIN_D5      12
#define CAM_PIN_D4      14
#define CAM_PIN_D3      15
#define CAM_PIN_D2      16
#define CAM_PIN_D1      18
#define CAM_PIN_D0      21
#define CAM_PIN_VSYNC   42
#define CAM_PIN_HREF    17
#define CAM_PIN_PCLK    13

extern void startCameraServer(); // app_httpd.cpp

// #define USING_EXISTING_I2C

#if defined(USING_EXISTING_I2C)
static camera_config_t camera_config =
{
    .pin_pwdn  = CAM_PIN_PWDN;
    .pin_reset = CAM_PIN_RESET;
    .pin_xclk = CAM_PIN_XCLK;
    //.pin_xclk = -1;
    //.pin_sccb_sda = CAM_PIN_SIOD;
    .pin_sccb_sda = -1;
    //.pin_sccb_scl = CAM_PIN_SIOC;
    .pin_sccb_scl = -1;

    .pin_d7 = CAM_PIN_D7;
    .pin_d6 = CAM_PIN_D6;
    .pin_d5 = CAM_PIN_D5;
    .pin_d4 = CAM_PIN_D4;
    .pin_d3 = CAM_PIN_D3;
    .pin_d2 = CAM_PIN_D2;
    .pin_d1 = CAM_PIN_D1;
    .pin_d0 = CAM_PIN_D0;
    .pin_vsync = CAM_PIN_VSYNC;
    .pin_href = CAM_PIN_HREF;
    .pin_pclk = CAM_PIN_PCLK;

    .xclk_freq_hz = 20000000;
    .ledc_timer = LEDC_TIMER_0;
    .ledc_channel = LEDC_CHANNEL_0;
    .fb_location  = CAMERA_FB_IN_PSRAM,
    .pixel_format = PIXFORMAT_RGB565;
    //.pixel_format = PIXFORMAT_YUV422;
    //.frame_size = FRAMESIZE_VGA; // 640x480
    //.frame_size = FRAMESIZE_QVGA; // 320x240
    //.frame_size = FRAMESIZE_240X240,  // 240x240
    //.frame_size = FRAMESIZE_QQVGA; // 160x120
    .jpeg_quality = 0,
    //.fb_count = 1;
    .fb_count = 2; // CPU Loads too much but faster
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    .sccb_i2c_port = M5.In_I2C.getPort()
};
#else
static camera_config_t camera_config =
{
    .pin_pwdn     = 39,
    .pin_reset    = 41,
    .pin_xclk     = 5,
    .pin_sscb_sda = 48,
    .pin_sscb_scl = 47,
    .pin_d7 = 40,
    .pin_d6 = 38,
    .pin_d5 = 12,
    .pin_d4 = 14,
    .pin_d3 = 15,
    .pin_d2 = 16,
    .pin_d1 = 18,
    .pin_d0 = 21,
    .pin_vsync = 42,
    .pin_href  = 17,
    .pin_pclk  = 13,
    .xclk_freq_hz = 20000000,
    .ledc_timer   = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_RGB565,
    //.pixel_format = PIXFORMAT_YUV422;
    //.frame_size   = FRAMESIZE_QQVGA,
    .frame_size   = FRAMESIZE_QVGA,
    // .frame_size   = FRAMESIZE_VGA,
    .jpeg_quality = 0,
    .fb_count     = 2,
    .fb_location  = CAMERA_FB_IN_PSRAM,
    .grab_mode    = CAMERA_GRAB_WHEN_EMPTY,
    .sccb_i2c_port = -1,
};
#endif

void setup()
{

#if defined(USING_EXISTING_I2C)
    camera_config.sccb_i2c_port = M5.In_I2C.getPort();
    esp_err_t err = esp_camera_init(&ccfg);
#else
    esp_err_t err = esp_camera_init(&camera_config);
#endif
    if (err != ESP_OK)
    {
        // M5.Display.clear(TFT_BLUE);
        ESP_LOGE( "setup", "Failed to init camera:%d", err);
        delay(1000 * 10);
        abort();
    }
    if(!goblib::camera::GC0308::complementDriver())
    {
        ESP_LOGE("setup", "Failed to complement GC0308");
    }

    // WiFi
    WiFi.begin("ocean", "oceanocean"); // or use it.
    WiFi.setSleep(false);

    int retry = 32;
    while(retry-- > 0 && WiFi.status() != WL_CONNECTED)
    {
        ESP_LOGI("setup", ".");
        delay(200);
    }
    if(WiFi.status() != WL_CONNECTED)
    {
        ESP_LOGE("setup", "Failed to connect WiFi");
        delay(1000);
        abort();
    }

    // Server
    startCameraServer();
    ESP_LOGI("setup", "Camera ready use: http://%s to connect", WiFi.localIP().toString().c_str());

}

void loop()
{
    //    M5_LOGI("%u", esp_get_free_heap_size());
    delay(10000);
}
