/* 
 * Android SPEAr test battery driver.
 * Based on goldfish_battery.c
 * Author: Vincenzo Frascino <vincenzo.frascino@st.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/interrupt.h>
#include <linux/spear_adc.h>

struct spear_battery_pdata_t {
	int adc_volt;
	int adc_current;
	int delay; /* loop inteval of update volt data */
	int st1; /* status pin 1 of ld6924 */
	int st2; /* status pin 2 of ld6924 */
};

static struct spear_battery_pdata_t spear_battery_pdata = {
	.adc_current	= ADC_CHANNEL0,
	.adc_volt	= ADC_CHANNEL1,
	.delay		= 30 * HZ, /* sec */
	.st1		= GPIO0_7,
	.st2		= PLGPIO_32,
};

static struct platform_device *battery_pdev;
static struct spear_battery_pdata_t *battery_pdata = &spear_battery_pdata;
static struct delayed_work battery_dwork;
static int old_dc_status = 0;

static int spear_battery_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val);

static enum power_supply_property spear_battery_battery_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_CAPACITY,
};

static struct power_supply spear_battery_bat = {
	.name = "battery",
	.type = POWER_SUPPLY_TYPE_BATTERY,
	.properties 	=  spear_battery_battery_props,
	.num_properties = ARRAY_SIZE(spear_battery_battery_props),
	.get_property	= spear_battery_get_property,
	.use_for_apm 	= 1,
};

/**
 * init status pin of charger chip
 */
static int spear_battery_gpio_init(struct platform_device *pdev, int gpio)
{
	int ret;

	ret = gpio_request(gpio, "battery");
	if (ret) {
		dev_err(&pdev->dev, "couldn't req gpio: %d\n", ret);
		return ret;
	}

	ret = gpio_direction_input(gpio);
	if (ret) {
		dev_err(&pdev->dev, "couldn't set gpio direction: %d\n", ret);
		goto free_gpio;
	}

	return 0;

free_gpio:
	gpio_free(gpio);
	return ret;
}

/**
 * adc return volt value of pin.
 */
static inline uint spear_battery_adc_value(int chan_id)
{
	uint adc_val;
	spear_adc_get_data(battery_pdev, chan_id, &adc_val, 1);
	return adc_val;
}

static int spear_charger_status(void)
{
	int st1, st2;

	st1 = gpio_get_value_cansleep(battery_pdata->st1);
	st2 = gpio_get_value_cansleep(battery_pdata->st2);

	printk("Battery debug: st1 = %d, st2 = %d\n", st1, st2);

	if (st1 == 0 && st2 == 1) { /* st1: on, st2: off, charge in progress */
		return POWER_SUPPLY_STATUS_CHARGING;
	} else if (st1 == 1 && st2 == 0) { /* st1: off, st2: on, charge done */
		return POWER_SUPPLY_STATUS_FULL;
	} else { /* st1: off, st2: off, discharging (stand by mode in doc) */
		return POWER_SUPPLY_STATUS_DISCHARGING;
	}
}

static int spear_battery_calc_level(int value, int v_max, int v_min, int l_max, int l_min)
{
	int level;

	dev_info(&battery_pdev->dev, "value=%d, v_max=%d, v_min=%d, l_max=%d, l_min=%d\n",
					value, v_max, v_min, l_max, l_min);
	value = value > v_max ? v_max : value;
	value = value < v_min ? v_min : value;
	level = (value - v_min) * 100 / (v_max - v_min);
	level = (l_max - l_min) * level / 100 + l_min;
	dev_info(&battery_pdev->dev, "level=%d\n", level);

	return level;
}

/**
 * currently just use a simple alogrithm for calc power level.
 */
static uint spear_battery_get_capacity(void)
{
	int adc_volt, adc_curr;
	int level;
	int dc_status;
	static int max_volt = 3980;
	static int min_volt = 2500;
	static int base_level = 100;
	static int old_level= 100;
	static int div = 90;
	static int base_curr = 2000;
	static int delay = -1;

	/* get adc value */
	adc_volt = spear_battery_adc_value(battery_pdata->adc_volt) * 2;
	adc_curr = spear_battery_adc_value(battery_pdata->adc_current);
	dev_info(&battery_pdev->dev, "adc_volt=%d, adc_curr=%d\n", adc_volt, adc_curr);

	dc_status = spear_charger_status();
	if (old_dc_status != dc_status) { /* dc status changed */
		dev_info(&battery_pdev->dev, "status changing\n");
		old_dc_status = dc_status;
		delay = 1; /* do not read adc immediately */
	}
	if (delay > 0) {
		delay--;
		return old_level;
	} else if (delay == 0) {
		delay = -1; /* read once */
		base_level = old_level;
		max_volt = adc_volt;
		dev_info(&battery_pdev->dev, "max_volt=%d, base_level=%d\n", max_volt, base_level);
	}

	if (dc_status == POWER_SUPPLY_STATUS_DISCHARGING) {
		/* battery */
		max_volt = adc_volt > max_volt ? adc_volt : max_volt;
		level = spear_battery_calc_level(adc_volt, max_volt, min_volt, base_level, 0);
	} else {
		/* dc */
		if (adc_volt < max_volt && base_level < div) {
			/* constant current charging stage, volt is increasing */
			/* base_level - div % */
			level = spear_battery_calc_level(adc_volt, max_volt, min_volt, div, base_level);
		} else {
			/* constant voltage charging stage, curr is decreasing */
			/* base - 100 % */
			if (adc_curr < 100) {
				level = 100;
				max_volt = adc_volt;
			} else {
				adc_curr = adc_curr > base_curr ? 0 : base_curr - adc_curr;
				level = spear_battery_calc_level(adc_curr, base_curr, 100, 100, base_level);
			}
		}

		level = level < old_level ? old_level : level ;
	}

	old_level = level;
	return level;
}

