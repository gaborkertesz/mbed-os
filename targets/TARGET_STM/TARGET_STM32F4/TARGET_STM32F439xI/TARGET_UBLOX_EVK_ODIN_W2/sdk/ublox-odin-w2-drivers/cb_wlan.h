/*---------------------------------------------------------------------------
 * Copyright (c) 2016, u-blox Malmö, All Rights Reserved
 * SPDX-License-Identifier: LicenseRef-PBL
 *
 * This file and the related binary are licensed under the
 * Permissive Binary License, Version 1.0 (the "License");
 * you may not use these files except in compliance with the License.
 *
 * You may obtain a copy of the License here:
 * LICENSE-permissive-binary-license-1.0.txt and at
 * https://www.mbed.com/licenses/PBL-1.0
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Component   : WLAN
 * File        : cb_wlan.h
 *
 * Description : Main WLAN component, ties together WM, SUPPLICANT and 
 *               TARGET to one streamlined API.
 *-------------------------------------------------------------------------*/

/**
 * @file cb_wlan.h The main WLAN component interface.
 * All functions declared extern needs to be provided by another/upper layer.
 * @ingroup wlan
 */

#ifndef _CB_WLAN_H_
#define _CB_WLAN_H_

#include "cb_types.h"
#include "cb_wlan_types.h"
#include "cb_cert_utils.h"
#include "cb_status.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 * DEFINES
 *=========================================================================*/

/**
 * Max username length in @ref cbWLAN_EnterpriseConnectParameters
 *
 * @ingroup wlan
 */
#define cbWLAN_MAX_USERNAME_LENGTH      64

/**
 * Max password length in @ref cbWLAN_Util_PSKFromPWD and @ref cbWLAN_EnterpriseConnectParameters
 *
 * @ingroup wlan
 */
#define cbWLAN_MAX_PASSPHRASE_LENGTH    64

/**
 * PSK length in @ref cbWLAN_WPAPSKConnectParameters
 *
 * @ingroup wlan
 */
#define cbWLAN_PSK_LENGTH    32


/**
 * Max domain name length in @ref cbWLAN_EnterpriseConnectParameters
 *
 * @ingroup wlan
 */
#define cbWLAN_MAX_DOMAIN_LENGTH        64


/*===========================================================================
 * TYPES
 *=========================================================================*/
/**
 * Start parameters passed to WLAN driver.
 *
 * @ingroup wlan
 */
typedef struct cbWLAN_StartParameters {
    cbWLAN_MACAddress      mac;        /**< MAC of WLAN interface, set to all zeros if hardware programmed address should be used. */
    cb_boolean disable80211d;
    cbWM_ModuleType  deviceType;      /**< Specify current device type. */
    union {
        struct {
            cbWM_TxPowerSettings txPowerSettings;   /**< Transmission power settings. */
        } ODIN_W26X;
    } deviceSpecific;
} cbWLAN_StartParameters;

/**
 * Common connect parameters.
 *
 * @ingroup wlan
 */
typedef struct cbWLAN_CommonConnectParameters {
    cbWLAN_MACAddress      bssid;      /**< BSSID to connect to, set to all zero for any BSSID. */
    cbWLAN_Ssid            ssid;       /**< SSID to connect to. */
} cbWLAN_CommonConnectParameters;


/**
 * WEP specific connect parameters.
 *
 * @ingroup wlan
 */
typedef struct cbWLAN_WEPConnectParameters {
    cbWLAN_WEPKey           keys[4];    /**< WEP keys. */
    cb_uint32               txKey;      /**< Active WEP transmission key index (0-3). */
} cbWLAN_WEPConnectParameters;

/**
* WPA PSK parameters.
*
* @ingroup wlan
*/
typedef struct cbWLAN_WPAPSK {
    cb_uint8                key[cbWLAN_PSK_LENGTH]; /**< WPA pre-shared key in binary form. */
} cbWLAN_WPAPSK;

/**
 * WPA PSK specific connect parameters.
 *
 * @ingroup wlan
 */
typedef struct cbWLAN_WPAPSKConnectParameters {
    cbWLAN_WPAPSK           psk; /**< WPA pre-shared key*/
} cbWLAN_WPAPSKConnectParameters;


typedef enum cbWLAN_CipherSuite {
    cbWLAN_CIPHER_SUITE_NONE        = 0x00,
    cbWLAN_CIPHER_SUITE_WEP64       = 0x01,
    cbWLAN_CIPHER_SUITE_WEP128      = 0x02,
    cbWLAN_CIPHER_SUITE_TKIP        = 0x04,
    cbWLAN_CIPHER_SUITE_AES_CCMP    = 0x08,
} cbWLAN_CipherSuite;

