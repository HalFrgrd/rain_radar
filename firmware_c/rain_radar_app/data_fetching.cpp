#include "data_fetching.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include <string>

#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/platform.h"
#include "lwip/pbuf.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/dns.h"

#include "pico/async_context.h"
#include "http_client_util.hpp"
#include "rain_radar_common.hpp"
#include "wifi_setup.hpp"
#include "psram_display.hpp"
#include "inky_frame_7.hpp"
#include "pico/types.h"

#define HOST "muse-hub.taile8f45.ts.net"

namespace data_fetching
{

    struct ImageWriterHelper
    {
        pimoroni::PSRamDisplay &psram_display;
        size_t const max_address_write;
        size_t offset = 0;
        ImageHeader image_header;
        Err result;

        ImageWriterHelper(pimoroni::InkyFrame &inky_frame)
            : psram_display(inky_frame.ramDisplay)
            , max_address_write(inky_frame.width * inky_frame.height)
            , offset(0)
            , image_header()
            , result(Err::OK)
        {
        }
    };


    err_t image_data_callback_fn(void *_arg, __unused struct altcp_pcb *conn, struct pbuf *p, err_t err)
    {
        if (err != ERR_OK || p == NULL)
        {
            printf("Error in image_data_callback_fn: %d\n", err);
            return err;
        }

        ImageWriterHelper *image_writer = (ImageWriterHelper *)_arg;

        // TODO: handle pbuf chains
        size_t body_len = p->len;
        const uint8_t *body_data = (const uint8_t *)p->payload;

        if (image_writer->offset < 32) {
            assert(image_writer->offset==0);
            assert(body_len >= 32);
            // parse header
            int32_t magic_number = *((int32_t*)body_data);
            int8_t version = *((int8_t*)(body_data+4));
            int64_t update_ts = *((int64_t*)(body_data+6));
            int8_t next_wakeup_hours = *((int8_t*)(body_data+14));
            int8_t next_wakeup_minutes = *((int8_t*)(body_data+15));
            printf("Image header: magic=0x%08X version=%d update_ts=%lld next_wakeup=%02d:%02d\n",
                magic_number, version, update_ts, next_wakeup_hours, next_wakeup_minutes);
            image_writer->image_header.magic_number = magic_number;
            image_writer->image_header.update_ts = update_ts;
            image_writer->image_header.next_wakeup_hours = next_wakeup_hours;
            image_writer->image_header.next_wakeup_minutes = next_wakeup_minutes;
            body_data += 32;
            body_len -= 32;
            image_writer->offset += 32;

        }

        // Ive had to modify PSRamDisplay to make the write function and pointToAddress public
        size_t offset = image_writer->offset;
        
        size_t payload_offset = offset + body_len;
        if (payload_offset > image_writer->max_address_write)
        {
            printf("Image data exceeds display size\n");
            return ERR_BUF;
        }
        image_writer->psram_display.write_span(offset, body_len, body_data);
        image_writer->offset = payload_offset;

        // https://forums.raspberrypi.com/viewtopic.php?t=385648
        altcp_recved(conn, body_len);
        pbuf_free(p);

        return ERR_OK;
    }

    void result_fn(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err)
    {
        // httpc_result is already passed as req->result.
        // set arg to result
        ImageWriterHelper *image_writer = (ImageWriterHelper *)arg;
        image_writer->result = httpStatusToErr(srv_res);
    }

    ResultOr<ImageHeader> fetch_image(pimoroni::InkyFrame &inky_frame, int8_t connected_ssid_index)
    {
        printf("Fetching image for SSID index %d\n", connected_ssid_index);

        if (!wifi_setup::is_connected())
        {
            printf("Not connected to WiFi!\n");
            return Err::NO_CONNECTION;
        }

        http_client_util::http_req_t req = {0};
        req.hostname = HOST;
        std::string url_str = "/" + std::to_string(connected_ssid_index) + "/quantized.bin";
        req.url = url_str.c_str();
        printf("Requesting URL: %s from %s\n", req.url, req.hostname);

        ImageWriterHelper image_writer(inky_frame);

        req.callback_arg = &image_writer;

        req.headers_fn = http_client_util::http_client_header_print_fn;
        req.recv_fn = image_data_callback_fn;
        /* No CA certificate checking */
        struct altcp_tls_config *tls_config = altcp_tls_create_config_client(NULL, 0);
        assert(tls_config);
        req.tls_config = tls_config; // setting tls_config enables https

        req.result_fn = result_fn;

        int result = http_client_util::http_client_request_sync(cyw43_arch_async_context(), &req);
        altcp_tls_free_config(tls_config);

        if (image_writer.result != Err::OK)
        {
            return image_writer.result;
        }

        if (result)
        {
            return Err::ERROR;
        }

        ImageHeader image_header = image_writer.image_header;
        if (image_header.magic_number != data_fetching::MAGIC_NUMBER)
        {
            printf("Bad image header\n");
            return Err::COULDNT_PARSE_HEADER;
        }

        return ResultOr<ImageHeader>(image_header);
    }

}