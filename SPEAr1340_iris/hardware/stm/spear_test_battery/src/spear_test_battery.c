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
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/spear_adc.h>

#define BAT_ADC_CHN	ADC_CHANNEL1

static int spear_test_battery_bat_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val);

static struct platform_device *spear_test_battery_pdev;
static struct delayed_work g_spear_test_battery_work;

static enum power_supply_property spear_test_battery_battery_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_CAPACITY,
};

static struct power_supply spear_test_battery_bat = {
	.name = "battery",
	.type = POWER_SUPPLY_TYPE_BATTERY,
	.properties 	=  spear_test_battery_battery_props,
	.num_properties = ARRAY_SIZE(spear_test_battery_battery_props),
	.get_property	= spear_test_battery_bat_get_property,
	.use_for_apm 	= 1,
};

/**
 * adc return volt value of pin.
 * 2000mV take as full charged, 1700mV take as low power.
 * currently just use a simple alogrithm for calc power level.
 */
#define FULL_CHARGED 2000
#define LOW_POWER 1700
static uint spear_test_battery_bat_get_capacity(struct platform_device *pdev)
{
	uint adc_val;
	uint level;

	/* get adc voltage */
	spear_adc_get_data(pdev, BAT_ADC_CHN, &adc_val, 1);
	adc_val = adc_val > FULL_CHARGED ? FULL_CHARGED : adc_val;
	adc_val = adc_val < LOW_POWER ? LOW_POWER : adc_val;

	/* calculate power level */
	level = (adc_val - LOW_POWER) * 100 / (FULL_CHARGED - LOW_POWER);

	return level;
}

static int spear_test_battery_bat_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val)
{
	int ret = 0;

	switch (psp) {
		case POWER_SUPPLY_PROP_STATUS:
			val->intval = POWER_SUPPLY_STATUS_CHARGING;
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
			val->intval = spear_test_battery_bat_get_capacity(spear_test_battery_pdev);
			break;
		case POWER_SUPPLY_PROP_TEMP:
			val->intval = 20;
			break;
		case POWER_SUPPLY_PROP_VOLTAGE_NOW:
			val->intval = 5;
			break;
		default:
			ret = -EINVAL;
			break;
	}
	return ret;
}

/**
 * In android, need the driver notify the application to update power level display.
 * Here we take 30secs to loop update power level
 */
static void spear_test_battery_work(struct work_struct *work)
{
	const int interval = HZ * 30; /* seconds */
	static uint old_level = -1;
	uint level;

	level = spear_test_battery_bat_get_capacity(spear_test_battery_pdev);
	if (level != old_level ) {
		old_level = level;
		power_supply_changed(&spear_test_battery_bat);
	}

	schedule_delayed_work(&g_spear_test_battery_work, interval);
}

/**
 * spear_adc_config
 */
static int spear_adc_config(struct platform_device *pdev)
{
	int ret = 0;
	struct adc_chan_config adc_cfg;

	ret = spear_adc_chan_get(pdev, BAT_ADC_CHN);
	if (ret < 0) {
		dev_err(&pdev->dev, "Err in ADC channel get for battery ret=%d\n", ret);
		return ret;
	}

	adc_cfg.chan_id		= BAT_ADC_CHN;
	adc_cfg.avg_samples	= SAMPLE8;
	adc_cfg.scan_rate	= 5000; /* micro second*/
	adc_cfg.scan_rate_fixed = false;/* dma only available for chan-0 */

	ret = spear_adc_chan_configure(pdev, &adc_cfg);
	if (ret < 0) {
		dev_err(&pdev->dev, "Err in ADC configure for battery ret=%d\n", ret);
		return ret;
	}

	return ret;
}

static int __init spear_test_battery_init(void)
{
	int ret = 0;

	/** platform device register */
	spear_test_battery_pdev = platform_device_register_simple("battery",
								0,
								NULL,
								0);
	if (IS_ERR(spear_test_battery_pdev))
		return PTR_ERR(spear_test_battery_pdev);

	/** adc configure */
	if (spear_adc_config(spear_test_battery_pdev) < 0)
		goto adc_failed;

	/** power supply register */
	ret = power_supply_register(&spear_test_battery_pdev->dev,
				&spear_test_battery_bat);
	if (ret)
		goto bat_failed;

	/** init delay work */
	INIT_DELAYED_WORK(&g_spear_test_battery_work, spear_test_battery_work);
	schedule_delayed_work(&g_spear_test_battery_work, 0);

	printk(KERN_INFO "spear_test_battery: loaded.\n");
	return ret;

bat_failed:
	spear_adc_chan_put(spear_test_battery_pdev, BAT_ADC_CHN);
adc_failed:
	platform_device_unregister(spear_test_battery_pdev);
	return ret;
}

static void __exit spear_test_battery_exit(void)
{
	cancel_delayed_work_sync(&g_spear_test_battery_work);
	spear_adc_chan_put(spear_test_battery_pdev, BAT_ADC_CHN);
	power_supply_unregister(&spear_test_battery_bat);
	platform_device_unregister(spear_test_battery_pdev);
	printk(KERN_INFO "spear_test_battery: unloaded.\n");
}

module_init(spear_test_battery_init);
module_exit(spear_test_battery_exit);
MODULE_AUTHOR("Vincenzo Frascino <vincenzo.frascino@st.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Android SPEAr test battery driver.");

