#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/usb.h>

#define EXIT_SUCCESS        0
#define EXIT_FAILURE        1
#define IS_NEW_METHOD_USED  0
#define M_VENDOR_ID         ( 0x12d1 )
#define M_PRODUCT_ID        ( 0x107e )

#define PRINT_USB_INTERFACE_DESCRIPTOR( i )                         \
{                                                                   \
    pr_info("USB_INTERFACE_DESCRIPTOR:\n");                         \
    pr_info("-----------------------------\n");                     \
    pr_info("bLength: 0x%x\n", i.bLength);                          \
    pr_info("bDescriptorType: 0x%x\n", i.bDescriptorType);          \
    pr_info("bInterfaceNumber: 0x%x\n", i.bInterfaceNumber);        \
    pr_info("bAlternateSetting: 0x%x\n", i.bAlternateSetting);      \
    pr_info("bNumEndpoints: 0x%x\n", i.bNumEndpoints);              \
    pr_info("bInterfaceClass: 0x%x\n", i.bInterfaceClass);          \
    pr_info("bInterfaceSubClass: 0x%x\n", i.bInterfaceSubClass);    \
    pr_info("bInterfaceProtocol: 0x%x\n", i.bInterfaceProtocol);    \
    pr_info("iInterface: 0x%x\n", i.iInterface);                    \
    pr_info("\n");                                                  \
}

#define PRINT_USB_ENDPOINT_DESCRIPTOR( e )                          \
{                                                                   \
    pr_info("USB_ENDPOINT_DESCRIPTOR:\n");                          \
    pr_info("------------------------\n");                          \
    pr_info("bLength: 0x%x\n", e.bLength);                          \
    pr_info("bDescriptorType: 0x%x\n", e.bDescriptorType);          \
    pr_info("bEndPointAddress: 0x%x\n", e.bEndpointAddress);        \
    pr_info("bmAttributes: 0x%x\n", e.bmAttributes);                \
    pr_info("wMaxPacketSize: 0x%x\n", e.wMaxPacketSize);            \
    pr_info("bInterval: 0x%x\n", e.bInterval);                      \
    pr_info("\n");                                                  \
}


static int m_usbdrv_probe(struct usb_interface *i, const struct usb_device_id *id)
{
  unsigned int j;
  unsigned int m_endpoints_cnt;
  struct usb_host_interface *i_desc = i->cur_altsetting;
  dev_info(&i->dev, "USB driver probed\nVENDOR_ID:0x%02x\n"
           "PRODUCT_ID:0x%02x\n", id->idVendor, id->idProduct);
  m_endpoints_cnt = i_desc->desc.bNumEndpoints;
  PRINT_USB_INTERFACE_DESCRIPTOR(i_desc->desc);

  for (j = 0; j < m_endpoints_cnt; j++) {
    PRINT_USB_ENDPOINT_DESCRIPTOR( i_desc->endpoint[j].desc);
  }
  return EXIT_SUCCESS;
}

static void m_usbdrv_disconnect(struct usb_interface *intf)
{
  dev_info(&intf->dev, "USB driver disconnected\n");
}

const struct usb_device_id m_usbdrv_id_table[] = {
  { USB_DEVICE( M_VENDOR_ID , M_PRODUCT_ID ) },
  { },
};

MODULE_DEVICE_TABLE(m_usb, m_usbdrv_id_table);

static struct usb_driver m_usbdrv = {
  .name       = "m_usbdrv_kernel",
  .probe      = m_usbdrv_probe,
  .disconnect = m_usbdrv_disconnect,
  .id_table   = m_usbdrv_id_table,
};

#if (IS_NEW_METHOD_USED == 1)
module_usb_driver(m_usbdrv);
#else
static int __init m_usbdrv_init(void)
{
  usb_register(&m_usbdrv);
  return EXIT_SUCCESS;
}

static void __exit m_usbdrv_exit(void)
{
  usb_deregister(&m_usbdrv);
}

module_init(m_usbdrv_init);
module_exit(m_usbdrv_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sha");
MODULE_DESCRIPTION("USB driver module");
MODULE_VERSION("1.0.34");