typedef enum cbWLAN_AuthenticationSuite {
    cbWLAN_AUTHENTICATION_SUITE_NONE            = 0x00,
    cbWLAN_AUTHENTICATION_SUITE_SHARED_SECRET   = 0x01,
    cbWLAN_AUTHENTICATION_SUITE_PSK             = 0x02,
    cbWLAN_AUTHENTICATION_SUITE_8021X           = 0x04,
    cbWLAN_AUTHENTICATION_SUITE_USE_WPA         = 0x08,
    cbWLAN_AUTHENTICATION_SUITE_USE_WPA2        = 0x10,
} cbWLAN_AuthenticationSuite;


/**
 * WPA Enterprise specific connect parameters.
 *
 * @ingroup wlan
 */
typedef struct cbWLAN_EnterpriseConnectParameters {
    cbWLAN_EnterpriseMode   authMode;       /**< Enterprise authentication mode. */
    cb_uint8                username[cbWLAN_MAX_USERNAME_LENGTH];       /**< Username string. */
    cb_uint8                passphrase[cbWLAN_MAX_PASSPHRASE_LENGTH];   /**< Passphrase string. */
    cb_uint8                domain[cbWLAN_MAX_DOMAIN_LENGTH];           /**< Domain string. */
    cbCERT_Stream           *clientCertificate;     /**< Stream handle to provide SSL certificate for authentication. */
    cbCERT_Stream           *clientPrivateKey;      /**< STream handle to provide SSL private key for authentication. */
} cbWLAN_EnterpriseConnectParameters;

/**
 * Common access point parameters.
 *
 * @ingroup wlan
 */
typedef struct cbWLAN_CommonApParameters {
    cbWLAN_Ssid             ssid;       /**< SSID to connect to. */
    cbWLAN_Channel          channel;    /**< Active channel. */
    cbWLAN_RateMask         basicRates; /**< Basic rates. */
}cbWLAN_CommonApParameters;


/**
* WPA PSK specific AP parameters.
*
* @ingroup wlan
*/
typedef struct cbWLAN_WPAPSKApParameters {
    cbWLAN_CipherSuite      rsnCiphers;         /**< Bit field indicating which ciphers that shall be displayed in RSN information elements. If 0 no RSN information elements is added to beacons and probe responses. */
    cbWLAN_CipherSuite      wpaCiphers;         /**< Bit field indicating which ciphers that shall be displayed in WPA information elements. If 0 no WPA information elements is added to beacons and probe responses. */
    cbWLAN_WPAPSK           psk;                /**< WPA pre-shared key*/
    cb_uint32               gtkRekeyInterval;   /**< Group rekey interval in seconds */
} cbWLAN_WPAPSKApParameters;


/**
 * Scan parameters
 *
 * @ingroup wlan
 */
typedef struct cbWLAN_ScanParameters {
    cbWLAN_Ssid             ssid;       /**< SSID to scan for, set to zero length for broadcast scan. */
} cbWLAN_ScanParameters;

/**
 * Scan result information reported from WLAN component. Contains info for
 * one specific BSS.
 *
 * @ingroup wlan
 */
typedef struct cbWLAN_ScanIndicationInfo {
    cbWLAN_MACAddress bssid;                /**< BSS BSSID */
    cbWLAN_Ssid ssid;                       /**< BSS SSID */
    cbWLAN_Channel channel;                 /**< BSS channel */
    cbWLAN_OperationalMode operationalMode; /**< BSS type */
    cb_int32 rssi;                          /**< RSSI for scan result packet. */

    cbWLAN_AuthenticationSuite authenticationSuites; /**< Supported authentication suites */
    cbWLAN_CipherSuite unicastCiphers;              /**< Supported unicast cipher suites */
    cbWLAN_CipherSuite groupCipher;                 /**< Supported group cipher suites */

    cbWLAN_RateMask basicRateSet;                   /**< Basic rate set, i.e. required rates. */
    cbWLAN_RateMask supportedRateSet;               /**< Supported rate set, super set of basic rate set. */
    cb_uint32 beaconPeriod;                         /**< Beacon period in ms. */
    cb_uint32 DTIMPeriod;                           /**< DTIM period in beacon intervals */
    cb_uint8 countryCode[3];                        /**< Three letter country code */
    cb_uint32 flags;              // QoS, short preamble, DFS, privacy,
} cbWLAN_ScanIndicationInfo;

