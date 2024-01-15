# Makefile for Andes 


PLAT ?= AG101P
ADDR ?= 16MB
MODE ?= LOAD
FEAT ?= ""
DEBU ?= NO
SIMU ?= NO

OPT_AG101 := n
OPT_16MBADR := y
OPT_LLINIT := n
OPT_REMAP  := n
CROSS_COMPILE ?= nds32le-elf-
COC_MOD = /cygdrive/c/Nyquest/NYIDE/bin/tool/nx1-coc.exe
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
OBJ_PATH := OBJ/
LD_PATH  = ./
INC_PATH = -I. \
	-Isrc \
	-Isrc/Driver \
	-Isrc/Kernel \
	-Isrc/Kernel/Audio \
	-Isrc/Kernel/Callback \
	-Isrc/Kernel/System \
	-Isrc/Kernel/Utility \
	-Isrc/Kernel/Utility/IRAM \
	-Isrc/Table \
	-Isrc/Table/IO \
	-Isrc/Table/Midi \
	-Isrc/Table/Voice \
	-Isrc/Table/Midi/Table \
	-Isrc/Table/Vr \
	-Isrc/Table/TouchTab \
	-Isrc/Table/UserDefined \
	-Isrc/Table/LEDSTR \
	-Isrc/user_code
DEPS = $(OBJ_PATH)/src/Driver/eeprom_24LC256.d $(OBJ_PATH)/src/Driver/nx1_adc.d $(OBJ_PATH)/src/Driver/nx1_dac.d $(OBJ_PATH)/src/Driver/nx1_fmc.d $(OBJ_PATH)/src/Driver/nx1_gpio.d $(OBJ_PATH)/src/Driver/nx1_gpio_i2c.d $(OBJ_PATH)/src/Driver/nx1_i2c.d $(OBJ_PATH)/src/Driver/nx1_intc.d $(OBJ_PATH)/src/Driver/nx1_ir.d $(OBJ_PATH)/src/Driver/nx1_pwm.d $(OBJ_PATH)/src/Driver/nx1_rtc.d $(OBJ_PATH)/src/Driver/nx1_sdc.d $(OBJ_PATH)/src/Driver/nx1_smu.d $(OBJ_PATH)/src/Driver/nx1_spi.d $(OBJ_PATH)/src/Driver/nx1_spi1.d $(OBJ_PATH)/src/Driver/nx1_timer.d $(OBJ_PATH)/src/Driver/nx1_uart.d $(OBJ_PATH)/src/Driver/nx1_wdt.d $(OBJ_PATH)/src/Kernel/Audio/adpcm_audioapi.d $(OBJ_PATH)/src/Kernel/Audio/adpcm_chx_audioapi.d $(OBJ_PATH)/src/Kernel/Audio/audio_record_audioapi.d $(OBJ_PATH)/src/Kernel/Audio/audioctl.d $(OBJ_PATH)/src/Kernel/Audio/celp_audioapi.d $(OBJ_PATH)/src/Kernel/Audio/midi_audioapi.d $(OBJ_PATH)/src/Kernel/Audio/midi_inst_noteon.d $(OBJ_PATH)/src/Kernel/Audio/pcm_audioapi.d $(OBJ_PATH)/src/Kernel/Audio/pcm_ch2_audioapi.d $(OBJ_PATH)/src/Kernel/Audio/pcm_ch3_audioapi.d $(OBJ_PATH)/src/Kernel/Audio/rt_audioapi.d $(OBJ_PATH)/src/Kernel/Audio/sbc_audioapi.d $(OBJ_PATH)/src/Kernel/Audio/sbc_ch2_audioapi.d $(OBJ_PATH)/src/Kernel/Audio/sbc2_audioapi.d $(OBJ_PATH)/src/Kernel/Callback/adpcm_callback.d $(OBJ_PATH)/src/Kernel/Callback/adpcm_chx_callback.d $(OBJ_PATH)/src/Kernel/Callback/audio_callback.d $(OBJ_PATH)/src/Kernel/Callback/callback_xip.d $(OBJ_PATH)/src/Kernel/Callback/celp_callback.d $(OBJ_PATH)/src/Kernel/Callback/LVD_monitor_callback.d $(OBJ_PATH)/src/Kernel/Callback/midi_callback.d $(OBJ_PATH)/src/Kernel/Callback/Multi_Channel_ledstr_callback.d $(OBJ_PATH)/src/Kernel/Callback/nx1_i2c_callback.d $(OBJ_PATH)/src/Kernel/Callback/nx1_spi_callback.d $(OBJ_PATH)/src/Kernel/Callback/pcm_callback.d $(OBJ_PATH)/src/Kernel/Callback/pcm_ch2_callback.d $(OBJ_PATH)/src/Kernel/Callback/pcm_ch3_callback.d $(OBJ_PATH)/src/Kernel/Callback/pitch_detect_callback.d $(OBJ_PATH)/src/Kernel/Callback/qfid_callback.d $(OBJ_PATH)/src/Kernel/Callback/qio_callback.d $(OBJ_PATH)/src/Kernel/Callback/reg_callback.d $(OBJ_PATH)/src/Kernel/Callback/rt_callback.d $(OBJ_PATH)/src/Kernel/Callback/sbc_callback.d $(OBJ_PATH)/src/Kernel/Callback/sbc_ch2_callback.d $(OBJ_PATH)/src/Kernel/Callback/sbc2_callback.d $(OBJ_PATH)/src/Kernel/Callback/sbc2_ch2_callback.d $(OBJ_PATH)/src/Kernel/Callback/Scrolling_Text_ledstr_callback.d $(OBJ_PATH)/src/Kernel/Callback/sound_detect_callback.d $(OBJ_PATH)/src/Kernel/Callback/sw_pwm_callback.d $(OBJ_PATH)/src/Kernel/Callback/Sync_Play_ledstr_callback.d $(OBJ_PATH)/src/Kernel/Callback/sys_callback.d $(OBJ_PATH)/src/Kernel/Callback/touch_callback.d $(OBJ_PATH)/src/Kernel/Callback/vio_callback.d $(OBJ_PATH)/src/Kernel/Callback/vrctl_callback.d $(OBJ_PATH)/src/Kernel/Callback/wave_id_callback.d $(OBJ_PATH)/src/Kernel/isr.d $(OBJ_PATH)/src/Kernel/System/debug.d $(OBJ_PATH)/src/Kernel/System/delay.d $(OBJ_PATH)/src/Kernel/System/interrupt.d $(OBJ_PATH)/src/Kernel/System/sys.d $(OBJ_PATH)/src/Kernel/Utility/adc_input.d $(OBJ_PATH)/src/Kernel/Utility/ir.d $(OBJ_PATH)/src/Kernel/Utility/IRAM/crt_iram1.d $(OBJ_PATH)/src/Kernel/Utility/IRAM/record_erasing_iram1.d $(OBJ_PATH)/src/Kernel/Utility/IRAM/vector_iram1.d $(OBJ_PATH)/src/Kernel/Utility/key.d $(OBJ_PATH)/src/Kernel/Utility/LVD_monitor.d $(OBJ_PATH)/src/Kernel/Utility/nx1_dacAssigner.d $(OBJ_PATH)/src/Kernel/Utility/nx1_dacMixerRamp.d $(OBJ_PATH)/src/Kernel/Utility/nx1_math.d $(OBJ_PATH)/src/Kernel/Utility/nx1_qio.d $(OBJ_PATH)/src/Kernel/Utility/nx1_sw_pwm.d $(OBJ_PATH)/src/Kernel/Utility/pitch_detection.d $(OBJ_PATH)/src/Kernel/Utility/prog_checksum.d $(OBJ_PATH)/src/Kernel/Utility/qfid.d $(OBJ_PATH)/src/Kernel/Utility/resource.d $(OBJ_PATH)/src/Kernel/Utility/Sound_Localization.d $(OBJ_PATH)/src/Kernel/Utility/sounddet.d $(OBJ_PATH)/src/Kernel/Utility/storage_access.d $(OBJ_PATH)/src/Kernel/Utility/touch.d $(OBJ_PATH)/src/Kernel/Utility/utilityctl.d $(OBJ_PATH)/src/Kernel/Utility/utilityctl_xip.d $(OBJ_PATH)/src/Kernel/Utility/vadctl.d $(OBJ_PATH)/src/Kernel/Utility/vioctl.d $(OBJ_PATH)/src/Kernel/Utility/vrctl.d $(OBJ_PATH)/src/Kernel/Utility/wave_id.d $(OBJ_PATH)/src/main.d $(OBJ_PATH)/src/mem.d $(OBJ_PATH)/src/Table/Io/IoTab.d $(OBJ_PATH)/src/Table/LEDSTR/LEDSTRTab.d $(OBJ_PATH)/src/Table/NX1_Tab.d $(OBJ_PATH)/src/Table/NX1_VoiceChangerTab.d $(OBJ_PATH)/src/Table/TouchTab/TouchTab.d $(OBJ_PATH)/src/Table/UserDefined/UserDefinedTab.d $(OBJ_PATH)/src/Table/Voice/VoiceTab.d $(OBJ_PATH)/src/Table/Vr/VRCmdTab.d $(OBJ_PATH)/src/user_code/user_key.d $(OBJ_PATH)/src/user_code/user_led.d $(OBJ_PATH)/src/user_code/user_main.d $(OBJ_PATH)/src/user_code/user_motion.d $(OBJ_PATH)/src/user_code/user_play.d $(OBJ_PATH)/src/user_code/user_record.d $(OBJ_PATH)/src/user_code/user_storage_break_point.d $(OBJ_PATH)/src/user_code/user_talkback_adpcm.d $(OBJ_PATH)/src/user_code/user_vr.d $(OBJ_PATH)/src/Table/UserDefined/UserDefinedTab.d $(OBJ_PATH)/init-default.d $(OBJ_PATH)/crt0.d $(OBJ_PATH)/security_empty.d 
OBJS = $(OBJ_PATH)/src/Driver/eeprom_24LC256.o $(OBJ_PATH)/src/Driver/nx1_adc.o $(OBJ_PATH)/src/Driver/nx1_dac.o $(OBJ_PATH)/src/Driver/nx1_fmc.o $(OBJ_PATH)/src/Driver/nx1_gpio.o $(OBJ_PATH)/src/Driver/nx1_gpio_i2c.o $(OBJ_PATH)/src/Driver/nx1_i2c.o $(OBJ_PATH)/src/Driver/nx1_intc.o $(OBJ_PATH)/src/Driver/nx1_ir.o $(OBJ_PATH)/src/Driver/nx1_pwm.o $(OBJ_PATH)/src/Driver/nx1_rtc.o $(OBJ_PATH)/src/Driver/nx1_sdc.o $(OBJ_PATH)/src/Driver/nx1_smu.o $(OBJ_PATH)/src/Driver/nx1_spi.o $(OBJ_PATH)/src/Driver/nx1_spi1.o $(OBJ_PATH)/src/Driver/nx1_timer.o $(OBJ_PATH)/src/Driver/nx1_uart.o $(OBJ_PATH)/src/Driver/nx1_wdt.o $(OBJ_PATH)/src/Kernel/Audio/adpcm_audioapi.o $(OBJ_PATH)/src/Kernel/Audio/adpcm_chx_audioapi.o $(OBJ_PATH)/src/Kernel/Audio/audio_record_audioapi.o $(OBJ_PATH)/src/Kernel/Audio/audioctl.o $(OBJ_PATH)/src/Kernel/Audio/celp_audioapi.o $(OBJ_PATH)/src/Kernel/Audio/midi_audioapi.o $(OBJ_PATH)/src/Kernel/Audio/midi_inst_noteon.o $(OBJ_PATH)/src/Kernel/Audio/pcm_audioapi.o $(OBJ_PATH)/src/Kernel/Audio/pcm_ch2_audioapi.o $(OBJ_PATH)/src/Kernel/Audio/pcm_ch3_audioapi.o $(OBJ_PATH)/src/Kernel/Audio/rt_audioapi.o $(OBJ_PATH)/src/Kernel/Audio/sbc_audioapi.o $(OBJ_PATH)/src/Kernel/Audio/sbc_ch2_audioapi.o $(OBJ_PATH)/src/Kernel/Audio/sbc2_audioapi.o $(OBJ_PATH)/src/Kernel/Callback/adpcm_callback.o $(OBJ_PATH)/src/Kernel/Callback/adpcm_chx_callback.o $(OBJ_PATH)/src/Kernel/Callback/audio_callback.o $(OBJ_PATH)/src/Kernel/Callback/callback_xip.o $(OBJ_PATH)/src/Kernel/Callback/celp_callback.o $(OBJ_PATH)/src/Kernel/Callback/LVD_monitor_callback.o $(OBJ_PATH)/src/Kernel/Callback/midi_callback.o $(OBJ_PATH)/src/Kernel/Callback/Multi_Channel_ledstr_callback.o $(OBJ_PATH)/src/Kernel/Callback/nx1_i2c_callback.o $(OBJ_PATH)/src/Kernel/Callback/nx1_spi_callback.o $(OBJ_PATH)/src/Kernel/Callback/pcm_callback.o $(OBJ_PATH)/src/Kernel/Callback/pcm_ch2_callback.o $(OBJ_PATH)/src/Kernel/Callback/pcm_ch3_callback.o $(OBJ_PATH)/src/Kernel/Callback/pitch_detect_callback.o $(OBJ_PATH)/src/Kernel/Callback/qfid_callback.o $(OBJ_PATH)/src/Kernel/Callback/qio_callback.o $(OBJ_PATH)/src/Kernel/Callback/reg_callback.o $(OBJ_PATH)/src/Kernel/Callback/rt_callback.o $(OBJ_PATH)/src/Kernel/Callback/sbc_callback.o $(OBJ_PATH)/src/Kernel/Callback/sbc_ch2_callback.o $(OBJ_PATH)/src/Kernel/Callback/sbc2_callback.o $(OBJ_PATH)/src/Kernel/Callback/sbc2_ch2_callback.o $(OBJ_PATH)/src/Kernel/Callback/Scrolling_Text_ledstr_callback.o $(OBJ_PATH)/src/Kernel/Callback/sound_detect_callback.o $(OBJ_PATH)/src/Kernel/Callback/sw_pwm_callback.o $(OBJ_PATH)/src/Kernel/Callback/Sync_Play_ledstr_callback.o $(OBJ_PATH)/src/Kernel/Callback/sys_callback.o $(OBJ_PATH)/src/Kernel/Callback/touch_callback.o $(OBJ_PATH)/src/Kernel/Callback/vio_callback.o $(OBJ_PATH)/src/Kernel/Callback/vrctl_callback.o $(OBJ_PATH)/src/Kernel/Callback/wave_id_callback.o $(OBJ_PATH)/src/Kernel/isr.o $(OBJ_PATH)/src/Kernel/System/debug.o $(OBJ_PATH)/src/Kernel/System/delay.o $(OBJ_PATH)/src/Kernel/System/interrupt.o $(OBJ_PATH)/src/Kernel/System/sys.o $(OBJ_PATH)/src/Kernel/Utility/adc_input.o $(OBJ_PATH)/src/Kernel/Utility/ir.o $(OBJ_PATH)/src/Kernel/Utility/IRAM/crt_iram1.o $(OBJ_PATH)/src/Kernel/Utility/IRAM/record_erasing_iram1.o $(OBJ_PATH)/src/Kernel/Utility/IRAM/vector_iram1.o $(OBJ_PATH)/src/Kernel/Utility/key.o $(OBJ_PATH)/src/Kernel/Utility/LVD_monitor.o $(OBJ_PATH)/src/Kernel/Utility/nx1_dacAssigner.o $(OBJ_PATH)/src/Kernel/Utility/nx1_dacMixerRamp.o $(OBJ_PATH)/src/Kernel/Utility/nx1_math.o $(OBJ_PATH)/src/Kernel/Utility/nx1_qio.o $(OBJ_PATH)/src/Kernel/Utility/nx1_sw_pwm.o $(OBJ_PATH)/src/Kernel/Utility/pitch_detection.o $(OBJ_PATH)/src/Kernel/Utility/prog_checksum.o $(OBJ_PATH)/src/Kernel/Utility/qfid.o $(OBJ_PATH)/src/Kernel/Utility/resource.o $(OBJ_PATH)/src/Kernel/Utility/Sound_Localization.o $(OBJ_PATH)/src/Kernel/Utility/sounddet.o $(OBJ_PATH)/src/Kernel/Utility/storage_access.o $(OBJ_PATH)/src/Kernel/Utility/touch.o $(OBJ_PATH)/src/Kernel/Utility/utilityctl.o $(OBJ_PATH)/src/Kernel/Utility/utilityctl_xip.o $(OBJ_PATH)/src/Kernel/Utility/vadctl.o $(OBJ_PATH)/src/Kernel/Utility/vioctl.o $(OBJ_PATH)/src/Kernel/Utility/vrctl.o $(OBJ_PATH)/src/Kernel/Utility/wave_id.o $(OBJ_PATH)/src/main.o $(OBJ_PATH)/src/mem.o $(OBJ_PATH)/src/Table/Io/IoTab.o $(OBJ_PATH)/src/Table/LEDSTR/LEDSTRTab.o $(OBJ_PATH)/src/Table/NX1_Tab.o $(OBJ_PATH)/src/Table/NX1_VoiceChangerTab.o $(OBJ_PATH)/src/Table/TouchTab/TouchTab.o $(OBJ_PATH)/src/Table/UserDefined/UserDefinedTab.o $(OBJ_PATH)/src/Table/Voice/VoiceTab.o $(OBJ_PATH)/src/Table/Vr/VRCmdTab.o $(OBJ_PATH)/src/user_code/user_key.o $(OBJ_PATH)/src/user_code/user_led.o $(OBJ_PATH)/src/user_code/user_main.o $(OBJ_PATH)/src/user_code/user_motion.o $(OBJ_PATH)/src/user_code/user_play.o $(OBJ_PATH)/src/user_code/user_record.o $(OBJ_PATH)/src/user_code/user_storage_break_point.o $(OBJ_PATH)/src/user_code/user_talkback_adpcm.o $(OBJ_PATH)/src/user_code/user_vr.o $(OBJ_PATH)/src/Table/UserDefined/UserDefinedTab.o $(OBJ_PATH)/init-default.o $(OBJ_PATH)/crt0.o $(OBJ_PATH)/security_empty.o 
OBJS := $(sort $(OBJS))
LIBS = OBJ/dVtPIG.tmp OBJ/uON5PYg3.tmp OBJ/tZdIgONX.tmp OBJ/sluWwE3.tmp OBJ/9eOB4.tmp OBJ/8qeOJMv7P.tmp OBJ/Qj83Re6.tmp 
LIB_PATH = ./
LDSCRIPT = C:\Nyquest\NYIDE\include\nx1.ld
CFLAGS = $(INC_PATH) -O2 -g3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections
LDFLAGS = -O2 -nostartfiles -static -mcmodel=medium -T$(LDSCRIPT) -Wl,-Map -Wl,map.txt -Wl,--gc-sections -mvh
TARGET = LGW-DEMO-V02
PROJECT_NAME = LGW-DEMO-V02
TEMP_FOLDER = 

