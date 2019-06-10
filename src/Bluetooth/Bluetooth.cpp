#include "Bluetooth.h"

namespace bluetooth {

    
    float lattitude = 0;
     float longitude = 0;


    mavlink_message_t msg;
    mavlink_status_t status;

    // Hole Name von der Geräteantwort und schreibt ihn nach bdname
    bool getEirValues(uint8_t *eir, char *bdname, uint8_t *bdname_len) {
        // Lokaler name vom remote device
        uint8_t *rmt_bdname = NULL;
        // Länge des Namens
        uint8_t rmt_bdname_len = 0;

        if (!eir) {
            return false;
        }

        // Erhalte eir(extended inquiry response) data für ein spezifischer Typ (eir enthaltet informationen wie: device name, service class UUIDs für jeden Service, transmission
        // power, manufacturer specific values)
        rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
        // Der Name (ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME) wird vom Core angegeben
        if (!rmt_bdname) {
            rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
        }
        // Name kürzen wenn nötig
        if (rmt_bdname) {
            if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
                rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
            }

            if (bdname) {
                // Kopiere rmt_bdname nach bdname
                memcpy(bdname, rmt_bdname, rmt_bdname_len);
                // Arrayende kennzeichnen
                bdname[rmt_bdname_len] = '\0';
            }
            if (bdname_len) {
                *bdname_len = rmt_bdname_len;
            }
            return true;
        }

