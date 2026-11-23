/* D-Bus 알림 메시지 클라이언트
 * 파일 이름: notify_client.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>
 
#define SERVICE   "org.example.Notifier"
#define OBJ_PATH  "/org/example/Notifier"
#define INTERFACE "org.example.Notifier"
 
/* Notified 시그널 콜백 (dbusclient.c 의 on_tick) */
static int on_notified(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
    uint32_t id = 0;
    const char *title, *body;
    int ret = sd_bus_message_read(m, "uss", &id, &title, &body);
    if (ret < 0)
        return ret;
    printf("[listen] #%u %s: %s\n", id, title, body);
    fflush(stdout);
    return 0;
}
 
int main(int argc, char *argv[])
{
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    sd_bus *bus = NULL;
    sd_bus_slot *sigslot = NULL;
    uint32_t id = 0, count = 0;
    int ret = 0;
 
    if (argc < 2) {
        fprintf(stderr, "Usage: notify_client send title body | listen [sec]\n");
        return 1;
    }
    /* session 버스에 연결 */
    ret = sd_bus_default_user(&bus);
    if (ret < 0) {
        fprintf(stderr, "Failed to connect to dbus-daemon: %s\n", strerror(-ret));
        goto finish;
    }
 
    if (strcmp(argv[1], "send") == 0 && argc == 4) {
        /* D-Bus 메소드 호출 */
        ret = sd_bus_call_method(bus, SERVICE, OBJ_PATH, INTERFACE, "Notify",
                                 &error, &m, "ss", argv[2], argv[3]);
        if (ret < 0) {
            fprintf(stderr, "Failed to issue method call: %s\n", error.message);
            goto finish;
        }
        /* 성공 메시지를 읽어서 return 값 획득 */
        ret = sd_bus_message_read(m, "u", &id);
        if (ret < 0) {
            fprintf(stderr, "Failed to parse response message\n");
            goto finish;
        }
        /* 속성 획득 */
        ret = sd_bus_get_property_trivial(bus, SERVICE, OBJ_PATH, INTERFACE, "Count",
                                          &error, 'u', &count);
        if (ret < 0) {
            fprintf(stderr, "Failed to get property: %s\n", error.message);
            goto finish;
        }
        printf("[client] notification accepted, id=%u (server Count=%u)\n", id, count);
    }
    else if (strcmp(argv[1], "listen") == 0) {
        int sec = argc > 2 ? atoi(argv[2]) : 30;
        /* 신호 수신: Notified */
        ret = sd_bus_add_match(bus, &sigslot,
                "type='signal',interface='" INTERFACE "',member='Notified',path='" OBJ_PATH "'",
                on_notified, NULL);
        if (ret < 0) {
            fprintf(stderr, "Failed to add match: %s\n", strerror(-ret));
            goto finish;
        }
        printf("[listen] waiting Notified signals for ~%d seconds...\n", sec);
        fflush(stdout);
        for (int i = 0; i < sec * 10; i++) {
            ret = sd_bus_process(bus, NULL);
            if (ret < 0) {
                fprintf(stderr, "sd_bus_process: %s\n", strerror(-ret));
                goto finish;
            }
            if (ret == 0)
                sd_bus_wait(bus, (uint64_t)(100 * 1000));   /* 100ms */
        }
    }
    else {
        fprintf(stderr, "unknown command\n");
        ret = -1;
    }
 
finish:
    sd_bus_slot_unref(sigslot);
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
    sd_bus_unref(bus);
    return ret < 0 ? 1 : 0;
}
