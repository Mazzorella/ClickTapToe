/*
 * Copyright (c) 2026 ClickTapToe contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/init.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include <zephyr/dt-bindings/input/input-event-codes.h>

#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/sensor_event.h>

LOG_MODULE_REGISTER(clicktaptoe_status_led, CONFIG_CLICKTAPTOE_STATUS_LED_LOG_LEVEL);

#define RED_LED_NODE DT_ALIAS(led0)
#define GREEN_LED_NODE DT_ALIAS(led1)
#define BLUE_LED_NODE DT_ALIAS(led2)

#define HAS_STATUS_LEDS                                                                           \
    (DT_NODE_HAS_STATUS(RED_LED_NODE, okay) && DT_NODE_HAS_STATUS(GREEN_LED_NODE, okay) &&         \
     DT_NODE_HAS_STATUS(BLUE_LED_NODE, okay))

#define AS5600_NODE DT_NODELABEL(scroll_as5600)
#define PMW3610_NODE DT_NODELABEL(pmw3610)
#define AS5600_STATUS_REGISTER 0x0B
#define PMW3610_PRODUCT_ID_REGISTER 0x00
#define PMW3610_PRODUCT_ID 0x3E
#define PMW3610_SPI_MODE (SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_MODE_CPOL | SPI_MODE_CPHA |   \
                          SPI_TRANSFER_MSB)

#if HAS_STATUS_LEDS

static const struct gpio_dt_spec red_led = GPIO_DT_SPEC_GET(RED_LED_NODE, gpios);
static const struct gpio_dt_spec green_led = GPIO_DT_SPEC_GET(GREEN_LED_NODE, gpios);
static const struct gpio_dt_spec blue_led = GPIO_DT_SPEC_GET(BLUE_LED_NODE, gpios);
#if DT_NODE_HAS_STATUS(AS5600_NODE, okay)
static const struct i2c_dt_spec as5600_i2c = I2C_DT_SPEC_GET(AS5600_NODE);
#endif
#if DT_NODE_HAS_STATUS(PMW3610_NODE, okay)
static const struct spi_dt_spec pmw3610_spi = SPI_DT_SPEC_GET(PMW3610_NODE, PMW3610_SPI_MODE, 0);
#endif

static struct k_work_delayable health_check_work;
static struct k_work_delayable error_blink_work;
static struct k_work_delayable activity_off_work;

static bool health_initialized;
static bool health_error;
static bool error_blink_on;

static int set_led(const struct gpio_dt_spec *led, bool on) {
    int err = gpio_pin_set_dt(led, on);

    if (err) {
        LOG_WRN("Failed to set status LED: %d", err);
    }

    return err;
}

static void set_all_leds_off(void) {
    (void)set_led(&red_led, false);
    (void)set_led(&green_led, false);
    (void)set_led(&blue_led, false);
}

static bool required_devices_ready(void) {
#if DT_NODE_HAS_STATUS(AS5600_NODE, okay)
    if (!i2c_is_ready_dt(&as5600_i2c)) {
        LOG_WRN("AS5600 I2C bus is not ready");
        return false;
    }

    uint8_t status;
    int err = i2c_reg_read_byte_dt(&as5600_i2c, AS5600_STATUS_REGISTER, &status);
    if (err) {
        LOG_WRN("AS5600 scroll device probe failed: %d", err);
        return false;
    }
#endif

#if DT_NODE_HAS_STATUS(PMW3610_NODE, okay)
    if (!spi_is_ready_dt(&pmw3610_spi)) {
        LOG_WRN("PMW3610 SPI bus is not ready");
        return false;
    }

    uint8_t reg = PMW3610_PRODUCT_ID_REGISTER;
    uint8_t product_id = 0;
    const struct spi_buf tx_buf = {.buf = &reg, .len = sizeof(reg)};
    const struct spi_buf_set tx = {.buffers = &tx_buf, .count = 1};
    struct spi_buf rx_buf[] = {
        {.buf = NULL, .len = sizeof(reg)},
        {.buf = &product_id, .len = sizeof(product_id)},
    };
    const struct spi_buf_set rx = {.buffers = rx_buf, .count = ARRAY_SIZE(rx_buf)};
    int pmw_err = spi_transceive_dt(&pmw3610_spi, &tx, &rx);
    if (pmw_err) {
        LOG_WRN("PMW3610 pointer device probe failed: %d", pmw_err);
        return false;
    }

    if (product_id != PMW3610_PRODUCT_ID) {
        LOG_WRN("PMW3610 pointer product id mismatch: 0x%02x", product_id);
        return false;
    }
#endif

    return true;
}

static void error_blink_handler(struct k_work *work) {
    if (!health_error) {
        error_blink_on = false;
        (void)set_led(&red_led, false);
        return;
    }

    error_blink_on = !error_blink_on;
    (void)set_led(&red_led, error_blink_on);

    k_work_schedule(&error_blink_work,
                    K_MSEC(CONFIG_CLICKTAPTOE_STATUS_LED_ERROR_BLINK_MS));
}

static void set_health_state(bool error) {
    if (health_initialized && error == health_error) {
        return;
    }

    health_initialized = true;
    health_error = error;

    if (error) {
        error_blink_on = false;
        (void)set_led(&green_led, false);
        k_work_reschedule(&error_blink_work, K_NO_WAIT);
    } else {
        (void)k_work_cancel_delayable(&error_blink_work);
        error_blink_on = false;
        (void)set_led(&red_led, false);
        (void)set_led(&green_led, true);
    }
}

static void health_check_handler(struct k_work *work) {
    set_health_state(!required_devices_ready());
    k_work_schedule(&health_check_work,
                    K_MSEC(CONFIG_CLICKTAPTOE_STATUS_LED_HEALTH_CHECK_MS));
}

static void activity_off_handler(struct k_work *work) { (void)set_led(&blue_led, false); }

static void pulse_activity_led(void) {
    (void)set_led(&blue_led, true);
    k_work_reschedule(&activity_off_work,
                      K_MSEC(CONFIG_CLICKTAPTOE_STATUS_LED_ACTIVITY_PULSE_MS));
}

static bool sensor_event_has_value(const struct zmk_sensor_event *event) {
    for (int i = 0; i < event->channel_data_size; i++) {
        struct sensor_value value = event->channel_data[i].value;

        if (value.val1 != 0 || value.val2 != 0) {
            return true;
        }
    }

    return false;
}

static int status_led_event_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *position_event = as_zmk_position_state_changed(eh);

    if (position_event != NULL) {
        pulse_activity_led();
        return ZMK_EV_EVENT_BUBBLE;
    }

    const struct zmk_sensor_event *sensor_event = as_zmk_sensor_event(eh);

    if (sensor_event != NULL && sensor_event_has_value(sensor_event)) {
        pulse_activity_led();
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(clicktaptoe_status_led, status_led_event_listener);
ZMK_SUBSCRIPTION(clicktaptoe_status_led, zmk_position_state_changed);
ZMK_SUBSCRIPTION(clicktaptoe_status_led, zmk_sensor_event);

#if DT_NODE_HAS_STATUS(PMW3610_NODE, okay)
static void pmw3610_input_callback(struct input_event *event, void *user_data) {
    if (event->type == INPUT_EV_REL && event->value != 0) {
        pulse_activity_led();
    }
}

INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(PMW3610_NODE), pmw3610_input_callback, NULL);
#endif

static int status_led_init(void) {
    if (!gpio_is_ready_dt(&red_led) || !gpio_is_ready_dt(&green_led) ||
        !gpio_is_ready_dt(&blue_led)) {
        LOG_WRN("Status LED GPIO device is not ready");
        return -ENODEV;
    }

    int err = gpio_pin_configure_dt(&red_led, GPIO_OUTPUT_INACTIVE);
    if (err) {
        return err;
    }

    err = gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_INACTIVE);
    if (err) {
        return err;
    }

    err = gpio_pin_configure_dt(&blue_led, GPIO_OUTPUT_INACTIVE);
    if (err) {
        return err;
    }

    set_all_leds_off();

    k_work_init_delayable(&health_check_work, health_check_handler);
    k_work_init_delayable(&error_blink_work, error_blink_handler);
    k_work_init_delayable(&activity_off_work, activity_off_handler);

    k_work_schedule(&health_check_work,
                    K_MSEC(CONFIG_CLICKTAPTOE_STATUS_LED_STARTUP_DELAY_MS));

    return 0;
}

#else

static int status_led_init(void) {
    LOG_WRN("XIAO status LED aliases led0/led1/led2 are not available");
    return 0;
}

#endif

SYS_INIT(status_led_init, APPLICATION, CONFIG_CLICKTAPTOE_STATUS_LED_INIT_PRIORITY);
