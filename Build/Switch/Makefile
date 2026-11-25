#---------------------------------------------------------------------------------
.SUFFIXES:
.DEFAULT_GOAL := all
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)

# PLATFORM selects the console:
#   switch  - Switch, with libnx (default)
#   switch2 - Switch 2 under kaiHEN, with ounce-devkit. The outputs end in -NS2.
PLATFORM	?=	switch

ifeq ($(PLATFORM),switch)
include $(DEVKITPRO)/libnx/switch_rules
PLATFORM_ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57
else ifeq ($(PLATFORM),switch2)
ifeq ($(wildcard $(DEVKITPRO)/libounce/switch2_rules),)
$(error PLATFORM=switch2 needs ounce-devkit, https://github.com/ouncebrew/ounce-devkit, installed in $(DEVKITPRO))
endif
include $(DEVKITPRO)/libounce/switch2_rules
PLATFORM_ARCH	:=	$(SWITCH2_CPU)
PLATFORM_SUFFIX	:=	-NS2
else
$(error PLATFORM must be "switch" or "switch2", not "$(PLATFORM)")
endif

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# ROMFS is the directory containing data to be added to RomFS, relative to the Makefile (Optional)
#
# NO_ICON: if set to anything, do not use icon.
# NO_NACP: if set to anything, no .nacp file is generated.
# APP_TITLE is the name of the app stored in the .nacp file (Optional)
# APP_AUTHOR is the author of the app stored in the .nacp file (Optional)
# APP_VERSION is the version of the app stored in the .nacp file (Optional)
# APP_TITLEID is the titleID of the app stored in the .nacp file (Optional)
# ICON is the filename of the icon (.jpg), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.jpg
#     - icon.jpg
#     - <libnx folder>/default_icon.jpg
#
# CONFIG_JSON is the filename of the NPDM config file (.json), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.json
#     - config.json
#   If a JSON file is provided or autodetected, an ExeFS PFS0 (.nsp) is built instead
#   of a homebrew executable (.nro). This is intended to be used for sysmodules.
#   NACP building is skipped as well.
#---------------------------------------------------------------------------------
# MENUS selects which set of menus to build:
#   new - the redesigned menus (default), JellyCar.nro
#   old - the menus from before the v1.3.0 redesign, JellyCar-OldMenus.nro
MENUS		?=	new

ifeq ($(MENUS),new)
MENUS_DIR	:=	New
APP_TITLE	:=	JellyCar
TARGET		:=	JellyCar$(PLATFORM_SUFFIX)
else ifeq ($(MENUS),old)
MENUS_DIR	:=	Old
APP_TITLE	:=	JellyCar (Old Menus)
TARGET		:=	JellyCar-OldMenus$(PLATFORM_SUFFIX)
else
$(error MENUS must be "new" or "old", not "$(MENUS)")
endif

APP_AUTHOR	:=	drakon
APP_VERSION	:=	1.4.0

BUILD		:=	build-$(MENUS)$(PLATFORM_SUFFIX)
SOURCES		:=	../../JellyCar/Menus/$(MENUS_DIR)
DATA		:=	data
ASSETS_DIR	:=	../../Assets
ANDROMEDA_LIB	?=	../../../Andromeda-Lib
INCLUDES	:=	$(ANDROMEDA_LIB) $(ANDROMEDA_LIB)/Libs $(ANDROMEDA_LIB)/Libs/freetype_gl $(ANDROMEDA_LIB)/Libs/stb $(ANDROMEDA_LIB)/Libs/tinyxml $(ANDROMEDA_LIB)/Libs/tiny_obj_loader \
				../..
ROMFS	:=	romfs
FILE_LOG	?=	0
DEFINES		+=	-DANDROMEDA_FILE_LOG=$(FILE_LOG)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	$(PLATFORM_ARCH) -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -O2 -ffunction-sections \
			$(ARCH) $(DEFINES)

# keep the folder of the build machine out of the binary
CFLAGS	+=	-ffile-prefix-map=$(TOPDIR)/=

CFLAGS	+=	$(INCLUDE) -D__SWITCH__ `aarch64-none-elf-pkg-config --cflags SDL2_mixer freetype2`

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11 -ffat-lto-objects -flto  -DANDROMEDA_SWITCH -D__SWITCH__ `aarch64-none-elf-pkg-config --cflags SDL2_mixer freetype2`
CFLAGS	+=	$(INCLUDE) -D__SWITCH__

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(LIBNX)/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lnx -lglad -lEGL -lglapi -ldrm_nouveau -lpthread -lz -lm -lstdc++ -lfreetype -lpng -lbz2 `aarch64-none-elf-pkg-config --libs  sdl2 SDL2_mixer freetype2` 


#---------------------------------------------------------------------------------
# source files
#---------------------------------------------------------------------------------

SRC_RENDERER	:=\
	$(ANDROMEDA_LIB)/Andromeda/FileSystem/FileManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/FileSystem/BaseFile.cpp \
	$(ANDROMEDA_LIB)/Andromeda/FileSystem/Stdio/StdioFile.cpp \
	$(ANDROMEDA_LIB)/Andromeda/FileSystem/Stdio/StdioDirectory.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Utils/Logger.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Utils/SimpleProfiler.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Audio/Sound.o \
	$(ANDROMEDA_LIB)/Andromeda/Audio/AudioManager.o \
	$(ANDROMEDA_LIB)/Andromeda/Audio/Sdl/SdlSound.o \
	$(ANDROMEDA_LIB)/Andromeda/Audio/Sdl/SdlAudioManager.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Camera3d.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Texture.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/RenderManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Shader.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/ShaderManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Sprite.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/TextureManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/VertexArrayObject.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/VertexBufferObject.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/FrameBufferObject.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/GL/RenderManagerGL3.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/GL/ShaderGlsl.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/GL/VertexArrayObjectGL3.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/GL/VertexBufferObjectGL3.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/GL/FrameBufferObjectGL3.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Text/TextureAtlas.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Text/TexturedFont.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Text/FontManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Models/ModelObj.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Models/ModelMaterial.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/GameLoader.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/GameManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/GameState.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/SystemManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/MemoryManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/Timer.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/SWITCH/GameLoaderSwitch.o \
	$(ANDROMEDA_LIB)/Andromeda/System/SWITCH/MemoryManagerSwitch.o \
	$(ANDROMEDA_LIB)/Andromeda/Input/InputManager.o \
	$(ANDROMEDA_LIB)/Andromeda/Input/SWITCH/InputManagerSwitch.o \
	$(ANDROMEDA_LIB)/Andromeda/Input/SWITCH/GamepadDeviceSwitch.o \
	$(ANDROMEDA_LIB)/Andromeda/Input/SWITCH/TouchDeviceSwitch.o \
	$(ANDROMEDA_LIB)/Libs/tinyxml/tinystr.cpp \
	$(ANDROMEDA_LIB)/Libs/tinyxml/tinyxmlerror.cpp \
	$(ANDROMEDA_LIB)/Libs/tinyxml/tinyxmlparser.cpp \
	$(ANDROMEDA_LIB)/Libs/tinyxml/tinyxml.cpp \
	$(ANDROMEDA_LIB)/Libs/tiny_obj_loader/tiny_obj_loader.cpp\
	../../JellyPhysics/AABB.cpp \
	../../JellyPhysics/Body.cpp \
	../../JellyPhysics/ClosedShape.cpp \
	../../JellyPhysics/InternalSpring.cpp \
	../../JellyPhysics/PointMass.cpp \
	../../JellyPhysics/PressureBody.cpp \
	../../JellyPhysics/SpringBody.cpp \
	../../JellyPhysics/Vector2.cpp \
	../../JellyPhysics/VectorTools.cpp \
	../../JellyPhysics/World.cpp \
	../../JellyCar/Car/Car.cpp \
	../../JellyCar/Car/Chassis.cpp \
	../../JellyCar/Car/GameBallonBody.cpp \
	../../JellyCar/Car/SuspensionShock.cpp \
	../../JellyCar/Car/Tire.cpp \
	../../JellyCar/Levels/KinematicControl.cpp \
	../../JellyCar/Levels/KinematicMotor.cpp \
	../../JellyCar/Levels/KinematicPlatform.cpp \
	../../JellyCar/Levels/LevelManager.cpp \
	../../JellyCar/Levels/LevelSoftBody.cpp \
	../../JellyCar/Levels/GamePressureBody.cpp \
	../../JellyCar/Levels/GameSpringBody.cpp \
	../../JellyCar/Utils/JellyHelper.cpp \
	../../JellyCar/Utils/RectangleDrawer.cpp \
	../../JellyCar/Utils/UiPrompt.cpp \
	../../JellyCar/Utils/InputHelper.cpp \
	../../JellyCar/Utils/AudioHelper.cpp \
	../../JellyCar/Mods/ModLoader.cpp \
	../../JellyCar/Game/JellyCore.cpp \
	../../JellyCar/Game/JellyGame.cpp \
	../../JellyCar/Game/JellyOptions.cpp \
	../../JellyCar/Game/JellyMenuBetter.cpp \
	../../JellyCar/Game/JellyIntro.cpp \
	../../JellyCar/Game/JellySplash.cpp \
	../../JellyCar/JellyGameManager.cpp \
	../../JellyCar/JellyCar.cpp

SOURCEFILES_CPP	:=\
	$(SRC_RENDERER) 

SOURCEFILES_C	:=\
	$(ANDROMEDA_LIB)/Libs/freetype_gl/distance-field.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/edtaa3func.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/font-manager.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/platform.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/texture-atlas.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/texture-font.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/utf8-utils.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/vector.c

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

resolve_path = $(if $(filter /%,$(1)),$(1),$(CURDIR)/$(1))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)
export ROMFS_SRC	:=	$(call resolve_path,$(ASSETS_DIR))
export ROMFS_DST	:=	$(CURDIR)/$(ROMFS)/Assets
export ROMFS_STAMP	:=	$(CURDIR)/$(ROMFS)/.assets-staged

