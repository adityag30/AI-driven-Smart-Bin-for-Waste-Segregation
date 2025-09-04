#include "esp_camera.h"
#include <WiFi.h>
#include <ESP32Servo.h>
#include "tflite-micro/tensorflow/lite/micro/all_ops_resolver.h"
#include "tflite-micro/tensorflow/lite/micro/micro_error_reporter.h"
#include "tflite-micro/tensorflow/lite/micro/micro_interpreter.h"
#include "tflite-micro/tensorflow/lite/schema/schema_generated.h"
#include "model.h" // TFLite model as C array

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// WiFi credentials (optional)
const char *ssid = "Galaxy S20 FE 5G E388";
const char *password = "yasl8311";

// Servo configuration
Servo myservo;
int servoPin = 15; // GPIO15 for servo signal

// TFLite globals
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter *error_reporter = &micro_error_reporter;
const tflite::Model *model = nullptr;
tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *input = nullptr;
TfLiteTensor *output = nullptr;
constexpr int kTensorArenaSize = 300 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_RGB565;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_QVGA;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Servo setup
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);
  myservo.write(0);

  // WiFi connection (optional)
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Initialize TFLite
  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema version mismatch");
    return;
  }

  static tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    Serial.println("AllocateTensors failed");
    return;
  }

  input = interpreter->input(0);
  output = interpreter->output(0);

  Serial.printf("Input tensor shape: [%d,%d,%d,%d]\n",
                input->dims->data[0], input->dims->data[1],
                input->dims->data[2], input->dims->data[3]);
  Serial.printf("Output tensor shape: [%d,%d]\n",
                output->dims->data[0], output->dims->data[1]);
}

void loop() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  preprocessImage(fb);

  if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("Inference failed");
    return;
  }

  float *output_data = output->data.f;
  int max_index = 0;
  float max_value = output_data[0];
  for (int i = 1; i < output->dims->data[1]; i++) {
    if (output_data[i] > max_value) {
      max_value = output_data[i];
      max_index = i;
    }
  }

  if (max_index == 0) {
    myservo.write(0);
    Serial.println("Detected: Organic, Servo to 0°");
  } else {
    myservo.write(90);
    Serial.println("Detected: Recyclable, Servo to 90°");
  }

  esp_camera_fb_return(fb);
  delay(500);
}

void preprocessImage(camera_fb_t *fb) {
  uint8_t *rgb888 = (uint8_t *)malloc(224 * 224 * 3);
  if (!rgb888) {
    Serial.println("Memory allocation failed");
    return;
  }

  int k = 0;
  for (int i = 0; i < fb->len; i += 2) {
    uint16_t rgb565 = (fb->buf[i] << 8) | fb->buf[i + 1];
    rgb888[k++] = (rgb565 >> 11) * 255 / 31; // Red
    rgb888[k++] = ((rgb565 >> 5) & 0x3F) * 255 / 63; // Green
    rgb888[k++] = (rgb565 & 0x1F) * 255 / 31; // Blue
  }

  for (int i = 0; i < 224 * 224 * 3; i++) {
    input->data.f[i] = rgb888[i] / 255.0f; // Normalize
  }

  free(rgb888);
}
