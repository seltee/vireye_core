#pragma once
#include <stm32f10x.h>

enum RESULT
{
  USB_SUCCESS = 0,
  USB_ERROR,
  USB_UNSUPPORT,
  USB_NOT_READY
};

typedef union
{
  uint16_t w;
  struct BW
  {
    uint8_t bb1;
    uint8_t bb0;
  }bw;
} uint16_t_uint8_t;

/*-*-*-*-*-*-*-*-*-*-* Definitions for endpoint level -*-*-*-*-*-*-*-*-*-*-*-*/
struct EndpointInfo
{
  uint16_t  Usb_wLength;
  uint16_t  Usb_wOffset;
  uint16_t  PacketSize;
  uint8_t   *(*CopyData)(uint16_t Length);
};

struct DeviceInfo
{
  uint8_t USBbmRequestType;       			// bmRequestType
  uint8_t USBbRequest;            			// bRequest
  uint16_t_uint8_t USBwValues;         	// wValue
  uint16_t_uint8_t USBwIndexs;         	// wIndex
  uint16_t_uint8_t USBwLengths;        	// wLength
  uint8_t ControlState;           			// of type CONTROL_STATE
  uint8_t Current_Feature;
  uint8_t Current_Configuration;   			// Selected configuration
  uint8_t Current_Interface;       			// Selected interface of current configuration
  uint8_t Current_AlternateSetting;			// Selected Alternate Setting of current interface
  EndpointInfo Ctrl_Info;
};

struct DeviceProp
{
  void (*Init)(void);        /* Initialize the device */
  void (*Reset)(void);       /* Reset routine of this device */
  void (*Process_Status_IN)(void);
  void (*Process_Status_OUT)(void);
  RESULT (*Class_Data_Setup)(uint8_t RequestNo);
  RESULT (*Class_NoData_Setup)(uint8_t RequestNo);
  RESULT  (*Class_Get_Interface_Setting)(uint8_t Interface, uint8_t AlternateSetting);

  uint8_t* (*GetDeviceDescriptor)(uint16_t Length);
  uint8_t* (*GetConfigDescriptor)(uint16_t Length);
  uint8_t* (*GetStringDescriptor)(uint16_t Length);

  void* RxEP_buffer;
   
  uint8_t MaxPacketSize;
};

struct UserStandartRequests
{
  void (*User_GetConfiguration)(void);       /* Get Configuration */
  void (*User_SetConfiguration)(void);       /* Set Configuration */
  void (*User_GetInterface)(void);           /* Get Interface */
  void (*User_SetInterface)(void);           /* Set Interface */
  void (*User_GetStatus)(void);              /* Get Status */
  void (*User_ClearFeature)(void);           /* Clear Feature */
  void (*User_SetEndPointFeature)(void);     /* Set Endpoint Feature */
  void (*User_SetDeviceFeature)(void);       /* Set Device Feature */
  void (*User_SetDeviceAddress)(void);       /* Set Device Address */
};

DeviceInfo *getDeviceInfo();



