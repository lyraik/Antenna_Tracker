/*********************************Defines******************************************/

#define SPP_TAG "bt"
// Name des Gerätes
#define DEVICE_NAME "Antenna Tracker"

#define SPP_SHOW_DATA 0
#define SPP_SHOW_MODE SPP_SHOW_Data /*Choose show mode: show data or speed*/
#define SPP_DATA_LEN 20

#define CONFIG_BTDM_CONTROLLER_MODE_BR_EDR_ONLY

/*************************includes*************************/
#include <etl/vector.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_log.h"
#include "esp_spp_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "utils/String.h"

#include "sys/time.h"
#include "time.h"

#include <mavlink/common/mavlink.h>

namespace bt {

    static constexpr size_t DISCOVER_DEVICE_COUNT = 20;

    /******************************Attributes********************************************/

    // Kein VFS -> Wenn Daten ankommen wird ein Callback aufgerufen!
    static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;

    // Sicherheits-stufe -> Keine Sicherheit
    static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_NONE;
    // Rolle ist Master
    static const esp_spp_role_t role_master = ESP_SPP_ROLE_MASTER;

    // Gelesener Name vom Gerät -> wird mit dem remote_device name verglichen um zu sehen ob eine Verbindung aufgebaut werden soll
    // static char peer_bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1];
    // Erwartete uid vom fernen Gerät
    extern utils::String remoteDeviceUid;
    // Erwarterter Name von Gerät, wenn remoteDeviceUid leer ist
    static constexpr utils::StringView defaultDeviceName = "Crossfire 2771";
    // Anfrage modus: Art der Anfrage
    static const esp_bt_inq_mode_t inq_mode = ESP_BT_INQ_MODE_GENERAL_INQUIRY;

    // Afragelänge
    static const uint8_t inq_len = 30;
    // Anzahl Wiederholungen bei der Anfrage
    static const uint8_t inq_num_rsps = 0;

    // Char Array für Datenstring
    static uint8_t spp_data[SPP_DATA_LEN];

    extern mavlink_message_t msg;
    extern mavlink_status_t status;

    extern float lattitude;
    extern float longitude;
    extern etl::vector<utils::String, DISCOVER_DEVICE_COUNT> devices;
    extern etl::vector<utils::String, DISCOVER_DEVICE_COUNT> deviceUids;

    /************************************API's******************************************/

    // Task for Bluetooth handeling
    void bluetooth_task(void* params);

    // Callback function, when bluedroid rises a gap event
    void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* param);

    // Extracts the values from the eir response
    bool getEirValues(uint8_t* eir, char* bdname, uint8_t* bdname_len);

    // Callback function, when bluedroid rises a spp event
    void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t* param);

    // enables and initialises the Bluetooth stack and sets the params
    void init();

    /**
     * @brief Connects to the current device specified in remoteDeviceName.
     */
    void connect(const utils::StringView& deviceUid);

}  // namespace bt