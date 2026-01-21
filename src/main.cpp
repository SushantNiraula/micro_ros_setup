#include <Arduino.h>

#include <micro_ros_platformio.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/string.h>

static const int LED_PIN = 2;

rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;

rcl_publisher_t pub;
rcl_subscription_t sub;

rcl_timer_t timer;
rclc_executor_t executor;

std_msgs__msg__String pub_msg;
std_msgs__msg__Int32 sub_msg;

char msg_buffer[64];
int counter = 0;

// ---------------- Subscriber callback ----------------
void led_callback(const void * msgin)
{
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;

  if (msg->data == 1) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

// ---------------- Timer callback ----------------
void timer_callback(rcl_timer_t * timer_handle, int64_t last_call_time)
{
  (void) last_call_time;
  if (timer_handle == NULL) return;

  counter++;

  // Fill string message safely
  snprintf(msg_buffer, sizeof(msg_buffer), "Hello from ESP32 micro-ROS. Count=%d", counter);

  pub_msg.data.data = msg_buffer;
  pub_msg.data.size = strlen(msg_buffer);
  pub_msg.data.capacity = sizeof(msg_buffer);

  rcl_publish(&pub, &pub_msg, NULL);
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  delay(1500);

  // micro-ROS serial transport
  set_microros_serial_transports(Serial);

  allocator = rcl_get_default_allocator();

  // Support + node
  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "esp32_dummy_node", "", &support);

  // Publisher: /esp32/chatter
  rclc_publisher_init_default(
    &pub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "/esp32/chatter"
  );

  // Subscriber: /esp32/led
  rclc_subscription_init_default(
    &sub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "/esp32/led"
  );

  // Timer: publish at 2Hz (every 500ms)
  rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(500),
    timer_callback
  );

  // Executor: 1 sub + 1 timer = 2 handles
  rclc_executor_init(&executor, &support.context, 2, &allocator);
  rclc_executor_add_subscription(&executor, &sub, &sub_msg, &led_callback, ON_NEW_DATA);
  rclc_executor_add_timer(&executor, &timer);

  // Init message structs
  std_msgs__msg__String__init(&pub_msg);
  std_msgs__msg__Int32__init(&sub_msg);
}

void loop()
{
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
}