OBJ_DIR := $(sort $(dir $(OBJS)))
OBJ_DIR := $(OBJ_DIR:%/=%)

AS := $(CROSS_COMPILE)as
AS_INC_PATH := -I src/Table/Midi
ASFLAGS := $(AS_INC_PATH) -g

COC_ENABLE  := 0
DISA_ENABLE := 0

COC_LST     := $(OBJ_PATH)coc-info.xml
COC_PREFIX  := __redir_
COC_SEC     := .xip_redirect_tab
COC_TAB     := $(OBJ_PATH)__xip_redirect_tab.s
COC_SPLIT   := 1
DISA_FILE   := $(TARGET).disassembly.txt

COC_FLAGS	:= --list-tab $(COC_LST) --wrap-prefix $(COC_PREFIX) \
               --sect-name $(COC_SEC)
L := @

ifeq (1,$(COC_ENABLE))
	COC_TAB_OBJ := $(COC_TAB:%.s=%.o)
	CFLAGS += -fno-tree-switch-conversion
endif
ifeq (1,$(COC_SPLIT))
	COC_FLAGS += --split-table
endif

ifeq (1,$(DISA_ENABLE))
	DISA_TXT := $(DISA_FILE)
endif

all: $(TARGET).elf $(DISA_TXT)
	