/**
 * Status indications indicated by @ref cbWLAN_statusIndication.
 *
 * @ingroup wlan
 */
typedef enum {
    cbWLAN_STATUS_STOPPED,
    cbWLAN_STATUS_STARTED,
    cbWLAN_STATUS_ERROR,
    cbWLAN_STATUS_DISCONNECTED,
    cbWLAN_STATUS_CONNECTING,
    cbWLAN_STATUS_CONNECTED,
    cbWLAN_STATUS_CONNECTION_FAILURE,
    cbWLAN_STATUS_AP_UP,
    cbWLAN_STATUS_AP_DOWN,
    cbWLAN_STATUS_AP_STA_ADDED,
    cbWLAN_STATUS_AP_STA_REMOVED,
} cbWLAN_StatusIndicationInfo;

/**
 * Disconnection reasons for @ref cbWLAN_STATUS_DISCONNECTED.
 *
 * @ingroup wlan
 */
typedef enum {
    cbWLAN_STATUS_DISCONNECTED_UNKNOWN,
    cbWLAN_STATUS_DISCONNECTED_NO_BSSID_FOUND,
    cbWLAN_STATUS_DISCONNECTED_AUTH_TIMEOUT,
    cbWLAN_STATUS_DISCONNECTED_MIC_FAILURE, 
} cbWLAN_StatusDisconnectedInfo;

/**
 * IOCTL parameters @ref cbWLAN_ioctl
 *
 * @ingroup wlan
 */
typedef enum {
    cbWLAN_IOCTL_FIRST,
    cbWLAN_IOCTL_SET_POWER_SAVE_MODE = cbWLAN_IOCTL_FIRST,       //!< Set power mode  @ref cbWLAN_IoctlPowerSaveMode
    cbWLAN_IOCTL_GET_POWER_SAVE_MODE,                            //!< Get power mode  @ref cbWLAN_IoctlPowerSaveMode
    cbWLAN_IOCTL_SET_LISTEN_INTERVAL,                            //!< Set listen interval, integer value 0 - 16 
    cbWLAN_IOCTL_GET_LISTEN_INTERVAL,                            //!< Get listen interval, integer value 0 - 16 
    cbWLAN_IOCTL_SET_DTIM_ENABLE,                                //!< Set DTIM enable 0, disable 1 enable
    cbWLAN_IOCTL_GET_DTIM_ENABLE,                                //!< Get DTIM enable 0, disable 1 enable
    cbWLAN_IOCTL_SET_SLEEP_TIMEOUT,                              //!< Set enter power save entry delay (in ms). Power save mode will be entered only if there no activity during this delay
    cbWLAN_IOCTL_GET_SLEEP_TIMEOUT,                              //!< Get enter power save entry delay (in ms). Power save mode will be entered only if there no activity during this delay
    cbWLAN_IOCTL_LAST,
    cbWLAN_IOCTL_SET_GSETTING = 1000,                            //!< Pipe to @ref cbWM_gSet.
    cbWLAN_IOCTL_SET_TSETTING = 2000,                            //!< Pipe to @ref cbWM_tSet.
    cbWLAN_IOCTL_GET_GSETTING = 3000,                            //!< Pipe to @ref cbWM_gGet.
    cbWLAN_IOCTL_GET_TSETTING = 4000,                            //!< Pipe to @ref cbWM_tGet.
} cbWLAN_Ioctl;

/**
 * Power save modes set using  @ref cbWLAN_ioctl
 *
 * @ingroup wlan
 */
typedef enum {
    cbWLAN_IOCTL_POWER_SAVE_MODE_OFF,
    cbWLAN_IOCTL_POWER_SAVE_MODE_SLEEP,
    cbWLAN_IOCTL_POWER_SAVE_MODE_DEEP_SLEEP
} cbWLAN_IoctlPowerSaveMode;

/**
 * Start parameters indicated from WLAN driver for status indication 
 * @ref cbWLAN_STATUS_STARTED.
 *
 * @ingroup wlan
 */
typedef struct cbWLAN_StatusStartedInfo {
    cbWLAN_MACAddress macAddress;      /**< MAC address of WLAN driver. */
} cbWLAN_StatusStartedInfo;

/**
 * Connected parameters indicated from WLAN driver for status indication 
 * @ref cbWLAN_STATUS_CONNECTED.
 *
 * @ingroup wlan
 */