static int spear_battery_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val)
{
	int ret = 0;

	switch (psp) {
		case POWER_SUPPLY_PROP_STATUS:
			val->intval = spear_charger_status();
			break;
		case POWER_SUPPLY_PROP_HEALTH:
			val->intval = POWER_SUPPLY_HEALTH_GOOD;
			break;
		case POWER_SUPPLY_PROP_PRESENT:
			val->intval = 1;
			break;
		case POWER_SUPPLY_PROP_TECHNOLOGY:
			val->intval = POWER_SUPPLY_TECHNOLOGY_LION;
			break;
		case POWER_SUPPLY_PROP_CAPACITY:
			val->intval = spear_battery_get_capacity();
			break;
		case POWER_SUPPLY_PROP_TEMP:
			val->intval = 20;
			break;
		case POWER_SUPPLY_PROP_VOLTAGE_NOW:
			val->intval = spear_battery_adc_value(battery_pdata->adc_volt) * 2 * 1000;
			break;
		default:
			ret = -EINVAL;
			break;
	}
	return ret;
}

/**
 * In android, need the driver notify the application to update power level display.
 */
static void spear_battery_work(struct work_struct *work)
{
	power_supply_changed(&spear_battery_bat);
	schedule_delayed_work(&battery_dwork, battery_pdata->delay);
}

/**
 * adc config
 */
static int spear_battery_adc_config(struct platform_device *pdev, int chan_id)
{
	int ret = 0;
	struct adc_chan_config adc_cfg;

	adc_cfg.chan_id		= chan_id;
	adc_cfg.avg_samples	= SAMPLE8;
	adc_cfg.scan_rate	= 5000; /* micro second*/
	adc_cfg.scan_rate_fixed = false;/* dma only available for chan-0 */

	ret = spear_adc_chan_get(pdev, adc_cfg.chan_id);
	if (ret < 0) {
		dev_err(&pdev->dev, "Err in ADC channel get for battery ret=%d\n", ret);
		return ret;
	}

	ret = spear_adc_chan_configure(pdev, &adc_cfg);
	if (ret < 0) {
		dev_err(&pdev->dev, "Err in ADC configure for battery ret=%d\n", ret);
		return ret;
	}

	return ret;
}

static int __init spear_battery_init(void)
{
	int ret = 0;

	battery_pdev = platform_device_register_simple("battery", 0, NULL, 0);
	if (IS_ERR(battery_pdev))
		return PTR_ERR(battery_pdev);

	/** adc configure */
	spear_battery_adc_config(battery_pdev, battery_pdata->adc_volt);
	spear_battery_adc_config(battery_pdev, battery_pdata->adc_current);

	/** charger init */
	spear_battery_gpio_init(battery_pdev, battery_pdata->st1);
	spear_battery_gpio_init(battery_pdev, battery_pdata->st2);
	old_dc_status = spear_charger_status();

	/** power supply register */
	power_supply_register(&battery_pdev->dev, &spear_battery_bat);

	/** init delay work */
	INIT_DELAYED_WORK(&battery_dwork, spear_battery_work);
	schedule_delayed_work(&battery_dwork, 0);

	printk(KERN_INFO "spear_battery: loaded.\n");
	return ret;
}

static void __exit spear_battery_exit(void)
{
	cancel_delayed_work_sync(&battery_dwork);
	spear_adc_chan_put(battery_pdev, battery_pdata->adc_volt);
	spear_adc_chan_put(battery_pdev, battery_pdata->adc_current);
	gpio_free(battery_pdata->st1);
	gpio_free(battery_pdata->st2);
	power_supply_unregister(&spear_battery_bat);
	platform_device_unregister(battery_pdev);

	printk(KERN_INFO "spear_battery: unloaded.\n");
}


module_init(spear_battery_init);
module_exit(spear_battery_exit);
MODULE_AUTHOR("Vincenzo Frascino <vincenzo.frascino@st.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Android SPEAr battery driver.");