        return false;
    }

    // Register Callback Funktion (spekulation: wird vom Core aufgerufen, wenn ein Spp Event beginnt
    void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
        // Wert vom Event wird abgefragt
        switch (event) {
            // Wenn spp initialisiert wurde
        case ESP_SPP_INIT_EVT:
            // Schreibe Init Event seriell heraus
            ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
            // example device name ist esp spp initiator (Name für spp verbundug)
            esp_bt_dev_set_device_name(DEVICE_NAME);
            esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
            // Starte Suche nach Geräten mit (allen,30, 0 Wiederholungen)
            esp_bt_gap_start_discovery(inq_mode, inq_len, inq_num_rsps);

            break;
            // Suche abgeschlossen
        case ESP_SPP_DISCOVERY_COMP_EVT:
            ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT status=%d scn_num=%d", param->disc_comp.status, param->disc_comp.scn_num);
            // param : event parameter suche kompltett ist ein Erfolg?
            if (param->disc_comp.status == ESP_SPP_SUCCESS) {
                esp_spp_connect(sec_mask, role_master, param->disc_comp.scn[0], peer_bd_addr);
            }
            break;
        case ESP_SPP_OPEN_EVT:
            ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
            // Schreibe Daten heraus: Daten mit der Länge spp_data_len & dem Pointer zu spp_data
            // esp_spp_write(param->srv_open.handle, SPP_DATA_LEN, spp_data);
            break;
        case ESP_SPP_CLOSE_EVT:
            ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
            break;
        case ESP_SPP_START_EVT:
            ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
            break;
        case ESP_SPP_CL_INIT_EVT:
            ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
            break;
        case ESP_SPP_DATA_IND_EVT:

            for (int i = 0; i < param->data_ind.len; i++) {
                if (mavlink_parse_char(1, *(param->data_ind.data + i), &msg, &status)) {
                    // Wenn die MessageId 33 ist (Message mit ID 33 sind GPS-Daten) -> Die Daten ins Terminal herausschreiben
                    if (msg.msgid == 33) {
                        // Lattitude und Longitude ins Terminal herausschreiben
                        ESP_LOGI(SPP_TAG, "Lattitude: \t %d", mavlink_msg_global_position_int_cov_get_lat(&msg));
                        ESP_LOGI(SPP_TAG, "Longitude: \t %d", mavlink_msg_global_position_int_cov_get_lon(&msg));
                        // Schreibe den Höhenwert ins Servo ! muss noch auf einen Winkel angepasst werden
                    }
                    // Wenn die ID 30 ist (= pitch, yaw, roll Werte der Drohne)
                    else if (msg.msgid == 30) {
                        ESP_LOGI(SPP_TAG, "PITCH: \t %f \n", mavlink_msg_attitude_get_pitch(&msg));
                        ESP_LOGI(SPP_TAG, "ROLL: \t %f \n", mavlink_msg_attitude_get_roll(&msg));
                        ESP_LOGI(SPP_TAG, "YAW: \t %f \n", mavlink_msg_attitude_get_yaw(&msg));
                    }
                     else if (msg.msgid == 0) {

                        ESP_LOGI(SPP_TAG, "Hearbeat");
                    } 
                    else if (msg.msgid == 109)
                    {
                        ESP_LOGI(SPP_TAG, "no device connected");
                    }
                }
            }
            break;
        case ESP_SPP_CONG_EVT:
#if (SPP_SHOW_MODE == SPP_SHOW_DATA)
            ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT cong=%d", param->cong.cong);
#endif
            if (param->cong.cong == 0) {
                //   esp_spp_write(param->cong.handle, SPP_DATA_LEN, spp_data);
            }
            break;

        case ESP_SPP_WRITE_EVT:
#if (SPP_SHOW_MODE == SPP_SHOW_DATA)
            ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT len=%d cong=%d", param->write.len, param->write.cong);
            esp_log_buffer_hex("", spp_data, SPP_DATA_LEN);
#else
            gettimeofday(&time_new, NULL);
            data_num += param->write.len;
            if (time_new.tv_sec - time_old.tv_sec >= 3) {
                print_speed();
            }
#endif
            // Schreibe Daten über Bluetooth heraus
            if (param->write.cong == 0) {
                //      esp_spp_write(param->write.handle, SPP_DATA_LEN, spp_data);
            }
            break;
        case ESP_SPP_SRV_OPEN_EVT:
            ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
            break;
        default:
            break;
        }
    }

    void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
        switch (event) {
            // Discovery Resultat erhalten
        case ESP_BT_GAP_DISC_RES_EVT:
            // Seriell herauschreiben: ein Resultat erhalten!
            ESP_LOGI(SPP_TAG, "ESP_BT_GAP_DISC_RES_EVT");
            // Schreibe hex Werte heraus (Die empfangene Addresse)
            esp_log_buffer_hex(SPP_TAG, param->disc_res.bda, ESP_BD_ADDR_LEN);
            for (int i = 0; i < param->disc_res.num_prop; i++) {
                if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_EIR
                    // Hole name von param->disc_resp.prop[i].val und schreibe nach peer_bdname
                    && getEirValues(static_cast<uint8_t *>(param->disc_res.prop[i].val), peer_bdname, &peer_bdname_len)) {
                    // Schreibe Name seriell heraus
                    esp_log_buffer_char(SPP_TAG, peer_bdname, peer_bdname_len);
                    if (strlen(remote_device_name) == peer_bdname_len && strncmp(peer_bdname, remote_device_name, peer_bdname_len) == 0) {
                        memcpy(peer_bd_addr, param->disc_res.bda, ESP_BD_ADDR_LEN);
                        // Spp Verbindungssuche mit dem Gerät (ist gewünschtes Gerät)
                        esp_spp_start_discovery(peer_bd_addr);
                        // Stoppe Suche nach neuen Geräten
                        esp_bt_gap_cancel_discovery();
                    }
                }
            }
            break;
            // Durchsuchung angefangen/angehalten
        case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
            ESP_LOGI(SPP_TAG, "ESP_BT_GAP_DISC_STATE_CHANGED_EVT");
            break;
            // Service Durchsuchung hat geendet -> Wird von esp_bt_gap_get_remote_services gesetzt
        case ESP_BT_GAP_RMT_SRVCS_EVT:
            ESP_LOGI(SPP_TAG, "ESP_BT_GAP_RMT_SRVCS_EVT");
            break;
            // Suche nach Service mit matchender uui beendet -> WIrd von esp_bt_gap_get_remote_service_record aufgerufen
        case ESP_BT_GAP_RMT_SRVC_REC_EVT:
            ESP_LOGI(SPP_TAG, "ESP_BT_GAP_RMT_SRVC_REC_EVT");
            break;
            // Gap auth compl event -> Nichts weiteres in der Dokumentation!
        case ESP_BT_GAP_AUTH_CMPL_EVT: {

            if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
                ESP_LOGI(SPP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
                esp_log_buffer_hex(SPP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
            } else {
                ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
            }
            break;
        }
        case ESP_BT_GAP_PIN_REQ_EVT: {
            ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
            if (param->pin_req.min_16_digit) {
                ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
                esp_bt_pin_code_t pin_code = {0};
                esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
            } else {
                ESP_LOGI(SPP_TAG, "Input pin code: 1234");
                esp_bt_pin_code_t pin_code;
                pin_code[0] = '1';
                pin_code[1] = '2';
                pin_code[2] = '3';
                pin_code[3] = '4';
                esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
            }
            break;
        }
#if (CONFIG_BT_SSP_ENABLED == true)
        case ESP_BT_GAP_CFM_REQ_EVT:
            ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
            esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
            break;
        case ESP_BT_GAP_KEY_NOTIF_EVT:
            ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
            break;
        case ESP_BT_GAP_KEY_REQ_EVT:
            ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
            break;
#endif

        default:
            break;
        }
    }

    void enableBluetooth(void) {
        for (int i = 0; i < SPP_DATA_LEN; ++i) {
            spp_data[i] = i;
        }

        ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

        esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
        esp_err_t ret = ESP_OK;
        if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
            ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
            return;
        }

        if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
            ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
            return;
        }

        // Bluedroid initialisieren
        if ((ret = esp_bluedroid_init()) != ESP_OK) {
            ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
            return;
        }
        // bluedroid stack einschalten
        if ((ret = esp_bluedroid_enable()) != ESP_OK) {
            ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
            return;
        }

        // Register Callback Funktion -> Weist eine Funktion den register callback zu; esp_bt_gap_cb wird aufgerufen nachdem bt enable abgeschlossen ist
        if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
            ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
            return;
        }

        // Register callback funktion für Spp
        if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
            ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
            return;
        }

        // spp initialisieren
        if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK) {
            ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
            return;
        }

        /*
         * Set default parameters for Legacy Pairing
         * Use variable pin, input pin code when pairing
         */
        esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
        esp_bt_pin_code_t pin_code;
        esp_bt_gap_set_pin(pin_type, 0, pin_code);
    }

} // namespace bluetooth