typedef struct cbWLAN_StatusConnectedInfo {
    cbWLAN_MACAddress bssid;           /**< BSSID of the BSS connected to. */
    cbWLAN_Channel channel;             /**< Operating channels of the BSS connected to. */
} cbWLAN_StatusConnectedInfo;

/**
 * Received Ethernet data packet information and properties.
 *
 * @ingroup wlan
 */
typedef struct cbWLAN_PacketIndicationInfo {
    void        *rxData;                /**< Pointer to the port specific data type. */
    cb_uint32    size;                   /**< Length of the data payload in the port specific packet data type. */
    cb_boolean  isChecksumVerified;     /**< True if the TCP/UDP checksum is verified and correct. */
} cbWLAN_PacketIndicationInfo;

/**
 * Status updates from WLAN component. 
 * @note The callback must not make any call back to WLAN.
 *
 * @param callbackContext Context pointer provided in @ref cbWLAN_registerStatusCallback.
 * @param status Status indication type.
 * @param data Additional status indication data, depends on indication type.
 *
 * @sa cbWLAN_registerStatusCallback
 */
typedef void (*cbWLAN_statusIndication)(void *callbackContext, cbWLAN_StatusIndicationInfo status, void *data);


/**
 * Indication of received Ethernet data packet.
 *
 * @param callbackContext Context pointer provided in @ref cbWLAN_init.
 * @param packetInfo Pointer to struct containing packet information and data pointers.
 */
typedef void (*cbWLAN_packetIndication)(void *callbackContext, cbWLAN_PacketIndicationInfo *packetInfo);

/**
* Scan result indication from WLAN component.
*
* @param callbackContext Context pointer provided in @ref cbWLAN_init.
* @param bssDescriptor Pointer to struct containing scan result information.
* @param isLastResult @ref TRUE if scan scan is finished.
*/
typedef void (*cbWLAN_scanIndication)(void *callbackContext, cbWLAN_ScanIndicationInfo *bssDescriptor, cb_boolean isLastResult);

/*===========================================================================
 * WLAN API
 *=========================================================================*/

/**
 * Initialize WLAN component.
 *
 * @param callbackContext Context handle used in indication callbacks.
 * @return @ref cbSTATUS_OK if successful, otherwise cbSTATUS_ERROR.
 */
cbRTSL_Status cbWLAN_init(void *callbackContext);


/**
 * Stop WLAN component.
 * Stop and destroy WLAN driver instance.
 *
 * @return @ref cbSTATUS_OK if successful, otherwise cbSTATUS_ERROR.
 */
cbRTSL_Status cbWLAN_stop(void);

/**
 * Connect to access point in open mode (no encryption).
 * Connection progress is reported as @ref cbWLAN_statusIndication callbacks.
 *
 * @param commonParams Connection parameters.
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR. 
 */
cbRTSL_Status cbWLAN_connectOpen(cbWLAN_CommonConnectParameters *commonParams);

/**
 * Connect to access point in open mode with WEP encryption.
 * Connection progress is reported as @ref cbWLAN_statusIndication callbacks.
 *
 * @param commonParams Connection parameters.
 * @param wepParams WEP specific connection parameters.
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR. 
 */
cbRTSL_Status cbWLAN_connectWEP(cbWLAN_CommonConnectParameters *commonParams, cbWLAN_WEPConnectParameters *wepParams);

/**
 * Connect to access point with WPA PSK authentication.
 * Connection progress is reported as @ref cbWLAN_statusIndication callbacks.
 *
 * @param commonParams Connection parameters.
 * @param wpaParams WPA PSK specific connection parameters.
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR. 
 */
cbRTSL_Status cbWLAN_connectWPAPSK(cbWLAN_CommonConnectParameters *commonParams, cbWLAN_WPAPSKConnectParameters *wpaParams);

/**
 * Disconnect from access point or stop ongoing connection attempt.
 * Disconnection progress is reported as @ref cbWLAN_statusIndication callback.
 *
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR. 
 */
cbRTSL_Status cbWLAN_disconnect(void);

/**
 * Initiate BSS scan.
 * If specific channel is set in scan parameters, only that channel is 
 * scanned. If SSID is specified, a directed probe request against that SSID
 * will be used. Scan results are reported in @ref cbWLAN_scanIndication 
 * callbacks.
 * @note Depending on channel using DFS or not, passive scans may be used 
 * instead of active probe requests.
 *
 * @param params Scan parameters
 * @param scanIndication Callback function for scan results.
 * @param callbackContext Context pointer, will be sent back in callback.
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR.
 */
