include $(LITEOSTOPDIR)/config.mk

MODULE_NAME := $(LITEOS_PLATFORM)

-include $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/config.mk

USER_SRC = \
    os_adapt/os_adapt.c \
    Src/main.c \
    Src/system_gd32f4xx.c \
    Src/gpio.c \
    Src/tim.c \
    Src/usart.c \
    Src/user_task.c \
    Src/SEGGER_RTT_printf.c \
    Src/SEGGER_RTT.c \
    Src/toolkit.c \
    Src/i2c.c \
    Src/queue.c \
    Src/sensor.c \
    Src/modbus.c \
    Src/dog.c \
    Src/esp32c3.c


C_SOURCES += $(USER_SRC)

GD_DRIVER_INC = \
    -I $(LITEOSTOPDIR)/targets/bsp/drivers/GD32F4xx_Firmware/GD32F4xx_Firmware_Library/Firmware/GD32F4xx_standard_peripheral/Include
BOARD_INCLUDES += $(GD_DRIVER_INC)

INCLUDE_INC = \
    -I $(LITEOSTOPDIR)/include
BOARD_INCLUDES += $(INCLUDE_INC)

BSP_INC = \
    -I $(LITEOSTOPDIR)/targets/bsp/common \
    -I $(LITEOSTOPDIR)/targets/bsp/include \
    -I $(LITEOSTOPDIR)/targets/bsp/hw/include \
    -I $(LITEOSTOPDIR)/targets/GD32F470IGH6/include \
    -I $(LITEOSTOPDIR)/kernel/extended/include \
    -I $(LITEOSTOPDIR)/kernel/include
BOARD_INCLUDES += $(BSP_INC)

USER_INC = \
    -I $(LITEOSTOPDIR)/targets/GD32F470IGH6/Inc
BOARD_INCLUDES += $(USER_INC)

# C defines
C_DEFS += \
    -DUSE_HAL_DRIVER \
    -DGD32F470 \
    -DNDEBUG \
    -D__LITEOS__ \
    -D_ALL_SOURCE \
    -DUSE_STDPERIPH_DRIVER

ASM_SOURCES = \
    ${wildcard los_startup_gcc.S}

BOARD_SRCS += $(ASM_SOURCES)
BOARD_SRCS += $(C_SOURCES)

LOCAL_SRCS := $(subst $(CURDIR)/,, $(BOARD_SRCS))

BOARD_DEF += $(C_DEFS)
LOCAL_INCLUDE += $(BOARD_INCLUDES)

LOCAL_FLAGS := $(LITEOS_CFLAGS_INTERWORK) $(BOARD_DEF) $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS)

include $(MODULE)