$(OBJ_DIR):
	$(L)-mkdir -p $(subst /,\,$@)

$(TARGET).elf: $(OBJS) $(COC_TAB_OBJ)
	$(L)$(CC) $(OBJS) $(COC_TAB_OBJ) $(LIBS) $(LDFLAGS) -o $(TEMP_FOLDER)$@

$(DISA_FILE): $(TARGET).elf
	$(L)$(OBJDUMP) -d $< > $@

$(COC_TAB): $(OBJS) $(LIBS)
	$(L)$(COC_MOD) -o $@ $^ $(COC_FLAGS)

$(COC_TAB_OBJ): $(COC_TAB)
	$(L)$(CC) $(CFLAGS) -o $@ $<	

$(OBJ_PATH)/src/Kernel/Utility/prog_checksum.o: src/Kernel/Utility/prog_checksum.c | $(OBJ_DIR)
	@echo Building file ... $<
	@$(CC) $(CFLAGS) -mcmodel=large -MMD -MP -MF"$(@:%.o=%.d)" -o $@ $<

#$(OBJ_PATH)/src/Kernel/Utility/prog_checksum_xip.o: src/Kernel/Utility/prog_checksum_xip.c | $(OBJ_DIR)
#	@echo Building file ... $<
#	@$(CC) $(CFLAGS) -mcmodel=large -MMD -MP -MF"$(@:%.o=%.d)" -o $@ $<