cbRTSL_Status cbWLAN_scan(cbWLAN_ScanParameters *params, cbWLAN_scanIndication scanIndication, void *callbackContext);


/**
* Retrieve an RSSI value for station mode.
*
* @note Depending on connection state and data transfer interval
* the value may be incorrect.
*
* @return RSSI value in dBm
*/
cb_int16 cbWLAN_STA_getRSSI();

/**
 * Start access point in open mode (no encryption).
 * Connection progress is reported as @ref cbWLAN_statusIndication callbacks.
 *
 * @param commonParams Common Accesspoint parameters.
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR. 
 */
cbRTSL_Status cbWLAN_apStartOpen(cbWLAN_CommonApParameters *commonParams);

/**
* Start access point with WPA PSK authentication.
* Connection progress is reported as @ref cbWLAN_statusIndication callbacks.
*
* @param commonParams Common Accesspoint parameters.
* @param wpaParams WPA PSK specific parameters.
* @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR.
*/
cbRTSL_Status cbWLAN_apStartWPAPSK(cbWLAN_CommonApParameters *commonParams, cbWLAN_WPAPSKApParameters *wpaParams);

/**
 * Stop access point.
 *
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR. 
 */
cbRTSL_Status cbWLAN_apStop(void);

/**
 * Send an Ethernet data packet.
 * @note Data send when not in connected state is just dropped.
 *
 * @param txData Pointer to the port specific Ethernet data type containing transmit data
 */
void cbWLAN_sendPacket(void *txData);

/**
 * Register a status indication callback.
 * @note There may be multiple clients connected.
 *
 * @param statusIndication Callback function.
 * @param callbackContext Context pointer, will be sent back in callback.
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR. 
 */
cbRTSL_Status cbWLAN_registerStatusCallback(cbWLAN_statusIndication statusIndication, void *callbackContext);


/**
 * Register a status indication callback.
 *
 * @param packetIndication Callback function.
 * @param callbackContext Context pointer, will be sent back in callback.
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR. 
 */
cbRTSL_Status cbWLAN_registerPacketIndicationCallback(cbWLAN_packetIndication packetIndication, void *callbackContext);

/**
 * Deregister the specified status indication callback.
 *
 * @param statusIndication Callback function.
 * @param callbackContext Context pointer, will be sent back in callback.
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR. 
 */
cbRTSL_Status cbWLAN_deregisterStatusCallback(cbWLAN_statusIndication statusIndication, void *callbackContext);


cbRTSL_Status cbWLAN_Util_PSKFromPWD(cb_char passphrase[cbWLAN_MAX_PASSPHRASE_LENGTH], cbWLAN_Ssid ssid, cb_uint8 psk[cbWLAN_PSK_LENGTH]);

/**
 * Set the channel list to be used for connection and scanning.
 * The list will be filtered according to the allowed channel list
 * set. The list can include both 2.4GHz and 5GHz channels.
 * If channel list parameter is NULL the default channel list is 
 * restored.
 * 
 * @param channelList Pointer to channel list for the driver to use.
 *
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR.
 */
cbRTSL_Status cbWLAN_setChannelList(const cbWLAN_ChannelList *channelList);

/**
 * Returns the wanted channel list.
 *
 * @param channelList Pointer to channel list
 *
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR.
 */
cbRTSL_Status cbWLAN_getChannelList(cbWLAN_ChannelList *channelList);

/**
 * Returns the channel list currently used. This channel list
 * depend on the channel list specified by the user and the 
 * current regulatory domain.
 *
 * @param channelList Pointer to channel list
 * 
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR.
 */
cbRTSL_Status cbWLAN_getActiveChannelList(cbWLAN_ChannelList *channelList);

/**
 * WLAN control settings. Both in and out parameters are supported. 
 * If an ioctl request is not supported cbSTATUS_ERROR is returned and 
 * the value parameter shall be ignored.
 *
 * @param ioctl Parameter that shall be set. @ref cbWLAN_Ioctl lists all supported parameters.
 * @param value Value. @ref cbWLAN_Ioctl lists the type for all supported parameters.
 *
 * @return @ref cbSTATUS_OK if call successful, otherwise cbSTATUS_ERROR.
 */
cbRTSL_Status cbWLAN_ioctl(cbWLAN_Ioctl ioctl, void* value);

#ifdef __cplusplus
}
#endif

#endif /* _CB_WLAN_H_ */

