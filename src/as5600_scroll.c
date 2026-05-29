/*
 * Copyright (c) 2026 ClickTapToe contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT clicktaptoe_as5600_scroll

#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(clicktaptoe_as5600_scroll, CONFIG_CLICKTAPTOE_AS5600_SCROLL_LOG_LEVEL);

#define AS5600_RAW_ANGLE_REGISTER 0x0C
#define AS5600_STATUS_REGISTER 0x0B
#define AS5600_COUNTS_PER_REV 4096
#define AS5600_HALF_REV_COUNTS (AS5600_COUNTS_PER_REV / 2)
#define AS5600_FULL_ROTATION_DEGREES 360
#define AS5600_MICRO_UNITS 1000000
#define AS5600_MIN_REPORT_COUNTS DIV_ROUND_UP(AS5600_COUNTS_PER_REV, AS5600_FULL_ROTATION_DEGREES)

#define AS5600_STATUS_MH_BIT 3
#define AS5600_STATUS_ML_BIT 4
#define AS5600_STATUS_MD_BIT 5
#define AS5600_STATUS_MAGNET_MASK                                                                  \
    (BIT(AS5600_STATUS_MH_BIT) | BIT(AS5600_STATUS_ML_BIT) | BIT(AS5600_STATUS_MD_BIT))

struct as5600_scroll_config {
    struct i2c_dt_spec i2c;
    uint16_t poll_ms;
    uint16_t deadband;
    bool invert;
};

struct as5600_scroll_data {
    const struct device *dev;
    struct k_work_delayable poll_work;
    sensor_trigger_handler_t handler;
    const struct sensor_trigger *trigger;
    int16_t last_position;
    int32_t pending_counts;
    int last_error;
    uint8_t last_status;
    bool has_position;
    bool has_status;
    bool has_error;
};

static void as5600_scroll_record_error(struct as5600_scroll_data *data, const char *message,
                                       int err) {
    if (!data->has_error || data->last_error != err) {
        LOG_WRN("%s: %d", message, err);
    }

    data->has_error = true;
    data->last_error = err;
}

static int as5600_scroll_read_status(const struct device *dev, uint8_t *status) {
    const struct as5600_scroll_config *config = dev->config;

    return i2c_reg_read_byte_dt(&config->i2c, AS5600_STATUS_REGISTER, status);
}

static int as5600_scroll_check_status(const struct device *dev) {
    struct as5600_scroll_data *data = dev->data;
    uint8_t status;
    int err = as5600_scroll_read_status(dev, &status);

    if (err) {
        as5600_scroll_record_error(data, "Failed to read AS5600 status", err);
        return err;
    }

    uint8_t magnet_status = status & AS5600_STATUS_MAGNET_MASK;
    bool status_changed = !data->has_status || magnet_status != data->last_status;

    data->has_status = true;
    data->last_status = magnet_status;

    if (!(status & BIT(AS5600_STATUS_MD_BIT))) {
        if (status_changed) {
            LOG_WRN("AS5600 magnet not detected");
        }
        return -ENODATA;
    }

    if (status & BIT(AS5600_STATUS_MH_BIT)) {
        if (status_changed) {
            LOG_WRN("AS5600 magnet too strong");
        }
        return -ENODATA;
    }

    if (status & BIT(AS5600_STATUS_ML_BIT)) {
        if (status_changed) {
            LOG_WRN("AS5600 magnet too weak");
        }
        return -ENODATA;
    }

    return 0;
}

static int as5600_scroll_read_position(const struct device *dev, uint16_t *position) {
    const struct as5600_scroll_config *config = dev->config;
    uint8_t buffer[2] = {0};
    int err = i2c_burst_read_dt(&config->i2c, AS5600_RAW_ANGLE_REGISTER, buffer, sizeof(buffer));

    if (!err) {
        *position = sys_get_be16(buffer) & 0x0FFF;
    }

    return err;
}

static int as5600_scroll_update(const struct device *dev) {
    struct as5600_scroll_data *data = dev->data;
    const struct as5600_scroll_config *config = dev->config;
    uint16_t position;
    int err = as5600_scroll_check_status(dev);

    if (err) {
        data->has_position = false;
        return err;
    }

    err = as5600_scroll_read_position(dev, &position);
    if (err) {
        as5600_scroll_record_error(data, "Failed to read AS5600 angle", err);
        return err;
    }

    data->has_error = false;

    if (!data->has_position) {
        data->last_position = position;
        data->has_position = true;
        return 0;
    }

    int16_t delta = (int16_t)position - data->last_position;

    if (delta > AS5600_HALF_REV_COUNTS) {
        delta -= AS5600_COUNTS_PER_REV;
    } else if (delta < -AS5600_HALF_REV_COUNTS) {
        delta += AS5600_COUNTS_PER_REV;
    }

    data->last_position = position;

    if (delta == 0 || abs(delta) <= config->deadband) {
        return 0;
    }

    data->pending_counts += config->invert ? -delta : delta;

    LOG_DBG("position %u delta %d pending %d", position, delta, data->pending_counts);
    return 0;
}

static void as5600_scroll_poll(struct k_work *work) {
    struct k_work_delayable *delayable = k_work_delayable_from_work(work);
    struct as5600_scroll_data *data = CONTAINER_OF(delayable, struct as5600_scroll_data, poll_work);
    const struct device *dev = data->dev;
    const struct as5600_scroll_config *config = dev->config;
    int32_t pending_before = data->pending_counts;

    (void)as5600_scroll_update(dev);

    /* ZMK treats val1 == 0 rotation reports as legacy EC11 ticks, so only notify once the
     * accumulated AS5600 delta can be reported as at least one whole degree.
     */
    if (data->handler && abs(data->pending_counts) >= AS5600_MIN_REPORT_COUNTS &&
        data->pending_counts != pending_before) {
        data->handler(dev, data->trigger);
    }

    k_work_schedule(&data->poll_work, K_MSEC(config->poll_ms));
}