$(OBJ_PATH)/%.o : %.c | $(OBJ_DIR)
	@echo Building file ... $<
	$(L)$(CC) $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -o $@ $<
	
$(OBJ_PATH)/%.o : $(OBJ_PATH)/%.c
	@echo Building file ... $<
	$(L)$(CC) $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -o $@ $<
	
$(OBJ_PATH)/%.o : %.C | $(OBJ_DIR)
	@echo Building file ... $<
	$(L)$(CC) $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -o $@ $<
	
$(OBJ_PATH)/%.o : %.s | $(OBJ_DIR)
	@echo Building file ... $<
	$(L)$(AS) $(ASFLAGS) --MD "$(@:%.o=%.d)" -o $@ $<
	
$(OBJ_PATH)/%.o : %.S | $(OBJ_DIR)
	@echo Building file ... $<
	$(L)$(CC) $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -o $@ $<
	
$(OBJ_PATH)/%.o : $(OBJ_PATH)/%.S
	@echo Building file ... $<
	$(L)$(CC) $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -o $@ $<

-include $(DEPS)
	
.PHONY: clean
clean:
	@echo Cleaning All Files ...
	@rm -f $(PROJECT_NAME).bin $(PROJECT_NAME).elf $(PROJECT_NAME).fpga \
		   $(PROJECT_NAME).dbg $(COC_LST) $(DISA_FILE) \
		   $(PROJECT_NAME)_Pack.bin $(PROJECT_NAME).rom 
	@rm -rf *.map *.out
	@rm -rf map.txt
	@rm -rf $(OBJ_PATH)/*.o
	@rm -rf $(OBJ_PATH)/*.d
	@rm -rf $(OBJ_PATH)/*.s
	@rm -rf *.htm
	@rm -rf $(OBJ_PATH)/src/*.o
	@rm -rf $(OBJ_PATH)/src/*.d
	@rm -rf $(OBJ_PATH)/src/Driver/*.o
	@rm -rf $(OBJ_PATH)/src/Driver/*.d
	@rm -rf $(OBJ_PATH)/src/Kernel/*.o
	@rm -rf $(OBJ_PATH)/src/Kernel/*.d
	@rm -rf $(OBJ_PATH)/src/Kernel/Audio/*.o
	@rm -rf $(OBJ_PATH)/src/Kernel/Audio/*.d
	@rm -rf $(OBJ_PATH)/src/Kernel/Callback/*.o
	@rm -rf $(OBJ_PATH)/src/Kernel/Callback/*.d
	@rm -rf $(OBJ_PATH)/src/Kernel/System/*.o
	@rm -rf $(OBJ_PATH)/src/Kernel/System/*.d
	@rm -rf $(OBJ_PATH)/src/Kernel/Utility/*.o
	@rm -rf $(OBJ_PATH)/src/Kernel/Utility/*.d
	@rm -rf $(OBJ_PATH)/src/Kernel/Utility/IRAM/*.o
	@rm -rf $(OBJ_PATH)/src/Kernel/Utility/IRAM/*.d
	@rm -rf $(OBJ_PATH)/src/Table/*.o
	@rm -rf $(OBJ_PATH)/src/Table/*.d
	@rm -rf $(OBJ_PATH)/src/Table/IO/*.o
	@rm -rf $(OBJ_PATH)/src/Table/IO/*.d
	@rm -rf $(OBJ_PATH)/src/Table/Midi/*.o
	@rm -rf $(OBJ_PATH)/src/Table/Midi/*.d
	@rm -rf $(OBJ_PATH)/src/Table/Voice/*.o
	@rm -rf $(OBJ_PATH)/src/Table/Voice/*.d
	@rm -rf $(OBJ_PATH)/src/Table/Midi/Table/*.o
	@rm -rf $(OBJ_PATH)/src/Table/Midi/Table/*.d
	@rm -rf $(OBJ_PATH)/src/Table/Vr/*.o
	@rm -rf $(OBJ_PATH)/src/Table/Vr/*.d
	@rm -rf $(OBJ_PATH)/src/Table/TouchTab/*.o
	@rm -rf $(OBJ_PATH)/src/Table/TouchTab/*.d
	@rm -rf $(OBJ_PATH)/src/Table/UserDefined/*.o
	@rm -rf $(OBJ_PATH)/src/Table/UserDefined/*.d
	@rm -rf $(OBJ_PATH)/src/Table/LEDSTR/*.o
	@rm -rf $(OBJ_PATH)/src/Table/LEDSTR/*.d
	@rm -rf $(OBJ_PATH)/src/user_code/*.o
	@rm -rf $(OBJ_PATH)/src/user_code/*.d
#	@echo Clean Finished
	
.PHONY: size
size:
	@$(CROSS_COMPILE)size $(TARGET).elf
