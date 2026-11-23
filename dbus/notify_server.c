/* D-Bus 알림 메시지 서버 
 * 파일 이름: notify_server.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>
 
#define SERVICE   "org.example.Notifier"
#define OBJ_PATH  "/org/example/Notifier"
#define INTERFACE "org.example.Notifier"
 
static uint32_t count = 0;
static sd_bus *g_bus = NULL;      /* 콜백에서 시그널을 방송하기 위해 보관 */
 
/* Notify 메소드 콜백 (dbusserver.c 의 hello) */
static int notify(sd_bus_message *m, void *userdata, sd_bus_error *error)
{
    const char *title, *body;
    int ret;
 
    ret = sd_bus_message_read(m, "ss", &title, &body);
    if (ret < 0) {
        fprintf(stderr, "Failed to parse arguments\n");
        return ret;
    }
    ++count;
    printf("[server] #%u\n         title: %s\n         body : %s\n", count, title, body);
    fflush(stdout);
 
    /* 알림을 구독 중인 모든 클라이언트에게 시그널 방송 (dbusserver.c 의 Tick) */
    ret = sd_bus_emit_signal(g_bus, OBJ_PATH, INTERFACE,
                             "Notified", "uss", count, title, body);
    if (ret < 0)
        fprintf(stderr, "Failed to emit signal: %s\n", strerror(-ret));
 
    return sd_bus_reply_method_return(m, "u", count);
}
 
/* Count 속성 (dbusserver.c 의 get_count) */
static int get_count(sd_bus *bus, const char *path, const char *interface,
                     const char *property, sd_bus_message *reply,
                     void *userdata, sd_bus_error *error)
{
    uint32_t *pcount = (uint32_t *)userdata;
    return sd_bus_message_append(reply, "u", *pcount);
}
 
/* 인터페이스를 만드는 매크로 (dbusserver.c 와 같은 방식) */
static const sd_bus_vtable notifier_vtable[] = {
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD_WITH_ARGS("Notify",
        SD_BUS_ARGS("s", title, "s", body),
        SD_BUS_RESULT("u", id),
        notify, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_PROPERTY("Count", "u", get_count, 0, 0),   /* 값이 바뀌므로 CONST 아님 */
    SD_BUS_SIGNAL("Notified", "uss", 0),
    SD_BUS_VTABLE_END
};
 
int main()
{
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;
    int ret = 0;
 
    /* session 버스에 연결 */
    ret = sd_bus_default_user(&bus);
    if (ret < 0) {
        fprintf(stderr, "Failed to connect to dbus-daemon: %s\n", strerror(-ret));
        goto finish;
    }
    g_bus = bus;
    /* Bus Name 요청 */
    ret = sd_bus_request_name(bus, SERVICE, 0);
    if (ret < 0) {
        fprintf(stderr, "Failed to acquire name: %s\n", strerror(-ret));
        goto finish;
    }
    /* Object Path 와 Interface 등록 */
    ret = sd_bus_add_object_vtable(bus, &slot, OBJ_PATH, INTERFACE, notifier_vtable, &count);
    if (ret < 0) {
        fprintf(stderr, "Failed to add object interface: %s\n", strerror(-ret));
        goto finish;
    }
    printf("[server] owns %s\n", SERVICE);
    fflush(stdout);
 
    for (;;) {
        ret = sd_bus_process(bus, NULL);
        if (ret < 0) {
            fprintf(stderr, "Failed to process bus: %s\n", strerror(-ret));
            break;
        }
        if (ret > 0)            /* 처리한 메시지가 있으면 바로 다음 것 처리 */
            continue;
        ret = sd_bus_wait(bus, (uint64_t)-1);   /* 메시지가 올 때까지 대기 */
        if (ret < 0) {
            fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-ret));
            break;
        }
    }
 
finish:
    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);
    return ret < 0 ? 1 : 0;
}