export VPATH	:=	$(foreach dir,$(SOURCES),$(call resolve_path,$(dir))) \
					$(foreach sf,$(SOURCEFILES_CPP),$(call resolve_path,$(dir $(sf)))) \
					$(foreach sf,$(SOURCEFILES_C),$(call resolve_path,$(dir $(sf)))) \
					$(foreach dir,$(DATA),$(call resolve_path,$(dir)))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c))) $(foreach f,$(SOURCEFILES_C),$(notdir $(f)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp))) $(foreach f,$(SOURCEFILES_CPP),$(notdir $(f)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(call resolve_path,$(dir))) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(CONFIG_JSON)),)
	jsons := $(wildcard *.json)
	ifneq (,$(findstring $(TARGET).json,$(jsons)))
		export APP_JSON := $(TOPDIR)/$(TARGET).json
	else
		ifneq (,$(findstring config.json,$(jsons)))
			export APP_JSON := $(TOPDIR)/config.json
		endif
	endif
else
	export APP_JSON := $(TOPDIR)/$(CONFIG_JSON)
endif

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/$(TARGET).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(call resolve_path,$(ROMFS))
endif

.PHONY: $(BUILD) clean all both everything

#---------------------------------------------------------------------------------
all: $(BUILD)

both:
	@$(MAKE) --no-print-directory MENUS=new all
	@$(MAKE) --no-print-directory MENUS=old all

