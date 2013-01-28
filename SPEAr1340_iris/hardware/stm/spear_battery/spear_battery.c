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

#include <linux/device.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
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
	.delay		= 60 * HZ, /* sec */
	.st1		= GPIO0_7,
	.st2		= PLGPIO_32,
};

static struct platform_device *battery_pdev;
static struct spear_battery_pdata_t *battery_pdata = &spear_battery_pdata;
static struct delayed_work battery_dwork;

static int old_state;
static int vmax = 1200; // 4000 - vmin
static int vmin = 2800;
static int cmax = 1900; // 2000 - cmin
static int cmin = 100;
static int lmax = 100;
static int lold= 0;
static int div = 90; /* CC to CV charging percent divider */

static int spear_battery_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val);

static int spear_battery_ac_get_property(struct power_supply *psy,
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

static enum power_supply_property spear_battery_ac_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static struct power_supply spear_battery_ac = {
	.name = "ac",
	.type = POWER_SUPPLY_TYPE_MAINS,
	.properties 	=  spear_battery_ac_props,
	.num_properties = ARRAY_SIZE(spear_battery_ac_props),
	.get_property	= spear_battery_ac_get_property,
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

	if (st1 == 0 && st2 == 1) { /* st1: on, st2: off, charge in progress */
		return POWER_SUPPLY_STATUS_CHARGING;
	} else if (st1 == 1 && st2 == 0) { /* st1: off, st2: on, charge done */
		return POWER_SUPPLY_STATUS_FULL;
	} else { /* st1: off, st2: off, discharging (stand by mode in doc) */
		return POWER_SUPPLY_STATUS_DISCHARGING;
	}
}

/* remember changing state */
static int spear_charger_changing(int state, int adc_volt)
{
	static int delay = -1;

	if (old_state != state) { /* dc status changed */
		old_state = state;
		delay = 1; /* do not read adc immediately */
	}

	if (delay > 0) {
		delay--;
		return 0; /* return old_level */
	} else if (delay == 0) {
		delay = -1; /* read once */
		if (state == POWER_SUPPLY_STATUS_DISCHARGING) {
			/* dc to bat */
			vmax = adc_volt;
			lmax = lold;
		} else {
			/* bat to dc */
			if (lold < div)
				vmax = adc_volt * div / lold;
			else 
				vmax =  1200;
		}
		dev_info(&battery_pdev->dev, "\nstate change: vmax=%d, lmax=%d, lold=%d\n", vmax, lmax, lold);
	}

	return 1; /* normal */
}

/**
 * currently just use a simple alogrithm for calc power level.
 */
static uint spear_battery_get_capacity(void)
{
	int level;
	int adc_volt, adc_curr;
	int state;

	/* get adc value */
	adc_volt = spear_battery_adc_value(battery_pdata->adc_volt) * 2;
	adc_curr = spear_battery_adc_value(battery_pdata->adc_current);
	adc_volt -= vmin; adc_volt = adc_volt > 0 ? adc_volt : 0 ;
	adc_curr -= cmin; adc_curr = adc_curr > 0 ? adc_curr : 0 ;
	dev_info(&battery_pdev->dev, "volt=%d, curr=%d", adc_volt, adc_curr);

	state = spear_charger_status();
	if (spear_charger_changing(state, adc_volt) == 0)
		return lold;

	if (state == POWER_SUPPLY_STATUS_DISCHARGING) {
		/* battery */
		vmax = adc_volt > vmax ? adc_volt : vmax;
		level = adc_volt * lmax / vmax;

		/* level should not increase in bat mode */
		level = level > lold ? lold : level ;
	} else {
		/* dc */
		/* full */
		if (adc_curr < 100) {
			level = 100;
			vmax = adc_volt;
		} else {
			/* calc volt first */
			if (adc_volt <= vmax ) {
				level = adc_volt * div / vmax;
			}

			/* calc curr if current start to decrease */
			if (adc_curr <= cmax) {
				adc_curr = adc_curr > cmax ? 0 : cmax - adc_curr;
				level = adc_curr * (100-div) / cmax + div;
			}

			/* level should not decrease in dc mode */
			level = level < lold ? lold : level ;
		}
	}

	lold = level;
	dev_info(&battery_pdev->dev, "level=%d%%\n", level);
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

static int spear_battery_ac_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val)
{
	int ret = 0;

	switch (psp) {
		case POWER_SUPPLY_PROP_ONLINE:
			if (spear_charger_status() == POWER_SUPPLY_STATUS_DISCHARGING)
				val->intval = 0;
			else
				val->intval = 1;
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
 * cmd:
 * 0 - get channel
 * 1 - put channel
 */
static int spear_battery_adc_config(struct platform_device *pdev, int chan_id, int cmd)
{
	int ret = 0;
	struct adc_chan_config adc_cfg;

	adc_cfg.chan_id		= chan_id;
	adc_cfg.avg_samples	= SAMPLE8;
	adc_cfg.scan_rate	= 5000; /* micro second*/
	adc_cfg.scan_rate_fixed = false;/* dma only available for chan-0 */

	if (cmd == 0) { 
		ret = spear_adc_chan_get(pdev, adc_cfg.chan_id);
		if (ret < 0) {
			dev_err(&pdev->dev,
					"Err in ADC channel get for battery ret=%d\n", ret);
			return ret;
		}

		ret = spear_adc_chan_configure(pdev, &adc_cfg);
		if (ret < 0) {
			dev_err(&pdev->dev,
					"Err in ADC configure for battery ret=%d\n", ret);
			return ret;
		}
	} else {
		ret = spear_adc_chan_put(pdev, adc_cfg.chan_id);
		if (ret < 0) {
			dev_err(&pdev->dev,
					"Err in ADC channel put for battery ret=%d\n", ret);
			return ret;
		}
	}

	return ret;
}

static void spear_battery_cap_init(void)
{
	int adc_volt, adc_curr;

	mdelay(250);
	old_state = spear_charger_status();
	adc_volt = spear_battery_adc_value(battery_pdata->adc_volt) * 2;
	adc_curr = spear_battery_adc_value(battery_pdata->adc_current);
	adc_volt -= vmin; adc_volt = adc_volt > 0 ? adc_volt : 0 ;
	vmax = adc_volt > vmax ? adc_volt : vmax ;
	adc_curr -= cmin; adc_curr = adc_curr > 0 ? adc_curr : 0 ;

	if (old_state == POWER_SUPPLY_STATUS_DISCHARGING) {
		lmax = adc_volt * 100 / vmax;
		vmax = adc_volt;
		lold = lmax;
	} else {
		if (adc_curr <=0) {
			lmax = 100;
			lold = lmax;
			vmax = adc_volt;
		} else {
			if (adc_volt < vmax) {
				lold = adc_volt * div / vmax;
			} else {
				adc_curr = adc_curr > cmax ? 0 : cmax - adc_curr;
				lold = adc_curr * (100-div) / cmax + div;
			}
		}
	}

	dev_info(&battery_pdev->dev, "init: volt=%d, curr=%d, lold=%d%%\n", adc_volt, adc_curr, lold);

}

#ifdef CONFIG_PM
static int spear_battery_suspend(struct device *dev)
{
	spear_battery_adc_config(battery_pdev, battery_pdata->adc_volt, 1);
	spear_battery_adc_config(battery_pdev, battery_pdata->adc_current, 1);

	dev_info(dev, "Suspended.\n");

	return 0;
}

static int spear_battery_resume(struct device *dev)
{
	spear_battery_adc_config(battery_pdev, battery_pdata->adc_volt, 0);
	spear_battery_adc_config(battery_pdev, battery_pdata->adc_current, 0);

	dev_info(dev, "Resumed.\n");

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(spear_battery_pm_ops,
		spear_battery_suspend, spear_battery_resume);

static int spear_battery_probe(struct platform_device *pdev)
{
	int ret = 0;

	battery_pdev = platform_device_register_simple("battery", 0, NULL, 0);
	if (IS_ERR(battery_pdev))
		return PTR_ERR(battery_pdev);

	/** adc configure */
	spear_battery_adc_config(battery_pdev, battery_pdata->adc_volt, 0);
	spear_battery_adc_config(battery_pdev, battery_pdata->adc_current, 0);

	/** charger init */
	spear_battery_gpio_init(battery_pdev, battery_pdata->st1);
	spear_battery_gpio_init(battery_pdev, battery_pdata->st2);

	/* capacity init */
	spear_battery_cap_init();

	/** power supply register */
	power_supply_register(&battery_pdev->dev, &spear_battery_ac);
	power_supply_register(&battery_pdev->dev, &spear_battery_bat);

	/** init delay work */
	INIT_DELAYED_WORK(&battery_dwork, spear_battery_work);
	schedule_delayed_work(&battery_dwork, 0);

	dev_info(&pdev->dev, "Loaded.\n");
	return ret;
}

static int spear_battery_exit(struct platform_device *pdev)
{
	cancel_delayed_work_sync(&battery_dwork);
	spear_adc_chan_put(battery_pdev, battery_pdata->adc_volt);
	spear_adc_chan_put(battery_pdev, battery_pdata->adc_current);
	gpio_free(battery_pdata->st1);
	gpio_free(battery_pdata->st2);
	power_supply_unregister(&spear_battery_bat);
	power_supply_unregister(&spear_battery_ac);
	platform_device_unregister(battery_pdev);

	dev_info(&pdev->dev, "Unloaded.\n");
	return 0;
}

static struct platform_driver spear_battery_driver = {
	.probe = spear_battery_probe,
	.remove = spear_battery_exit,
	.driver = {
		.name = "spear_battery",
		.owner = THIS_MODULE,
#ifdef CONFIG_PM
		.pm = &spear_battery_pm_ops,
#endif
	},
};

static int __init spear_battery_init(void)
{
	return platform_driver_register(&spear_battery_driver);
}
module_init(spear_battery_init);

static void __exit spear_battery_cleanup(void)
{
	platform_driver_unregister(&spear_battery_driver);
}
module_exit(spear_battery_cleanup);

MODULE_AUTHOR("Vincenzo Frascino <vincenzo.frascino@st.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Android SPEAr battery driver.");

