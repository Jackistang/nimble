/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2022-05-20     Jackistang   first implementation
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sysinit/sysinit.h>
#include <nimble/ble.h>
#include <nimble/transport.h>
#include <nimble/transport/hci_h4.h>
#include <rtthread.h>
#include <rtdevice.h>

static struct hci_h4_sm g_hci_h4sm;
static rt_device_t g_hci_uart;
static rt_thread_t g_hci_tid;

static int
rtthread_ble_hci_frame_cb(uint8_t pkt_type, void *data)
{
    int rc;

    switch (pkt_type)
    {
    case HCI_H4_ACL:
        rc = ble_transport_to_hs_acl(data);
        break;
    case HCI_H4_EVT:
        rc = ble_transport_to_hs_evt(data);
        break;
    default:
        assert(0);
        break;
    }

    return rc;
}

static void
rtthread_ble_hci_uart_write_all(const uint8_t *buf, size_t size)
{
    size_t remaining = size;
    size_t write_len;

    while (remaining > 0)
    {
        write_len = rt_device_write(g_hci_uart, 0, buf, remaining);
        buf += write_len;
        remaining -= write_len;
    }
}

int
ble_transport_to_ll_cmd_impl(void *buf)
{
    assert(g_hci_uart);

    uint8_t ind = HCI_H4_CMD;
    uint8_t *cmd = buf;
    int len = 3 + cmd[2];

    rtthread_ble_hci_uart_write_all(&ind, 1);
    rtthread_ble_hci_uart_write_all(cmd, len);

    ble_transport_free(buf);

    return 0;
}

int
ble_transport_to_ll_acl_impl(struct os_mbuf *om)
{
    assert(g_hci_uart);

    uint8_t ind = HCI_H4_ACL;
    struct os_mbuf *x;

    rtthread_ble_hci_uart_write_all(&ind, 1);

    x = om;
    while (x)
    {
        rtthread_ble_hci_uart_write_all(x->om_data, x->om_len);
        x = SLIST_NEXT(x, om_next);
    }

    os_mbuf_free_chain(om);

    return 0;
}

#ifndef ARRAY_SIZE
    #define ARRAY_SIZE(array) (sizeof((array)) - sizeof((array)[0]))
#endif

static void
rtthread_hci_uart_entry(void *args)
{
    uint8_t rx[64];
    size_t len;

    while (1)
    {
        len = rt_device_read(g_hci_uart, 0, rx, ARRAY_SIZE(rx));
        if (len > 0) {
            hci_h4_sm_rx(&g_hci_h4sm, rx, len);
        }

        rt_thread_mdelay(1);
    }
}

static int
rtthread_hci_uart_init(void)
{
    int rc;
    /*
    TODO: 根据宏配置串口
        MYNEWT_VAL(BLE_TRANSPORT_UART_PORT)
        MYNEWT_VAL(BLE_TRANSPORT_UART_BAUDRATE)
        MYNEWT_VAL(BLE_TRANSPORT_UART_DATA_BITS)
        MYNEWT_VAL(BLE_TRANSPORT_UART_STOP_BITS)

        MYNEWT_VAL_CHOICE(BLE_TRANSPORT_UART_PARITY, odd)
        MYNEWT_VAL_CHOICE(BLE_TRANSPORT_UART_PARITY, even)
        MYNEWT_VAL_CHOICE(BLE_TRANSPORT_UART_PARITY, none)

        MYNEWT_VAL_CHOICE(BLE_TRANSPORT_UART_FLOW_CONTROL, rtscts)
        MYNEWT_VAL_CHOICE(BLE_TRANSPORT_UART_FLOW_CONTROL, off)
     */

    g_hci_uart = rt_device_find("uart1");
    if (NULL == g_hci_uart)
    {
        return -1;
    }

    rc = rt_device_open(g_hci_uart, RT_DEVICE_FLAG_INT_RX);
    if (-RT_EBUSY == rc)
    {
        return -1;
    }

    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    rc = rt_device_control(g_hci_uart, RT_DEVICE_CTRL_CONFIG, &config);
    if (-RT_EBUSY == rc)
    {
        return -1;
    }

    // 创建串口接收线程
    g_hci_tid = rt_thread_create("hci", rtthread_hci_uart_entry, RT_NULL, 1024, 20, 5);
    assert(g_hci_tid != RT_NULL);

    rt_thread_startup(g_hci_tid);

    return 0;
}

int
ble_transport_rtthread_init(void)
{
    int rc;

    hci_h4_sm_init(&g_hci_h4sm, &hci_h4_allocs_from_ll,
                   rtthread_ble_hci_frame_cb);
    
    rc = rtthread_hci_uart_init();
    if (-1 == rc) {
        printf("NimBLE transport rtthread init failed\n");
        return -1;
    }

    printf("NimBLE transport rtthread init success\n");
    return 0;
}
INIT_COMPONENT_EXPORT(ble_transport_rtthread_init);