everything:
	@$(MAKE) --no-print-directory PLATFORM=switch both
	@$(MAKE) --no-print-directory PLATFORM=switch2 both

$(ROMFS_STAMP): $(shell find $(ROMFS_SRC) -type f 2>/dev/null)
	@[ -d $(ROMFS_SRC) ] || (echo "Missing assets directory: $(ROMFS_SRC)" && false)
	@mkdir -p $(CURDIR)/$(ROMFS)
	@rm -rf $(ROMFS_DST)
	@cp -a $(ROMFS_SRC) $(ROMFS_DST)
	@touch $(ROMFS_STAMP)

$(BUILD): $(ROMFS_STAMP)
	@[ -d $(call resolve_path,$(ANDROMEDA_LIB))/Andromeda ] || (echo "Andromeda-Lib not found at $(ANDROMEDA_LIB). Clone https://github.com/Common-Leap/Andromeda-Lib there or set ANDROMEDA_LIB."; false)
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr build-new build-old build-new-NS2 build-old-NS2 $(ROMFS)
	@rm -f $(foreach t,JellyCar JellyCar-OldMenus JellyCar-NS2 JellyCar-OldMenus-NS2,$(t).nro $(t).nacp $(t).elf)

#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
ifeq ($(strip $(APP_JSON)),)

all	:	$(OUTPUT).nro

ifeq ($(strip $(NO_NACP)),)
$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp
else
$(OUTPUT).nro	:	$(OUTPUT).elf
endif

else

all	:	$(OUTPUT).nsp

$(OUTPUT).nsp	:	$(OUTPUT).nso $(OUTPUT).npdm

$(OUTPUT).nso	:	$(OUTPUT).elf

endif

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