static int as5600_scroll_sample_fetch(const struct device *dev, enum sensor_channel chan) {
    if (chan != SENSOR_CHAN_ALL && chan != SENSOR_CHAN_ROTATION) {
        return -ENOTSUP;
    }

    return 0;
}

static int as5600_scroll_channel_get(const struct device *dev, enum sensor_channel chan,
                                     struct sensor_value *val) {
    struct as5600_scroll_data *data = dev->data;
    int32_t counts = data->pending_counts;

    if (chan != SENSOR_CHAN_ROTATION) {
        return -ENOTSUP;
    }

    data->pending_counts = 0;

    int64_t microdegrees =
        ((int64_t)counts * AS5600_FULL_ROTATION_DEGREES * AS5600_MICRO_UNITS) /
        AS5600_COUNTS_PER_REV;

    val->val1 = microdegrees / AS5600_MICRO_UNITS;
    val->val2 = microdegrees % AS5600_MICRO_UNITS;

    return 0;
}

static int as5600_scroll_trigger_set(const struct device *dev, const struct sensor_trigger *trigger,
                                     sensor_trigger_handler_t handler) {
    struct as5600_scroll_data *data = dev->data;

    if (trigger->type != SENSOR_TRIG_DATA_READY || trigger->chan != SENSOR_CHAN_ROTATION) {
        return -ENOTSUP;
    }

    data->trigger = trigger;
    data->handler = handler;

    return 0;
}

static const struct sensor_driver_api as5600_scroll_api = {
    .trigger_set = as5600_scroll_trigger_set,
    .sample_fetch = as5600_scroll_sample_fetch,
    .channel_get = as5600_scroll_channel_get,
};

static int as5600_scroll_init(const struct device *dev) {
    struct as5600_scroll_data *data = dev->data;
    const struct as5600_scroll_config *config = dev->config;

    if (!i2c_is_ready_dt(&config->i2c)) {
        LOG_ERR("I2C device is not ready");
        return -ENODEV;
    }

    data->dev = dev;
    k_work_init_delayable(&data->poll_work, as5600_scroll_poll);
    k_work_schedule(&data->poll_work, K_MSEC(config->poll_ms));

    return 0;
}

#define AS5600_SCROLL_DEFINE(n)                                                                    \
    static struct as5600_scroll_data as5600_scroll_data_##n;                                       \
    static const struct as5600_scroll_config as5600_scroll_config_##n = {                          \
        .i2c = I2C_DT_SPEC_INST_GET(n),                                                            \
        .poll_ms = DT_INST_PROP(n, poll_ms),                                                       \
        .deadband = DT_INST_PROP(n, deadband),                                                     \
        .invert = DT_INST_PROP(n, invert),                                                         \
    };                                                                                             \
    DEVICE_DT_INST_DEFINE(n, as5600_scroll_init, NULL, &as5600_scroll_data_##n,                    \
                          &as5600_scroll_config_##n, POST_KERNEL,                                  \
                          CONFIG_CLICKTAPTOE_AS5600_SCROLL_INIT_PRIORITY, &as5600_scroll_api);

DT_INST_FOREACH_STATUS_OKAY(AS5600_SCROLL_DEFINE)
