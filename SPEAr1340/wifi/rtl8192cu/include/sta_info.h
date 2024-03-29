/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 
******************************************************************************/
#ifndef __STA_INFO_H_
#define __STA_INFO_H_

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>
#include <wifi.h>

#define NUM_STA 32
#define NUM_ACL 64


//if mode ==0, then the sta is allowed once the addr is hit.
//if mode ==1, then the sta is rejected once the addr is non-hit.
struct wlan_acl_node {
        _list		        list;
        u8       addr[ETH_ALEN];
        u8       mode;
};

struct wlan_acl_pool {
        struct wlan_acl_node aclnode[NUM_ACL];
};

typedef struct _RSSI_STA{
	int	UndecoratedSmoothedPWDB;
	int	UndecoratedSmoothedCCK;
}RSSI_STA, *PRSSI_STA;

struct	stainfo_stats	{

	u64	rx_pkts;
	u64	rx_bytes;
	u64	rx_drops;
	u64	last_rx_pkts;
	
	u64	tx_pkts;
	u64	tx_bytes;
	u64  tx_drops;

};

struct sta_info {

	_lock	lock;
	_list	list; //free_sta_queue
	_list	hash_list; //sta_hash
	//_list asoc_list; //20061114
	//_list sleep_list;//sleep_q
	//_list wakeup_list;//wakeup_q
	
	struct sta_xmit_priv sta_xmitpriv;
	struct sta_recv_priv sta_recvpriv;
	
	_queue sleep_q;
	unsigned int sleepq_len;
	
	uint state;
	uint aid;
	uint mac_id;
	uint qos_option;
	u8	hwaddr[ETH_ALEN];

	uint	ieee8021x_blocked;	//0: allowed, 1:blocked 
	uint	dot118021XPrivacy; //aes, tkip...
	union Keytype	dot11tkiptxmickey;
	union Keytype	dot11tkiprxmickey;
	union Keytype	dot118021x_UncstKey;	
	union pn48		dot11txpn;			// PN48 used for Unicast xmit.
	union pn48		dot11rxpn;			// PN48 used for Unicast recv.


	u8	bssrateset[16];
	u32	bssratelen;
	s32  rssi;
	s32	signal_quality;
	
	u8	cts2self;
	u8	rtsen;

	u8	raid;
	u8 	init_rate;

	struct stainfo_stats sta_stats;

	//for A-MPDU TX, ADDBA timeout check	
	_timer addba_retry_timer;
	
	//for A-MPDU Rx reordering buffer control 
	struct recv_reorder_ctrl recvreorder_ctrl[16];

	//for A-MPDU Tx
	//unsigned char		ampdu_txen_bitmap;

#ifdef CONFIG_80211N_HT
	struct ht_priv	htpriv;	
#endif
	
	//Notes:	
	//STA_Mode:
	//curr_network(mlme_priv/security_priv/qos/ht) + sta_info: (STA & AP) CAP/INFO	
	//scan_q: AP CAP/INFO

	//AP_Mode:
	//curr_network(mlme_priv/security_priv/qos/ht) : AP CAP/INFO
	//sta_info: (AP & STA) CAP/INFO
		
#ifdef CONFIG_AP_MODE

	_list asoc_list;
	_list auth_list;
	 
	unsigned int expire_to;
	unsigned int auth_seq;
	unsigned int authalg;
	unsigned char chg_txt[128];

	u16 capability;	
	int flags;	

	int dot8021xalg;//0:disable, 1:psk, 2:802.1x
	int wpa_psk;//0:disable, bit(0): WPA, bit(1):WPA2
	int wpa_group_cipher;
	int wpa2_group_cipher;
	int wpa_pairwise_cipher;
	int wpa2_pairwise_cipher;	

#ifdef CONFIG_NATIVEAP_MLME
	u8 wpa_ie[32];

	u8 nonerp_set;
	u8 no_short_slot_time_set;
	u8 no_short_preamble_set;
	u8 no_ht_gf_set;
	u8 no_ht_set;
	u8 ht_20mhz_set;
#endif

	unsigned int tx_ra_bitmap;
	u8 qos_info;

	u8 max_sp_len;
	u8 uapsd_bk;//BIT(0): Delivery enabled, BIT(1): Trigger enabled
	u8 uapsd_be;
	u8 uapsd_vi;
	u8 uapsd_vo;	

	u8 has_legacy_ac;
	unsigned int sleepq_ac_len;

#if ( P2P_INCLUDED == 1 )
	//p2p priv data
	u8 is_p2p_device;
	u8 p2p_status_code;

	//p2p client info
	u8 dev_addr[ETH_ALEN];
	//u8 iface_addr[ETH_ALEN];//= hwaddr[ETH_ALEN]
	u8 dev_cap;
	u16 config_methods;
	u8 primary_dev_type[8];
	u8 num_of_secdev_type;
	u8 secdev_types_list[32];// 32/8 == 4;
	u16 dev_name_len;
	u8 dev_name[32];	
#endif
#endif	

	//for DM
	RSSI_STA	 rssi_stat;
	

};



struct	sta_priv {
	
	u8 *pallocated_stainfo_buf;
	u8 *pstainfo_buf;
	_queue	free_sta_queue;
	
	_lock sta_hash_lock;
	_list   sta_hash[NUM_STA];
	int asoc_sta_count;
	_queue sleep_q;
	_queue wakeup_q;
	
	_adapter *padapter;
	

#ifdef CONFIG_AP_MODE
	_list asoc_list;
	_list auth_list;

	unsigned int auth_to;  //sec, time to expire in authenticating.
	unsigned int assoc_to; //sec, time to expire before associating.
	unsigned int expire_to; //sec , time to expire after associated.
	
	/* pointers to STA info; based on allocated AID or NULL if AID free
	 * AID is in the range 1-2007, so sta_aid[0] corresponders to AID 1
	 * and so on
	 */
	struct sta_info *sta_aid[NUM_STA];

	u16 sta_dz_bitmap;//only support 15 stations, staion aid bitmap for sleeping sta.
	u16 tim_bitmap;//only support 15 stations, aid=0~15 mapping bit0~bit15	

	u16 max_num_sta;
#endif		
	
};


__inline static u32 wifi_mac_hash(u8 *mac)
{
        u32 x;

        x = mac[0];
        x = (x << 2) ^ mac[1];
        x = (x << 2) ^ mac[2];
        x = (x << 2) ^ mac[3];
        x = (x << 2) ^ mac[4];
        x = (x << 2) ^ mac[5];

        x ^= x >> 8;
        x  = x & (NUM_STA - 1);
		
        return x;
}


extern u32	_init_sta_priv(struct sta_priv *pstapriv);
extern u32	_free_sta_priv(struct sta_priv *pstapriv);
extern struct sta_info *alloc_stainfo(struct	sta_priv *pstapriv, u8 *hwaddr);
extern u32	free_stainfo(_adapter *padapter , struct sta_info *psta);
extern void free_all_stainfo(_adapter *padapter);
extern struct sta_info *get_stainfo(struct sta_priv *pstapriv, u8 *hwaddr);
extern u32 init_bcmc_stainfo(_adapter* padapter);
extern struct sta_info* get_bcmc_stainfo(_adapter* padapter);
extern u8 access_ctrl(struct wlan_acl_pool* pacl_list, u8 * mac_addr);

#endif //_STA_INFO_H_
